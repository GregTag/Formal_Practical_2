#pragma once
#include "grammar.hpp"

class Parser {
 public:
  virtual ~Parser() = default;
  virtual bool recognize(const std::string& word) = 0;
};
