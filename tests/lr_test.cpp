#include <gtest/gtest.h>

#include "builder_from_bnf.hpp"
#include "lr_parser.hpp"

void TestSeries(LRParser& parser, bool result,
                const std::vector<std::string>& tests) {
  for (const auto& test : tests) {
    EXPECT_EQ(parser.recognize(test), result);
  }
}

TEST(LRTests, BuildLRTest) {
  BuilderFromBNF builder;
  LRParser parser1(builder.build("<S>::=\"a\"<S>\"b\"<S>|"));
  LRParser parser2(builder.build("<S>::=<S>\"a\"<S>\"b\"|"));

  std::vector<std::string> correct{"",       "ab",     "aabb",        "aabbab",
                                   "abaabb", "ababab", "aaabbbabaabb"};
  std::vector<std::string> not_correct{"ba", "aba", "abba", "ababa",
                                       "aaaaaabbbbb"};
  TestSeries(parser1, true, correct);
  TestSeries(parser2, true, correct);
  TestSeries(parser1, false, not_correct);
  TestSeries(parser2, false, not_correct);
}

TEST(LRTests, CtorsTest) {
  BuilderFromBNF builder("<S>::=\"a\"<S>\"b\"<S>|");
  auto grammar = builder.build();
  LRAnalyzer analyzer(grammar);
  LRParser parser1(analyzer);
  LRParser parser2(std::move(analyzer));
}

TEST(LRTests, NotLRGrammarsTest) {
  BuilderFromBNF builder;
  EXPECT_THROW(LRParser parser1(builder.build("<S>::=\"a\"<S>\"b\"|<S><S>|")),
               std::invalid_argument);
  EXPECT_THROW(LRParser parser2(builder.build("<S>::=<S>\"a\"|\"a\"|")),
               std::invalid_argument);
  EXPECT_THROW(LRParser parser3(builder.build("<S>::=<S><S><S>|<S><S>|")),
               std::invalid_argument);
  EXPECT_THROW(LRParser parser4(builder.build("<S>::=<S>\"a\"|\"a\"|")),
               std::invalid_argument);
}
