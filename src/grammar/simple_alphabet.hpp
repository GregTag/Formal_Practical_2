#pragma once
#include <stdexcept>
#include <unordered_map>

#include "alphabet.hpp"

class SimpleAlphabet : public GrammarAlphabet {
 public:
  SimpleAlphabet() = default;

  size_t add(const std::string& symbol, bool is_nonterminal) override;
  size_t getId(const std::string& symbol) const override;
  bool testSymbol(const std::string& symbol) const override;
  const std::string& getSymbol(size_t id) const override;
  bool isNonterminal(size_t id) const override;

 private:
  std::vector<std::string> _symbols;
  std::vector<bool> _is_nonterminal;
  std::unordered_map<std::string, size_t> _symbol_ids;
};
