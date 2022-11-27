#include <gtest/gtest.h>

#include <sstream>

#include "builder_from_bnf.hpp"
#include "earley_parser.hpp"

void TestSeries(EarleyParser& parser, bool result,
                const std::vector<std::string>& tests) {
  for (const auto& test : tests) {
    EXPECT_EQ(parser.recognize(test), result);
  }
}

TEST(EarleyTests, SimpleTest) {
  BuilderFromBNF builder;
  auto grammar1 = builder.build("<S> ::= \"a\"<S>\"b\"<S>|");
  EarleyParser parser1(grammar1);

  std::vector<std::string> correct{"ab",     "aabb",   "aabbab",
                                   "abaabb", "ababab", "aaabbbabaabb"};
  std::vector<std::string> not_correct{"ba", "aba", "abba", "ababa",
                                       "aaaaaabbbbb"};
  TestSeries(parser1, true, correct);
  TestSeries(parser1, false, not_correct);

  auto grammar2 = builder.build("<S> ::= \"a\"<S>\"b\"|<S><S>|");
  EarleyParser parser2(grammar2);
  TestSeries(parser2, true, correct);
  TestSeries(parser2, false, not_correct);
}

TEST(EarleyTests, LoggingTest) {
  BuilderFromBNF builder;
  auto grammar = builder.build("<S> ::= \"a\"");
  auto earley = std::make_shared<EarleyParser>(grammar);
  std::shared_ptr<Parser> parser(earley);
  std::ostringstream out("");
  earley->enable_logging(out);
  EXPECT_EQ(parser->recognize("a"), true);
  EXPECT_EQ(out.str(),
            "[0] (0, 0, 0)\n"
            "[1] (0, 1, 0)\n");
}
