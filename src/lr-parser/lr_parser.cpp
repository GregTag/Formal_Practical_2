#include "lr_parser.hpp"

LRParser::LRParser(std::shared_ptr<Grammar> grammar)
    : _grammar(std::move(grammar)), _alphabet(_grammar->getAlphabet()) {
  LRAnalyzer analyzer(_grammar);
  _goto = std::move(analyzer._goto);
  _table = std::move(analyzer._table);
  buildMapping(analyzer._terminals);
}

LRParser::LRParser(const LRAnalyzer& analyzer)
    : _grammar(analyzer._grammar),
      _alphabet(_grammar->getAlphabet()),
      _goto(analyzer._goto),
      _table(analyzer._table) {
  buildMapping(analyzer._terminals);
}

LRParser::LRParser(LRAnalyzer&& analyzer)
    : _grammar(std::move(analyzer._grammar)),
      _alphabet(_grammar->getAlphabet()),
      _goto(std::move(analyzer._goto)),
      _table(std::move(analyzer._table)) {
  buildMapping(analyzer._terminals);
}

bool LRParser::recognize(const std::string& word) {
  while (!_stack.empty()) _stack.pop();
  _stack.push(0);
  size_t pos = 0;
  while (pos <= word.size()) {
    switch (_table[_stack.top()][getId(word, pos)]) {
      case Transition::SHIFT: {
        _stack.push(_goto[_stack.top()][getSymbol(word, pos)]);
        ++pos;
        break;
      }
      case Transition::REDUCE: {
        const auto& production =
            _grammar
                ->getProductions()[_goto[_stack.top()][getSymbol(word, pos)]];
        for (size_t i = 0; i < production.rule.size(); ++i) {
          _stack.pop();
        }
        _stack.push(_goto[_stack.top()][production.symbol]);
        break;
      }
      case Transition::ACCEPT:
        return true;

      case Transition::REJECT:
        return false;
    }
  }
  return false;
}

size_t LRParser::getId(const std::string& word, size_t pos) {
  if (pos == word.size()) return _id_by_char.size();
  return _id_by_char[word[pos]];
}

size_t LRParser::getSymbol(const std::string& word, size_t pos) {
  if (pos == word.size()) return _alphabet.size();
  return _symbol_by_char[word[pos]];
}

void LRParser::buildMapping(const std::vector<size_t>& terminals) {
  for (size_t i = 0; i < terminals.size() - 1; ++i) {
    char character = _alphabet.getSymbol(terminals[i]).front();
    _id_by_char.emplace(character, i);
    _symbol_by_char.emplace(character, terminals[i]);
  }
}
