#pragma once
#include <ostream>
#include <queue>
#include <unordered_set>

#include "parser.hpp"

struct Situation {
  size_t rule_id;
  size_t point_pos;
  size_t origin_pos;
};

class EarleyParser : public Parser {
 public:
  EarleyParser(std::shared_ptr<Grammar>);
  bool recognize(const std::string& word) override;
  void enable_logging(std::ostream&);

 private:
  void initialize(size_t size);
  void add(const Situation&, size_t input_pos);
  void scan(char, size_t input_pos);
  void predict(size_t input_pos);
  void complete(size_t input_pos);

  size_t getNextSymbol(const Situation&);
  bool isChanging();
  void logSituation(const Situation&, size_t);

  std::ostream _output;
  std::shared_ptr<Grammar> _grammar;
  std::unordered_map<size_t, std::vector<size_t>> _rule_map;
  size_t _starting_rule;
  // dp[origin_pos][rule_id][point_pos]
  std::vector<std::vector<std::vector<ssize_t>>> _dynamics;
  std::vector<std::unordered_map<size_t, std::vector<Situation>>> _layers;
  bool _layer_changed;
  std::unordered_set<size_t> _completed_nonterminals;
  std::queue<Situation> _scan_queue;
  std::queue<Situation> _predict_queue;
  std::queue<Situation> _complete_queue;
};
