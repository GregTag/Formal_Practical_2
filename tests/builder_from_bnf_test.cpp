#include "builder_from_bnf.hpp"

#include <gtest/gtest.h>

TEST(BuilderTests, SimpleTest) {
  std::istringstream streams[] = {
      std::istringstream("<S> ::= <S> \"a\" <S> \"b\" |"),
      std::istringstream("<S> ::= <S> \"a\" <S> \"b\" | "),
      std::istringstream("<S> ::= <S> \"a\" <S> \"b\" | \n")};
  BuilderFromBNF builder;

  for (auto& stream : streams) {
    builder.reset(stream);
    builder.readRules();
    auto grammar = builder.getResult();

    EXPECT_EQ(grammar->getAlphabet().getSymbol(0), "S");
    EXPECT_EQ(grammar->getAlphabet().getSymbol(1), "a");
    EXPECT_EQ(grammar->getAlphabet().getSymbol(2), "b");
    EXPECT_THROW(grammar->getAlphabet().getSymbol(3), std::out_of_range);

    EXPECT_EQ(grammar->getAlphabet().isNonterminal(0), true);
    EXPECT_EQ(grammar->getAlphabet().isNonterminal(1), false);
    EXPECT_EQ(grammar->getAlphabet().isNonterminal(2), false);
    EXPECT_THROW(grammar->getAlphabet().isNonterminal(3), std::out_of_range);

    EXPECT_THROW(grammar->setStartingNonterminal(1), std::invalid_argument);
    EXPECT_EQ(grammar->getStartingNonterminal(), 0);

    EXPECT_EQ(grammar->getProductions().size(), 2);
    const auto& rule1 = grammar->getProductions()[0];
    const auto& rule2 = grammar->getProductions()[1];

    EXPECT_EQ(rule1.symbol, 0);
    EXPECT_EQ(rule2.symbol, 0);
    EXPECT_EQ(rule1.rule.size(), 4);
    EXPECT_EQ(rule1.rule[0], 0);
    EXPECT_EQ(rule1.rule[1], 1);
    EXPECT_EQ(rule1.rule[2], 0);
    EXPECT_EQ(rule1.rule[3], 2);
    EXPECT_EQ(rule2.rule.size(), 0);
  }
}

void run_builder(const std::string& str) {
  std::istringstream stream(str);
  BuilderFromBNF builder(stream);
  builder.readRules();
  builder.getResult();
}

TEST(BuilderTests, TestSyntaxCheck) {
  EXPECT_THROW(run_builder("<S>"), std::invalid_argument);
  EXPECT_THROW(run_builder("\"S\" ::= <S> \"b\" |"), std::invalid_argument);
  EXPECT_THROW(run_builder("<S> <S> \"b\" |"), std::invalid_argument);
  EXPECT_THROW(run_builder("<S> =:: <S> \"b\" |"), std::invalid_argument);
  EXPECT_THROW(run_builder("<S> ::= <S> \"a\" SSS |"), std::invalid_argument);
  EXPECT_THROW(run_builder("<S> ::= <S> \"b |"), std::invalid_argument);
  EXPECT_THROW(run_builder("<S> ::= <S \"b\" |"), std::invalid_argument);
}
