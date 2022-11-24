#pragma once
#include <istream>
#include <sstream>
#include <stdexcept>

#include "grammar.hpp"

class BuilderFromBNF {
 public:
  BuilderFromBNF();
  BuilderFromBNF(std::istream&);

  void reset(std::istream&);
  void readRules();
  std::shared_ptr<Grammar> getResult();

 private:
  std::istream _input;
  std::shared_ptr<Grammar> _grammar;
};
