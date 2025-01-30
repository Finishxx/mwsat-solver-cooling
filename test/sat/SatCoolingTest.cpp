#include <gtest/gtest.h>

#include <random>

#include "SatConfig.h"
#include "SatCooling.h"
#include "WSatInstance.h"
#include "dimacsParsing.h"

TEST(WSatSolverTest, initialization) {
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
  std::vector<bool> init;
  init.resize(res.varCount, false);
  SatConfig config = SatConfig(std::move(init));
  SatCooling cooling(res.clauses, res.weights);
  SatCriteria criteria = cooling.evaluateConfiguration(config);

  ASSERT_EQ(criteria.satisfied(), 5);
  ASSERT_EQ(criteria.weight(), 0);
}

TEST(WSatSolverTest, setConfig) {
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
  std::vector<bool> init;
  init.resize(res.varCount, true);
  SatConfig config = SatConfig(std::move(init));
  SatCooling cooling(res.clauses, res.weights);
  SatCriteria criteria = cooling.evaluateConfiguration(config);

  ASSERT_EQ(criteria.satisfied(), 5);
  ASSERT_EQ(criteria.weight(), 13);
}

TEST(WSatSolverTest, flipVariable) {
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
  std::vector<bool> init;
  init.resize(res.varCount, false);
  SatConfig config = SatConfig(std::move(init));
  SatCooling cooling(res.clauses, res.weights);

  config.underlying[3] = true;
  SatCriteria criteria = cooling.evaluateConfiguration(config);
  // 0 0 0 1
  ASSERT_EQ(criteria.satisfied(), 6);
  ASSERT_EQ(criteria.weight(), 6);

  // 1 0 0 1
  config.underlying[0] = true;
  criteria = cooling.evaluateConfiguration(config);
  ASSERT_EQ(criteria.satisfied(), 6);
  ASSERT_EQ(criteria.weight(), 8);

  // 1 0 1 1
  config.underlying[2] = true;
  criteria = cooling.evaluateConfiguration(config);

  ASSERT_EQ(criteria.satisfied(), 4);
  ASSERT_EQ(criteria.weight(), 9);

  // 1 1 1 1
  config.underlying[1] = true;
  criteria = cooling.evaluateConfiguration(config);
  ASSERT_EQ(criteria.satisfied(), 5);
  ASSERT_EQ(criteria.weight(), 13);
}