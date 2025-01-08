#include <gtest/gtest.h>

#include <random>

#include "dimacsParsing.h"

TEST(DimacsParserTest, EmptyFile) { std::string example = ""; }
TEST(PageAllocDeathTest, NoWeights) {
  std::string example = R"(c MWCNF Example
c 4 variables, 6 clauses
c each clause is terminated by '0' (not by the end of line)
p mwcnf 4 6
c zero-terminated as the clauses
1 -3 4 0
-1 2 -3 0
3 4 0
1 2 -3 -4 0
-2 3 0
-3 -4 0)";
  std::stringstream ss(example);
  EXPECT_THROW({ parseDimacsFile(ss); }, std::invalid_argument);
}

TEST(DimacsParserTest, NoPLine) {
  std::string example = R"(c MWCNF Example
c 4 variables, 6 clauses
c each clause is terminated by '0' (not by the end of line)
c zero-terminated as the clauses
w 2 4 1 6 0
1 -3 4 0
-1 2 -3 0
3 4 0
1 2 -3 -4 0
-2 3 0
-3 -4 0)";

  std::stringstream ss(example);
  EXPECT_THROW({ parseDimacsFile(ss); }, std::invalid_argument);
}

TEST(DimacsParserTest, NoClauses) {
  std::string example = R"(c MWCNF Example
c 4 variables, 6 clauses
c each clause is terminated by '0' (not by the end of line)
p mwcnf 4 6
c zero-terminated as the clauses
w 2 4 1 6 0
)";

  std::stringstream ss(example);
  EXPECT_THROW({ parseDimacsFile(ss); }, std::invalid_argument);
}

TEST(DimacsParserTest, SmallestValid) {
  std::string example = R"(c MWCNF Example
c 4 variables, 6 clauses
c each clause is terminated by '0' (not by the end of line)
p mwcnf 1 1
c zero-terminated as the clauses
w 1 0
1 0)";

  std::stringstream ss(example);
  const auto res = parseDimacsFile(ss);
  EXPECT_EQ(res.clauses.size(), 1);
  EXPECT_EQ(res.clauses[0].size(), 1);
  EXPECT_EQ(res.weights.size(), 1);
  EXPECT_EQ(res.weights[0], 1);
  EXPECT_EQ(res.varCount, 1);
}

TEST(DimacsParserTest, Valid) {
  std::string example = R"(c MWCNF Example
c 4 variables, 6 clauses
c each clause is terminated by '0' (not by the end of line)
p mwcnf 4 6
c zero-terminated as the clauses
w 2 4 1 6 0
1 -3 4 0
-1 2 -3 0
3 4 0
1 2 -3 -4 0
-2 3 0
-3 -4 0)";

  std::stringstream ss(example);
  const auto res = parseDimacsFile(ss);
  EXPECT_EQ(res.clauses.size(), 6);
  EXPECT_EQ(res.clauses[0].size(), 3);
  auto firstClause = std::vector<int32_t>{1, -3, 4};
  EXPECT_EQ(res.clauses[0], firstClause);
  EXPECT_EQ(res.weights.size(), 4);
  auto weights = std::vector<int32_t>{2, 4, 1, 6};
  EXPECT_EQ(res.weights, weights);
  EXPECT_EQ(res.varCount, 4);
}