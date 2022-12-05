#pragma once
#include <queue>
#include <unordered_set>

#include "grammar.hpp"

enum class Transition { NOT_INITIALIZED, SHIFT, REDUCE, ACCEPT, REJECT };

struct Situation {
  size_t rule_id;
  size_t point_pos;
  size_t next_id;
};

class LRAnalyzer {
 public:
  LRAnalyzer(std::shared_ptr<Grammar>);

  friend class LRParser;

 private:
  void initialize();
  void buildInitialize();
  size_t createState();
  size_t findState(const std::vector<Situation>&);
  void buildAnalizer();
  bool add(const Situation&, size_t);
  void buildNode(size_t, std::vector<std::vector<Situation>>&);
  size_t getNextSymbol(const Situation&);
  void precalcFirst();
  std::vector<size_t> calcFirst(const Situation&);
  void setTables(size_t, size_t, Transition, size_t);
  std::unordered_set<size_t>& getMapping(const Situation&);
  void clear_excess();

  std::shared_ptr<Grammar> _grammar;
  // $ == _alphabet.size() == _terminals.back()
  const GrammarAlphabet& _alphabet;
  std::vector<size_t> _terminals;
  std::vector<std::unordered_set<size_t>> _first_precalc;
  std::vector<bool> _is_generating_emptyness;
  size_t _starting_rule;
  // Для shift переходов, будет записано состояние, в которое нужно перейти.
  // Для reduce переходов, будет записано правило, которое нужно свернуть.
  // _goto[state][symbol]
  std::vector<std::vector<ssize_t>> _goto;
  // _table[state][terminal_id]
  std::vector<std::vector<Transition>> _table;
  // Хранит порождающие незаполненные ситуации
  std::vector<std::vector<Situation>> _states;
  std::vector<std::vector<std::vector<std::unordered_set<size_t>>>> _mapping;
};
