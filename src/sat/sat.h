#pragma once

/**
 * An abstraction over underlying data representation which presents the data
 * in human understandable format. In essence just knows where in the input the
 * clause starts and how long it is.
 */
struct SatClause {
  size_t id;  // index of clause
  size_t startIndex;  // start index inside underlying
  size_t size;  // size of clause
  std::vector<int32_t>* underlying;

  SatClause(size_t start, size_t size, const std::vector<int32_t>& underlying);

  // Maybe useless class after all? TODO: Try to add some helpers at least
  friend std::ostream& operator<<(std::ostream& os, const SatClause& dt);
  bool isSatisfied() const;
  bool isNotSatisfied() const;
};

/**
 * Holds where everywhere is the variable used
 */
struct VariableInfo {
  std::vector<size_t> ids;  // sorted indices of variable occurence
  std::vector<bool> used;  // bitset of variable occurence

  // sorted indices of variable occurence in (non)satisfied clauses
  std::vector<size_t> satisfied;
  std::vector<size_t> notSatisfied;

  // Clauses in where the variable is inside of
  std::vector<SatClause*> clauses;

  // Maybe add underlying to constructor too
  VariableInfo(const std::vector<SatClause>& clauses);
  friend std::ostream& operator<<(std::ostream& os, const VariableInfo& dt);
};

/**
 * Input of SAT problem
 *
 * Each variable has a positive integral id
 *
 * Underlying data representation is a vector of integral numbers.
 * If a number is positive, then value of variable with id of that number is
 * as-is, if it is negative, then that value is negated.
 *
 * On top of that there is variable infos, which remember where given variable
 * is used.
 */
class SatInput {
 public:
  std::vector<SatClause> clauses;
  std::vector<VariableInfo> variables;
  std::vector<int32_t> underlying;
  SatInput(
      std::vector<SatClause>&& clauses, std::vector<VariableInfo>&& variables
  );
};

/**
 * Configuration represented as bitset
 */
struct SatConfiguration {
  SatInput& input;
  std::vector<bool> bitset;

  SatConfiguration(std::vector<bool>&& bitset, SatInput& input);
  friend std::ostream& operator<<(std::ostream& os, const VariableInfo& dt);
  // TODO: Useless class for printing or are there any helpers usabel by result?
};

/**
 * Combines configuration and input to produce output
 */
struct SatResult {
  SatConfiguration* configuration;  // can change if mutated
  SatInput* input;

  SatResult(SatConfiguration* configuration, SatInput* input);

  // Mutating the results
  void flipVariable(size_t variableId);
  void flipVariable(const VariableInfo& info);
  // Creating new
  SatResult tryFlipVariable(size_t variableId);
  SatResult tryFlipVariable(const VariableInfo& info);

  size_t satisfiedClauseCount() const;
  size_t nonSatisfiedClauseCount() const;
  // getting: satisfiedClauses, unsatisfiedClauses

  bool isSolved() const;
  bool isNotSolved() const;
};

/**
 * SAT solver
 * - represents SAT input, output, configuration
 * - allows changing state with `flipVariable` function
 * - allows for querying state
 */
struct Sat {
  Sat(SatInput& is);
  SatResult tryConfiguration(SatConfiguration& configuration);
};