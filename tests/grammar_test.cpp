#include "grammar.hpp"

#include <gtest/gtest.h>

TEST(GrammarTests, SimpleTests) {
  Grammar grammar(std::make_shared<SimpleAlphabet>());
  EXPECT_THROW(grammar.getStartingNonterminal(), std::out_of_range);
  EXPECT_THROW(grammar.getAlphabet().getId("S"), std::out_of_range);
  EXPECT_THROW(grammar.getAlphabet().getSymbol(0), std::out_of_range);
  EXPECT_THROW(grammar.getAlphabet().isNonterminal(0), std::out_of_range);
  EXPECT_THROW(grammar.addSymbol("ab", false), std::invalid_argument);
  grammar.addSymbol("a", false);
  EXPECT_EQ(grammar.getAlphabet().size(), 1);
  EXPECT_THROW(grammar.getStartingNonterminal(), std::invalid_argument);
  grammar.addSymbol("S", true);
  EXPECT_EQ(grammar.getAlphabet().size(), 2);
  EXPECT_THROW(grammar.setStartingNonterminal(0), std::invalid_argument);
  EXPECT_NO_THROW(grammar.setStartingNonterminal(1));
  EXPECT_THROW(grammar.makeSingleStartRule("S"), std::invalid_argument);
  std::vector<size_t> rule{0, 1, 2, 3};
  EXPECT_THROW(grammar.addProduction(1, rule), std::invalid_argument);
  rule = {0, 1};
  EXPECT_NO_THROW(grammar.addProduction(1, rule));
  EXPECT_NO_THROW(grammar.makeSingleStartRule("S'"));
}
