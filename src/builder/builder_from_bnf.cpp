#include "builder_from_bnf.hpp"

BuilderFromBNF::BuilderFromBNF() : _input(nullptr), _grammar(nullptr) {}

BuilderFromBNF::BuilderFromBNF(std::istream& stream)
    : _input(stream.rdbuf()), _grammar(std::make_shared<Grammar>()) {}

BuilderFromBNF::BuilderFromBNF(const std::string& string)
    : _buf(string), _input(&_buf), _grammar(std::make_shared<Grammar>()) {}

std::shared_ptr<Grammar> BuilderFromBNF::build(std::istream& stream) {
  return reset(stream).readRules().getResult();
}
std::shared_ptr<Grammar> BuilderFromBNF::build(const std::string& string) {
  return reset(string).readRules().getResult();
}

BuilderFromBNF& BuilderFromBNF::reset(std::istream& stream) {
  _input.rdbuf(stream.rdbuf());
  _grammar = std::make_shared<Grammar>();
  return *this;
}

BuilderFromBNF& BuilderFromBNF::reset(const std::string& string) {
  _buf.str(string);
  _input.rdbuf(&_buf);
  _grammar = std::make_shared<Grammar>();
  return *this;
}

BuilderFromBNF& BuilderFromBNF::readRules() {
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
          for (char character : token) {
            rule.push_back(
                _grammar->addSymbol(std::string(1, character), false));
          }
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
  return *this;
}

std::shared_ptr<Grammar> BuilderFromBNF::getResult() {
  _input.rdbuf(nullptr);
  std::shared_ptr<Grammar> result(std::move(_grammar));
  return result;
}
