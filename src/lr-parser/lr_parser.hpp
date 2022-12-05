#pragma once
#include <stack>

#include "lr_analyzer.hpp"
#include "parser.hpp"

class LRParser : public Parser {
 public:
  LRParser(std::shared_ptr<Grammar>);
  LRParser(const LRAnalyzer&);
  LRParser(LRAnalyzer&&);

  bool recognize(const std::string& word) override;

 private:
  size_t getId(const std::string& word, size_t pos);
  size_t getSymbol(const std::string& word, size_t pos);
  void buildMapping(const std::vector<size_t>& terminals);

  std::shared_ptr<Grammar> _grammar;
  const GrammarAlphabet& _alphabet;
  std::vector<std::vector<ssize_t>> _goto;
  std::vector<std::vector<Transition>> _table;
  std::unordered_map<size_t, size_t> _id_by_char;
  std::unordered_map<size_t, size_t> _symbol_by_char;
  std::stack<size_t> _stack;
};
