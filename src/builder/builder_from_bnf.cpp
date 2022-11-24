#include "builder_from_bnf.hpp"

BuilderFromBNF::BuilderFromBNF() : _input(nullptr), _grammar(nullptr) {}

BuilderFromBNF::BuilderFromBNF(std::istream& stream)
    : _input(stream.rdbuf()), _grammar(std::make_shared<Grammar>()) {}

void BuilderFromBNF::reset(std::istream& stream) {
  _input.rdbuf(stream.rdbuf());
  _grammar = std::make_shared<Grammar>();
}

void BuilderFromBNF::readRules() {
  std::string line;
  std::istringstream stream;

  std::string token;
  size_t symbol_id;
  std::vector<size_t> rule;

  while (!_input.eof()) {
    std::getline(_input, line, '\n');
    stream.str(std::move(line));

    if ((stream >> std::ws).eof()) continue;

    if (stream.get() != '<' || std::getline(stream, token, '>').eof())
      throw std::invalid_argument("BNF syntax error");
    symbol_id = _grammar->addSymbol(token, true);
    stream >> token;
    if (token != "::=") throw std::invalid_argument("BNF syntax error");

    while (!(stream >> std::ws).eof()) {
      switch (stream.get()) {
        case '<':
          if (std::getline(stream, token, '>').eof())
            throw std::invalid_argument("BNF syntax error");
          rule.push_back(_grammar->addSymbol(token, true));
          break;

        case '"':
          if (std::getline(stream, token, '"').eof())
            throw std::invalid_argument("BNF syntax error");
          rule.push_back(_grammar->addSymbol(token, false));
          break;

        case '|':
          _grammar->addProduction(symbol_id, std::move(rule));
          break;

        default:
          throw std::invalid_argument("BNF syntax error");
      }
    }
    _grammar->addProduction(symbol_id, std::move(rule));
  }
}

std::shared_ptr<Grammar> BuilderFromBNF::getResult() {
  _input.rdbuf(nullptr);
  std::shared_ptr<Grammar> result(std::move(_grammar));
  return result;
}
