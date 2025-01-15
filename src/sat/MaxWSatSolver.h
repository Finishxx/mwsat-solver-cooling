#ifndef MAXWSATSOLVER_H
#define MAXWSATSOLVER_H
#include <optional>
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
    bool operator<(const LiveTerm& other) const;
    LiveTerm(Term original, bool isSet);
    LiveTerm(const LiveTerm& term) = default;
    LiveTerm& operator=(const LiveTerm& other) = default;
    [[nodiscard]] const Term& originalTerm() const;
    [[nodiscard]] uint32_t id() const;
    [[nodiscard]] bool isSatisfied() const;
    [[nodiscard]] bool isSet() const;
    [[nodiscard]] bool isUnset() const;
    void set();
    void unset();
    void flip();
  };

  class LiveClause {
   private:
    uint32_t satisfiedCount;
    /** Sorted terms by id representing the Clause */
    std::vector<LiveTerm> terms_;
    Clause* original;

    LiveTerm* findTerm(uint32_t);

   public:
    LiveClause(Clause* clause, SatConfig& config);

    [[nodiscard]] const Clause& originalClause() const;
    [[nodiscard]] const std::vector<LiveTerm>& terms() const;
    /** Returns null if Term with given id is not present */
    [[nodiscard]] const LiveTerm* getTerm(uint32_t id) const;

    /** Does nothing if given variable id, which is not in the clause */
    void setVariable(uint32_t variableId);
    /** Does nothing if given variable id, which is not in the clause */
    void unsetVariable(uint32_t variableId);
    /** Does nothing if given variable id, which is not in the clause */
    void flipVariable(uint32_t variableId);
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
