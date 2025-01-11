#ifndef MAXWSATSOLVER_H
#define MAXWSATSOLVER_H
#include <vector>

#include "EvaluatedWSatConfig.h"
#include "MaxWSatInstance.h"
#include "SatConfig.h"

class MaxWSatSolver {
 private:
  class LiveVariable {};
  class LiveClause {};

  std::vector<LiveVariable> variables;
  std::vector<LiveClause> clauses;

 public:
  /** Calculates new configuration by modifying previous configuration */
  void flipVariable(uint32_t variableId);

  MaxWSatSolver(MaxWSatInstance& instance, SatConfig& config);

  const EvaluatedWSatConfig& currentConfiguration() const;
  /** Creates copy of currentConfiguration */
  EvaluatedWSatConfig exportConfiguration() const;
  const MaxWSatInstance& instance() const;
};

#endif  // MAXWSATSOLVER_H
