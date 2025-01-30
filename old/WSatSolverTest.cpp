#include <gtest/gtest.h>

#include <random>

#include "WSatSolver.h"
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
  WSatInstance instance = WSatInstance(res.clauses, res.weights);
  std::vector<bool> init;
  init.resize(instance.variables().size(), false);
  SatConfig config = SatConfig(std::move(init));
  WSatSolver solver = WSatSolver(instance, config);

  // Config is copied and instance is the same
  ASSERT_EQ(&solver.instance(), &instance);
  ASSERT_EQ(solver.currentConfiguration().instance, &instance);
  ASSERT_NE(&solver.currentConfiguration().configuration, &config);
  EvaluatedWSatConfig exported = solver.exportConfiguration();
  ASSERT_NE(&exported, &solver.currentConfiguration());
  ASSERT_NE(
      &exported.configuration, &solver.currentConfiguration().configuration
  );

  // solver.printOut();
  ASSERT_EQ(solver.currentConfiguration().satisfiedCount, 5);
  ASSERT_EQ(solver.currentConfiguration().weight, 0);
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
  WSatInstance instance = WSatInstance(res.clauses, res.weights);
  std::vector<bool> init;
  init.resize(instance.variables().size(), false);
  SatConfig config = SatConfig(std::move(init));
  WSatSolver solver = WSatSolver(instance, config);

  std::vector<bool> newInit;
  newInit.resize(instance.variables().size(), true);
  SatConfig newConfig = SatConfig(std::move(newInit));
  solver.setConfig(newConfig);

  // Config is copied and instance is the same
  ASSERT_EQ(&solver.instance(), &instance);
  ASSERT_EQ(solver.currentConfiguration().instance, &instance);
  ASSERT_NE(&solver.currentConfiguration().configuration, &newConfig);
  EvaluatedWSatConfig exported = solver.exportConfiguration();
  ASSERT_NE(&exported, &solver.currentConfiguration());
  ASSERT_NE(
      &exported.configuration, &solver.currentConfiguration().configuration
  );

  solver.printOut();
  ASSERT_EQ(solver.currentConfiguration().satisfiedCount, 5);
  ASSERT_EQ(solver.currentConfiguration().weight, 13);
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
  WSatInstance instance = WSatInstance(res.clauses, res.weights);
  std::vector<bool> init;
  init.resize(instance.variables().size(), false);
  SatConfig config = SatConfig(std::move(init));
  WSatSolver solver = WSatSolver(instance, config);

  solver.flipVariable(4);

  // Config is copied and instance is the same
  ASSERT_EQ(&solver.instance(), &instance);
  ASSERT_EQ(solver.currentConfiguration().instance, &instance);
  ASSERT_NE(&solver.currentConfiguration().configuration, &config);
  EvaluatedWSatConfig exported = solver.exportConfiguration();
  ASSERT_NE(&exported, &solver.currentConfiguration());
  ASSERT_NE(
      &exported.configuration, &solver.currentConfiguration().configuration
  );

  // solver.printOut();
  // 0 0 0 1
  ASSERT_EQ(solver.currentConfiguration().satisfiedCount, 6);
  ASSERT_EQ(solver.currentConfiguration().weight, 6);

  solver.flipVariable(4);
  // 0 0 0 0
  ASSERT_EQ(solver.currentConfiguration().satisfiedCount, 5);
  ASSERT_EQ(solver.currentConfiguration().weight, 0);

  // 1 0 0 1
  solver.flipVariable(4);
  solver.flipVariable(1);
  ASSERT_EQ(solver.currentConfiguration().satisfiedCount, 6);
  ASSERT_EQ(solver.currentConfiguration().weight, 8);
  ASSERT_EQ(solver.currentConfiguration().configuration.underlying[0], true);
  ASSERT_EQ(solver.currentConfiguration().configuration.underlying[1], false);
  ASSERT_EQ(solver.currentConfiguration().configuration.underlying[2], false);
  ASSERT_EQ(solver.currentConfiguration().configuration.underlying[3], true);

  // 1 0 1 1
  solver.flipVariable(3);
  ASSERT_EQ(solver.currentConfiguration().satisfiedCount, 4);
  ASSERT_EQ(solver.currentConfiguration().weight, 9);

  // 1 1 1 1
  solver.flipVariable(2);
  ASSERT_EQ(solver.currentConfiguration().satisfiedCount, 5);
  ASSERT_EQ(solver.currentConfiguration().weight, 13);
}