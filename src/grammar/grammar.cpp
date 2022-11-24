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
  checkRule(rule);
  _productions.emplace_back(symbol, rule);
}

void Grammar::addProduction(size_t symbol, std::vector<size_t>&& rule) {
  checkRule(rule);
  _productions.emplace_back(symbol, std::move(rule));
}

void Grammar::setStartingNonterminal(size_t nonterminal) {
  if (!_alphabet->isNonterminal(nonterminal))
    throw std::invalid_argument("Starting nonterminal cannot be a terminal.");
  _starting_nonterminal = nonterminal;
}

const GrammarAlphabet& Grammar::getAlphabet() const { return *_alphabet; }

const std::vector<Production>& Grammar::getProductions() const {
  return _productions;
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
