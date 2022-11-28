#include "earley_parser.hpp"

EarleyParser::EarleyParser(std::shared_ptr<Grammar> grammar)
    : _output(nullptr), _grammar(std::move(grammar)) {
  const auto& productions = _grammar->getProductions();
  for (size_t i = 0; i < productions.size(); ++i) {
    _rule_map[productions[i].symbol].push_back(i);
  }

  size_t start = _grammar->getStartingNonterminal();
  if (_rule_map[start].size() == 1) {
    _starting_rule = _rule_map[start].front();
  } else {
    _starting_rule = _grammar->makeSingleStartRule("S'");
  }
}

bool EarleyParser::recognize(const std::string& word) {
  initialize(word.size());
  while (isChanging()) {
    complete(0);
    predict(0);
  }
  for (size_t input_pos = 1; input_pos <= word.size(); ++input_pos) {
    _completed_nonterminals.clear();
    scan(word[input_pos - 1], input_pos);
    while (isChanging()) {
      complete(input_pos);
      predict(input_pos);
    }
  }
  return _dynamics.front()[_starting_rule].back() == word.size();
}

void EarleyParser::enable_logging(std::ostream& output) {
  _output.rdbuf(output.rdbuf());
}

void EarleyParser::initialize(size_t size) {
  const auto& productions = _grammar->getProductions();
  std::vector<std::vector<ssize_t>> prepared(productions.size());
  for (size_t i = 0; i < productions.size(); ++i) {
    prepared[i].assign(productions[i].rule.size() + 1, -1);
  }
  _dynamics.assign(size + 1, prepared);
  _layers.assign(size + 1,
                 std::unordered_map<size_t, std::vector<Situation>>());
  _layer_changed = false;
  add(Situation{_starting_rule, 0, 0}, 0);
}

void EarleyParser::add(const Situation& situation, size_t input_pos) {
  const Production& production = _grammar->getProductions()[situation.rule_id];
  if (_dynamics[situation.origin_pos][situation.rule_id][situation.point_pos] ==
      input_pos)
    return;
  _dynamics[situation.origin_pos][situation.rule_id][situation.point_pos] =
      input_pos;
  if (situation.point_pos == production.rule.size()) {
    _complete_queue.push(situation);
    if (situation.origin_pos == input_pos)
      _completed_nonterminals.emplace(production.symbol);
  } else if (_grammar->getAlphabet().isNonterminal(
                 production.rule[situation.point_pos])) {
    _predict_queue.push(situation);
    _layers[input_pos][production.rule[situation.point_pos]].push_back(
        situation);
    _layer_changed = true;
  } else {
    _scan_queue.push(situation);
  }
  logSituation(situation, input_pos);
}

// (A -> u.av, i, j) --> (A -> ua.v, i, j+1)
void EarleyParser::scan(char character, size_t input_pos) {
  std::queue<Situation> queue(std::move(_scan_queue));
  while (!queue.empty()) {
    Situation situation = queue.front();
    queue.pop();
    size_t terminal = getNextSymbol(situation);
    if (_grammar->getAlphabet().getSymbol(terminal)[0] == character) {
      ++situation.point_pos;
      add(situation, input_pos);
    }
  }
}

// (A -> u.Bv, i, j) --> (B -> .w, j, j)
void EarleyParser::predict(size_t input_pos) {
  while (!_predict_queue.empty()) {
    Situation situation = _predict_queue.front();
    _predict_queue.pop();
    size_t nonterminal = getNextSymbol(situation);
    for (size_t rule_id : _rule_map[nonterminal]) {
      add(Situation{rule_id, 0, input_pos}, input_pos);
    }
  }
}

// (B -> w., k, j) ^ (A -> u.Bv, i, k) --> (A -> uB.v, i, j)
void EarleyParser::complete(size_t input_pos) {
  while (!_complete_queue.empty()) {
    Situation situation = _complete_queue.front();
    _complete_queue.pop();
    if (situation.origin_pos == input_pos) continue;
    size_t nonterminal = _grammar->getProductions()[situation.rule_id].symbol;
    for (auto proposed_situation : _layers[situation.origin_pos][nonterminal]) {
      ++proposed_situation.point_pos;
      add(proposed_situation, input_pos);
    }
  }
  _layer_changed = false;
  for (size_t nonterminal : _completed_nonterminals) {
    for (auto proposed_situation : _layers[input_pos][nonterminal]) {
      ++proposed_situation.point_pos;
      add(proposed_situation, input_pos);
    }
  }
}

size_t EarleyParser::getNextSymbol(const Situation& situation) {
  return _grammar->getProductions()[situation.rule_id]
      .rule[situation.point_pos];
}

bool EarleyParser::isChanging() {
  return _layer_changed || !_predict_queue.empty() || !_complete_queue.empty();
}

void EarleyParser::logSituation(const Situation& situation, size_t input_pos) {
  if (!_output) return;
  _output << "[" << input_pos << "] (" << situation.rule_id << ", "
          << situation.point_pos << ", " << situation.origin_pos << ")\n";
}
