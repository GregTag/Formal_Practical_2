#include "lr_analyzer.hpp"

LRAnalyzer::LRAnalyzer(std::shared_ptr<Grammar> grammar)
    : _grammar(std::move(grammar)), _alphabet(_grammar->getAlphabet()) {
  buildInitialize();
  precalcFirst();
  buildAnalizer();
  clear_excess();
}

// Замыкание: (A -> w.Bv, a) --> for b in First(va) : (B -> .u, b)
void LRAnalyzer::buildInitialize() {
  _starting_rule = _grammar->makeSingleStartRule("S'");

  for (size_t id = 0; id < _alphabet.size(); ++id) {
    if (!_alphabet.isNonterminal(id)) {
      _terminals.push_back(id);
    }
  }
  _terminals.push_back(_alphabet.size());

  const auto& productions = _grammar->getProductions();
  std::vector<std::vector<std::unordered_set<size_t>>> prepared(
      productions.size());
  for (size_t i = 0; i < productions.size(); ++i) {
    prepared[i].resize(productions[i].rule.size() + 1);
  }
  _mapping.assign(_terminals.size(), prepared);

  createState();
  _states.front().emplace_back(
      Situation{_starting_rule, 0, _terminals.size() - 1});
  add(_states.front().front(), 0);
}

size_t LRAnalyzer::createState() {
  _goto.emplace_back(_alphabet.size() + 1, -1);
  _table.emplace_back(_terminals.size(), Transition::NOT_INITIALIZED);
  _states.emplace_back();
  return _states.size() - 1;
}

size_t LRAnalyzer::findState(const std::vector<Situation>& situations) {
  for (size_t state = 0; state < _states.size(); ++state) {
    bool st = true;
    for (const auto& situation : situations) {
      if (!getMapping(situation).count(state)) {
        st = false;
        break;
      }
    }
    if (st) return state;
  }

  createState();
  for (const auto& situation : situations) {
    if (add(situation, _states.size() - 1)) {
      _states.back().emplace_back(situation);
    }
  };
  return _states.size() - 1;
}

void LRAnalyzer::buildAnalizer() {
  for (size_t state = 0; state < _states.size(); ++state) {
    std::vector<std::vector<Situation>> edges(_alphabet.size());
    buildNode(state, edges);
    ssize_t id = -1;
    for (size_t i = 0; i < _alphabet.size(); ++i) {
      if (!edges[i].size()) {
        if (!_alphabet.isNonterminal(i)) {
          ++id;
          if (_table[state][id] == Transition::NOT_INITIALIZED)
            setTables(state, id, Transition::REJECT, -1);
        }
        continue;
      }
      size_t connected = findState(edges[i]);
      if (!_alphabet.isNonterminal(i)) {
        ++id;
        setTables(state, id, Transition::SHIFT, connected);
      } else {
        _goto[state][i] = connected;
      }
    }
    if (_table[state][_terminals.size() - 1] == Transition::NOT_INITIALIZED)
      setTables(state, _terminals.size() - 1, Transition::REJECT, -1);
  }
}

// Обрабатывает добавление ситуации к состоянию. Если ситуация заполнена, то
// добавляет REDUCE в таблицы. Иначе добавляет в _states. Предотвращает
// дублирование ситуаций.
bool LRAnalyzer::add(const Situation& situation, size_t state) {
  auto& mapping = getMapping(situation);
  if (mapping.count(state)) return false;
  mapping.insert(state);
  if (_grammar->getProductions()[situation.rule_id].rule.size() ==
      situation.point_pos) {
    Transition transition;
    if (situation.next_id == _terminals.size() - 1 &&
        situation.rule_id == _starting_rule)
      transition = Transition::ACCEPT;
    else
      transition = Transition::REDUCE;
    setTables(state, situation.next_id, transition, situation.rule_id);
    return false;
  }
  return true;
}

// Замыкает состояние. До вызова в _states должны лежать все незаполненные
// порождающие ситуации.
void LRAnalyzer::buildNode(size_t state,
                           std::vector<std::vector<Situation>>& edges) {
  std::queue<Situation> to_close;
  for (const auto& situation : _states[state]) {
    to_close.push(situation);
  }

  while (!to_close.empty()) {
    Situation situation = to_close.front();
    to_close.pop();
    size_t next_symbol = getNextSymbol(situation);
    ++situation.point_pos;
    if (_alphabet.isNonterminal(next_symbol)) {
      auto first = calcFirst(situation);
      for (auto rule_id : _grammar->getRules(next_symbol)) {
        for (size_t next_character : first) {
          Situation new_situation{rule_id, 0, next_character};
          if (add(new_situation, state)) to_close.push(new_situation);
        }
      }
    }
    edges[next_symbol].emplace_back(std::move(situation));
  }
}

size_t LRAnalyzer::getNextSymbol(const Situation& situation) {
  return _grammar->getProductions()[situation.rule_id]
      .rule[situation.point_pos];
}

void LRAnalyzer::precalcFirst() {
  _first_precalc.resize(_alphabet.size());
  _is_generating_emptyness.assign(_alphabet.size(), false);
  std::vector<size_t> nonterminals;

  ssize_t id = -1;
  for (size_t symbol = 0; symbol < _alphabet.size(); ++symbol) {
    if (_alphabet.isNonterminal(symbol)) {
      nonterminals.push_back(symbol);
    } else {
      ++id;
      _first_precalc[symbol].insert(id);
    }
  }

  bool is_changing = true;
  while (is_changing) {
    is_changing = false;
    for (size_t nonterminal : nonterminals) {
      for (size_t rule_id : _grammar->getRules(nonterminal)) {
        const auto& rule = _grammar->getProductions()[rule_id].rule;
        size_t point_pos = 0;
        while (point_pos < rule.size()) {
          size_t diff = _first_precalc[nonterminal].size();
          _first_precalc[nonterminal].insert(
              _first_precalc[rule[point_pos]].begin(),
              _first_precalc[rule[point_pos]].end());
          diff = _first_precalc[nonterminal].size() - diff;
          if (diff) {
            is_changing = true;
          }
          if (!_is_generating_emptyness[rule[point_pos]]) break;
          ++point_pos;
        }
        if (point_pos == rule.size() &&
            !_is_generating_emptyness[nonterminal]) {
          _is_generating_emptyness[nonterminal] = true;
          is_changing = true;
        }
      }
    }
  }
}

std::vector<size_t> LRAnalyzer::calcFirst(const Situation& situation) {
  std::vector<size_t> first;
  const auto& rule = _grammar->getProductions()[situation.rule_id].rule;
  size_t point_pos = situation.point_pos;
  while (point_pos < rule.size()) {
    first.insert(first.end(), _first_precalc[rule[point_pos]].begin(),
                 _first_precalc[rule[point_pos]].end());
    if (!_is_generating_emptyness[rule[point_pos]]) break;
    ++point_pos;
  }
  if (point_pos == rule.size()) {
    first.push_back(situation.next_id);
  }
  return first;
}

void LRAnalyzer::setTables(size_t state, size_t id, Transition transition,
                           size_t value) {
  if (_table[state][id] == transition) return;
  if (_table[state][id] != Transition::NOT_INITIALIZED ||
      (_goto[state][_terminals[id]] != -1 &&
       _goto[state][_terminals[id]] != value))
    throw std::invalid_argument("Table conflict.");
  _table[state][id] = transition;
  _goto[state][_terminals[id]] = value;
}

std::unordered_set<size_t>& LRAnalyzer::getMapping(const Situation& situation) {
  return _mapping[situation.next_id][situation.rule_id][situation.point_pos];
}

void LRAnalyzer::clear_excess() {
  _first_precalc.clear();
  _is_generating_emptyness.clear();
  _states.clear();
  _mapping.clear();
}
