#ifndef MAXWSATSOLVER_H
#define MAXWSATSOLVER_H
#include <optional>
#include <vector>

#include "EvaluatedWSatConfig.h"
#include "SatConfig.h"
#include "WSatInstance.h"

/**
 * Max Weighted Sat Solver with fast `flipVariable()` method achieved by
 * storing additional information between the calls. The trade-off is that
 * `setConfig()` (also used in constructor) is slower, because it has to store
 * context needed for faster `flipVariable()`.
 *
 * Expects the given instance to be satisfiable and to not contain duplicates
 * inside clauses e.g. (1 2 3 1).
 */
class WSatSolver {
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
    const Clause* original;

    LiveTerm* findTerm(uint32_t);

   public:
    LiveClause(const Clause* clause, SatConfig& config);

    [[nodiscard]] const Clause& originalClause() const;
    [[nodiscard]] const std::vector<LiveTerm>& terms() const;
    /** Returns null if Term with given id is not present */
    [[nodiscard]] const LiveTerm* getTerm(uint32_t id) const;
    [[nodiscard]] bool isSatisfied() const;

    /** Does nothing if given variable id, which is not in the clause */
    void setVariable(uint32_t variableId);
    /** Does nothing if given variable id, which is not in the clause */
    void unsetVariable(uint32_t variableId);
    /** Does nothing if given variable id, which is not in the clause */
    void flipVariable(uint32_t variableId);
  };

  /** Initialized by the Solver */
  class LiveVariable {
   private:
    bool isSet_;

   public:
    const Variable* original;
    LiveVariable(const Variable* variable, bool isSet);
    /** Should not be public, but oh well */
    std::vector<LiveClause*> occurrences;
    [[nodiscard]] int32_t weight() const;
    [[nodiscard]] uint32_t id() const;
    [[nodiscard]] bool isSet() const;
    /** @return change in satisfied clauses */
    int32_t flip();
  };

  /** Indexed by variable's id, but 0th element is dummy */
  std::vector<LiveVariable> variables;
  std::vector<LiveClause> clauses;

  WSatInstance* instance_;
  EvaluatedWSatConfig config_;

  [[nodiscard]] const LiveVariable& variableById(uint32_t variableId) const;
  LiveVariable& variableById(uint32_t variableId);
  /** For iteration, because does not include the dummy 0th element */
  std::ranges::subrange<std::vector<LiveVariable>::iterator> legalVariables();

 public:
  /** Calculates new configuration by modifying previous configuration */
  void flipVariable(uint32_t variableId);
  /** Calculates new configuration given specific SatConfig. Can be slow. */
  void setConfig(SatConfig& config);

  WSatSolver(WSatInstance& instance, SatConfig& initialConfig);

  [[nodiscard]] const EvaluatedWSatConfig& currentConfiguration() const;
  /** Creates copy of currentConfiguration */
  EvaluatedWSatConfig exportConfiguration() const;
  const WSatInstance& instance() const;
};

#endif  // MAXWSATSOLVER_H
