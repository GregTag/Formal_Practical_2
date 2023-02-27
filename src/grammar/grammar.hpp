#pragma once
#include <memory>
#include <vector>

#include "simple_alphabet.hpp"

struct Production {
  Production() = default;
  Production(size_t, std::vector<size_t>&&);
  Production(size_t, const std::vector<size_t>&);

  size_t symbol;
  std::vector<size_t> rule;
};

class Grammar {
 public:
  Grammar();
  Grammar(std::shared_ptr<GrammarAlphabet>);

  size_t addSymbol(const std::string&, bool);
  void addProduction(size_t, const std::vector<size_t>&);
  void addProduction(size_t, std::vector<size_t>&&);
  void setStartingNonterminal(size_t);
  size_t makeSingleStartRule(const std::string&);
  const GrammarAlphabet& getAlphabet() const;
  const std::vector<Production>& getProductions() const;
  const std::vector<size_t>& getRules(size_t) const;
  size_t getStartingNonterminal() const;

 private:
  void checkRule(const std::vector<size_t>&);
  template <typename Arg>
  void addProductionImpl(size_t, Arg&&);

  size_t _starting_nonterminal = 0;
  std::shared_ptr<GrammarAlphabet> _alphabet;
  std::vector<Production> _productions;
  std::unordered_map<size_t, std::vector<size_t>> _rule_map;
};
