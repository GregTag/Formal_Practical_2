#pragma once
#include <string>
#include <vector>

class GrammarAlphabet {
 public:
  virtual ~GrammarAlphabet() = default;
  virtual size_t add(const std::string& symbol, bool is_nonterminal) = 0;
  virtual size_t getId(const std::string& symbol) const = 0;
  virtual const std::string& getSymbol(size_t id) const = 0;
  virtual bool isNonterminal(size_t id) const = 0;
};
