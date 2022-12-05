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

  std::vector<std::string> correct{"",       "ab",     "aabb",        "aabbab",
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

TEST(EarleyTests, TestedManually1) {
  EarleyParser parser(BuilderFromBNF("<S>::=<S><S>|\"a\"<S>\"b\"|").build());
  std::ostringstream out("");
  parser.enable_logging(out);
  EXPECT_EQ(parser.recognize("ababab"), true);
  EXPECT_EQ(out.str(),
            "[0] (3, 0, 0)\n"
            "[0] (0, 0, 0)\n"
            "[0] (1, 0, 0)\n"
            "[0] (2, 0, 0)\n"
            "[0] (3, 1, 0)\n"
            "[0] (0, 1, 0)\n"
            "[0] (0, 2, 0)\n"
            "[1] (1, 1, 0)\n"
            "[1] (0, 0, 1)\n"
            "[1] (1, 0, 1)\n"
            "[1] (2, 0, 1)\n"
            "[1] (1, 2, 0)\n"
            "[1] (0, 1, 1)\n"
            "[1] (0, 2, 1)\n"
            "[2] (1, 3, 0)\n"
            "[2] (3, 1, 0)\n"
            "[2] (0, 1, 0)\n"
            "[2] (0, 2, 0)\n"
            "[2] (0, 0, 2)\n"
            "[2] (1, 0, 2)\n"
            "[2] (2, 0, 2)\n"
            "[2] (0, 1, 2)\n"
            "[2] (0, 2, 2)\n"
            "[3] (1, 1, 2)\n"
            "[3] (0, 0, 3)\n"
            "[3] (1, 0, 3)\n"
            "[3] (2, 0, 3)\n"
            "[3] (1, 2, 2)\n"
            "[3] (0, 1, 3)\n"
            "[3] (0, 2, 3)\n"
            "[4] (1, 3, 2)\n"
            "[4] (0, 2, 0)\n"
            "[4] (0, 1, 2)\n"
            "[4] (0, 2, 2)\n"
            "[4] (3, 1, 0)\n"
            "[4] (0, 1, 0)\n"
            "[4] (0, 0, 4)\n"
            "[4] (1, 0, 4)\n"
            "[4] (2, 0, 4)\n"
            "[4] (0, 1, 4)\n"
            "[4] (0, 2, 4)\n"
            "[5] (1, 1, 4)\n"
            "[5] (0, 0, 5)\n"
            "[5] (1, 0, 5)\n"
            "[5] (2, 0, 5)\n"
            "[5] (1, 2, 4)\n"
            "[5] (0, 1, 5)\n"
            "[5] (0, 2, 5)\n"
            "[6] (1, 3, 4)\n"
            "[6] (0, 2, 2)\n"
            "[6] (0, 2, 0)\n"
            "[6] (0, 1, 4)\n"
            "[6] (0, 2, 4)\n"
            "[6] (0, 1, 2)\n"
            "[6] (3, 1, 0)\n"
            "[6] (0, 1, 0)\n"
            "[6] (0, 0, 6)\n"
            "[6] (1, 0, 6)\n"
            "[6] (2, 0, 6)\n"
            "[6] (0, 1, 6)\n"
            "[6] (0, 2, 6)\n");
}

TEST(EarleyTests, TestedManually2) {
  EarleyParser parser(BuilderFromBNF("<S>::=\"a\"<S>\"b\"<S>|").build());
  std::ostringstream out("");
  parser.enable_logging(out);
  EXPECT_EQ(parser.recognize("ababba"), false);
  EXPECT_EQ(out.str(),
            "[0] (2, 0, 0)\n"
            "[0] (0, 0, 0)\n"
            "[0] (1, 0, 0)\n"
            "[0] (2, 1, 0)\n"
            "[1] (0, 1, 0)\n"
            "[1] (0, 0, 1)\n"
            "[1] (1, 0, 1)\n"
            "[1] (0, 2, 0)\n"
            "[2] (0, 3, 0)\n"
            "[2] (0, 0, 2)\n"
            "[2] (1, 0, 2)\n"
            "[2] (0, 4, 0)\n"
            "[2] (2, 1, 0)\n"
            "[3] (0, 1, 2)\n"
            "[3] (0, 0, 3)\n"
            "[3] (1, 0, 3)\n"
            "[3] (0, 2, 2)\n"
            "[4] (0, 3, 2)\n"
            "[4] (0, 0, 4)\n"
            "[4] (1, 0, 4)\n"
            "[4] (0, 4, 2)\n"
            "[4] (0, 4, 0)\n"
            "[4] (2, 1, 0)\n");
}

TEST(EarleyTests, TestedManually3) {
  EarleyParser parser(
      BuilderFromBNF("<S>::=\"a\"<S>\"b\"<S>|\"b\"<S>\"a\"<S>|").build());
  std::ostringstream out("");
  parser.enable_logging(out);
  EXPECT_EQ(parser.recognize("ababba"), true);
  EXPECT_EQ(out.str(),
            "[0] (3, 0, 0)\n"
            "[0] (0, 0, 0)\n"
            "[0] (1, 0, 0)\n"
            "[0] (2, 0, 0)\n"
            "[0] (3, 1, 0)\n"
            "[1] (0, 1, 0)\n"
            "[1] (0, 0, 1)\n"
            "[1] (1, 0, 1)\n"
            "[1] (2, 0, 1)\n"
            "[1] (0, 2, 0)\n"
            "[2] (1, 1, 1)\n"
            "[2] (0, 3, 0)\n"
            "[2] (0, 0, 2)\n"
            "[2] (1, 0, 2)\n"
            "[2] (2, 0, 2)\n"
            "[2] (1, 2, 1)\n"
            "[2] (0, 4, 0)\n"
            "[2] (3, 1, 0)\n"
            "[3] (0, 1, 2)\n"
            "[3] (1, 3, 1)\n"
            "[3] (0, 0, 3)\n"
            "[3] (1, 0, 3)\n"
            "[3] (2, 0, 3)\n"
            "[3] (0, 2, 2)\n"
            "[3] (1, 4, 1)\n"
            "[3] (0, 2, 0)\n"
            "[4] (1, 1, 3)\n"
            "[4] (0, 3, 2)\n"
            "[4] (0, 3, 0)\n"
            "[4] (0, 0, 4)\n"
            "[4] (1, 0, 4)\n"
            "[4] (2, 0, 4)\n"
            "[4] (1, 2, 3)\n"
            "[4] (0, 4, 2)\n"
            "[4] (0, 4, 0)\n"
            "[4] (1, 2, 1)\n"
            "[4] (3, 1, 0)\n"
            "[5] (1, 1, 4)\n"
            "[5] (0, 0, 5)\n"
            "[5] (1, 0, 5)\n"
            "[5] (2, 0, 5)\n"
            "[5] (1, 2, 4)\n"
            "[6] (0, 1, 5)\n"
            "[6] (1, 3, 4)\n"
            "[6] (0, 0, 6)\n"
            "[6] (1, 0, 6)\n"
            "[6] (2, 0, 6)\n"
            "[6] (0, 2, 5)\n"
            "[6] (1, 4, 4)\n"
            "[6] (1, 2, 3)\n"
            "[6] (0, 4, 2)\n"
            "[6] (0, 4, 0)\n"
            "[6] (1, 2, 1)\n"
            "[6] (3, 1, 0)\n");
}
