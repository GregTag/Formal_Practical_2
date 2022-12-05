#include "simple_alphabet.hpp"

#include <utility>

size_t SimpleAlphabet::add(const std::string& symbol, bool is_nonterminal) {
  if (!is_nonterminal && symbol.size() != 1)
    throw std::invalid_argument("Terminal must be a character.");

  auto [iterator, is_placed] = _symbol_ids.try_emplace(symbol, _symbols.size());
  if (!is_placed) return iterator->second;

  _symbols.push_back(symbol);
  _is_nonterminal.push_back(is_nonterminal);
  return _symbols.size() - 1;
}

size_t SimpleAlphabet::size() const { return _symbols.size(); }

size_t SimpleAlphabet::getId(const std::string& symbol) const {
  return _symbol_ids.at(symbol);
}

bool SimpleAlphabet::testSymbol(const std::string& symbol) const {
  return _symbol_ids.count(symbol);
}

const std::string& SimpleAlphabet::getSymbol(size_t id) const {
  return _symbols.at(id);
}

bool SimpleAlphabet::isNonterminal(size_t id) const {
  return _is_nonterminal.at(id);
}
