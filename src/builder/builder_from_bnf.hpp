#pragma once
#include <istream>
#include <sstream>
#include <stdexcept>

#include "grammar.hpp"

class BuilderFromBNF {
 public:
  BuilderFromBNF();
  BuilderFromBNF(std::istream&);
  BuilderFromBNF(const std::string&);

  std::shared_ptr<Grammar> build();
  std::shared_ptr<Grammar> build(std::istream&);
  std::shared_ptr<Grammar> build(const std::string&);

 private:
  BuilderFromBNF& reset(std::istream&);
  BuilderFromBNF& reset(const std::string&);
  BuilderFromBNF& readRules();
  std::shared_ptr<Grammar> getResult();

  std::stringbuf _buf;
  std::istream _input;
  std::shared_ptr<Grammar> _grammar;
};
