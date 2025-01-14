#ifndef MAXWSATSOLVER_H
#define MAXWSATSOLVER_H
#include <vector>

#include "EvaluatedWSatConfig.h"
#include "MaxWSatInstance.h"
#include "SatConfig.h"

/**
 * Max Weighted Sat Solver with fast `flipVariable()` method achieved by
 * storing additional information between the calls. The trade-off is that
 * `setConfig()` (also used in constructor) is slower, because it has to store
 * context needed for faster `flipVariable()`.
 *
 * Expects the given instance to be satisfiable and to not contain duplicates
 * inside clauses e.g. (1 2 3 1).
 */
class MaxWSatSolver {
 private:
  class LiveTerm {
   private:
    bool isSet_;
    bool isSatisfied_;
    Term original;

   public:
    LiveTerm(Term original, bool isSet);
    const Term& originalTerm() const;
    uint32_t id() const;
    bool isSatisfied() const;
    bool isSet() const;
    bool isUnset() const;
    void set();
    void unset();
    void flip();
  };

  /** Keeps track of  */
  class LiveClause {
   private:
    uint32_t satisfiedTerms;
    /** Sorted terms by id, the Term  */
    std::vector<Term> terms;
    Clause* original;

   public:
    const Clause& originalClause() const;
    LiveClause(Clause* clause, SatConfig& config);
  };

  class LiveVariable {
   public:
    Variable* variable;
    std::vector<LiveClause*> clauses;
  };

  /** Indexed by variable id */
  std::vector<LiveVariable> variables;
  std::vector<LiveClause> clauses;

  MaxWSatInstance* instance_;
  EvaluatedWSatConfig config_;

 public:
  /** Calculates new configuration by modifying previous configuration */
  void flipVariable(uint32_t variableId);
  /** Calculates new configuration given specific SatConfig. Can be slow. */
  void setConfig(SatConfig& config);

  MaxWSatSolver(MaxWSatInstance& instance, SatConfig& initialConfig);

  const EvaluatedWSatConfig& currentConfiguration() const;
  /** Creates copy of currentConfiguration */
  EvaluatedWSatConfig exportConfiguration() const;
  const MaxWSatInstance& instance() const;
};

#endif  // MAXWSATSOLVER_H
