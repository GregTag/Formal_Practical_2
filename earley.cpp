#include <fstream>
#include <iostream>

#include "builder_from_bnf.hpp"
#include "earley_parser.hpp"

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <path/to/grammar_in_bnf>\n";
    return 1;
  }
  std::ifstream file(argv[1], std::ios::in);
  if (!file.is_open()) {
    std::cerr << "Error: No such file '" << argv[1] << "'\n";
    return 1;
  }

  BuilderFromBNF builder(file);
  EarleyParser parser(builder.build());

  std::string str;
  parser.enable_logging(std::cout);
  while (!std::cin.eof()) {
    std::getline(std::cin, str, '\n');
    std::cout << (parser.recognize(str) ? "Yes\n" : "No\n");
  }
}
