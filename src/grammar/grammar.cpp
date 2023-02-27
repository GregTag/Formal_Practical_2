#include "grammar.hpp"

Production::Production(size_t symbol, std::vector<size_t>&& rule)
    : symbol(symbol), rule(std::move(rule)) {}

Production::Production(size_t symbol, const std::vector<size_t>& rule)
    : symbol(symbol), rule(rule) {}

Grammar::Grammar() : _alphabet(std::make_shared<SimpleAlphabet>()) {}

Grammar::Grammar(std::shared_ptr<GrammarAlphabet> alphabet)
    : _alphabet(alphabet) {}

size_t Grammar::addSymbol(const std::string& symbol, bool is_nonterminal) {
  return _alphabet->add(symbol, is_nonterminal);
}

void Grammar::addProduction(size_t symbol, const std::vector<size_t>& rule) {
  addProductionImpl(symbol, rule);
}

void Grammar::addProduction(size_t symbol, std::vector<size_t>&& rule) {
  addProductionImpl(symbol, std::move(rule));
}

void Grammar::setStartingNonterminal(size_t nonterminal) {
  if (!_alphabet->isNonterminal(nonterminal))
    throw std::invalid_argument("Starting nonterminal cannot be a terminal.");
  _starting_nonterminal = nonterminal;
}

size_t Grammar::makeSingleStartRule(const std::string& symbol) {
  if (_rule_map[_starting_nonterminal].size() == 1)
    return _rule_map[_starting_nonterminal].front();
  if (_alphabet->testSymbol(symbol))
    throw std::invalid_argument("New nonterminal cannot be from the alphabet.");
  size_t new_nonterminal = addSymbol(symbol, true);
  addProduction(new_nonterminal, std::vector<size_t>(1, _starting_nonterminal));
  _starting_nonterminal = new_nonterminal;
  return _productions.size() - 1;
}

const GrammarAlphabet& Grammar::getAlphabet() const { return *_alphabet; }

const std::vector<Production>& Grammar::getProductions() const {
  return _productions;
}

const std::vector<size_t>& Grammar::getRules(size_t symbol) const {
  return _rule_map.at(symbol);
}

size_t Grammar::getStartingNonterminal() const {
  if (!_alphabet->isNonterminal(_starting_nonterminal))
    throw std::invalid_argument("Starting nonterminal cannot be a terminal.");
  return _starting_nonterminal;
}

void Grammar::checkRule(const std::vector<size_t>& rule) {
  try {
    for (size_t symbol : rule) {
      _alphabet->isNonterminal(symbol);
    }
  } catch (...) {
    throw std::invalid_argument("Incorrect rule.");
  }
}

template <typename Arg>
void Grammar::addProductionImpl(size_t symbol, Arg&& rule) {
  checkRule(rule);
  _productions.emplace_back(symbol, std::forward<Arg>(rule));
  _rule_map[symbol].push_back(_productions.size() - 1);
}
