#include <gtest/gtest.h>

#include <random>

#include "WSatInstance.h"
#include "dimacsParsing.h"

void testTerm(
    WSatInstance& instance,
    uint32_t clauseIndex,
    uint32_t termIndex,
    uint32_t id,
    bool isPlain
) {
  const Term& term = instance.clauses()[clauseIndex].disjuncts()[termIndex];
  EXPECT_EQ(term.id(), id);
  EXPECT_EQ(term.isPlain(), isPlain);
}

TEST(MaxWSatInstanceTest, testingInstance) {
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
  ParsedDimacsFile res = parseDimacsFile(ss);
  WSatInstance instance = WSatInstance(res.clauses, res.weights);

  // Test clauses
  EXPECT_EQ(instance.clauses().size(), 6);
  EXPECT_EQ(instance.clauses()[0].disjuncts().size(), 3);
  testTerm(instance, 0, 0, 1, true);
  testTerm(instance, 0, 1, 3, false);
  testTerm(instance, 0, 2, 4, true);
  EXPECT_EQ(instance.clauses()[1].disjuncts().size(), 3);
  testTerm(instance, 1, 0, 1, false);
  testTerm(instance, 1, 1, 2, true);
  testTerm(instance, 1, 2, 3, false);
  EXPECT_EQ(instance.clauses()[2].disjuncts().size(), 2);
  testTerm(instance, 2, 0, 3, true);
  testTerm(instance, 2, 1, 4, true);
  EXPECT_EQ(instance.clauses()[3].disjuncts().size(), 4);
  testTerm(instance, 3, 0, 1, true);
  testTerm(instance, 3, 1, 2, true);
  testTerm(instance, 3, 2, 3, false);
  testTerm(instance, 3, 3, 4, false);
  EXPECT_EQ(instance.clauses()[4].disjuncts().size(), 2);
  testTerm(instance, 4, 0, 2, false);
  testTerm(instance, 4, 1, 3, true);
  EXPECT_EQ(instance.clauses()[5].disjuncts().size(), 2);
  testTerm(instance, 5, 0, 3, false);
  testTerm(instance, 5, 1, 4, false);

  // Test variables
  EXPECT_EQ(instance.variables().size(), 4);
  EXPECT_EQ(instance.variables()[0].weight(), 2);
  EXPECT_EQ(instance.variables()[0].id(), 1);
  EXPECT_EQ(instance.variables()[1].weight(), 4);
  EXPECT_EQ(instance.variables()[1].id(), 2);
  EXPECT_EQ(instance.variables()[2].weight(), 1);
  EXPECT_EQ(instance.variables()[2].id(), 3);
  EXPECT_EQ(instance.variables()[3].weight(), 6);
  EXPECT_EQ(instance.variables()[3].id(), 4);

  EXPECT_EQ(instance.variables()[0].occurences().size(), 3);
  EXPECT_EQ(instance.variables()[1].occurences().size(), 3);
  EXPECT_EQ(instance.variables()[2].occurences().size(), 6);
  EXPECT_EQ(instance.variables()[3].occurences().size(), 4);

  EXPECT_EQ(instance.weightTotal(), 13);
}
