#include "MaxWSatSolver.h"

// ===================== LiveTerm =====================
MaxWSatSolver::LiveTerm::LiveTerm(Term original, bool isSet)
    : isSet_(isSet), original(original) {
  id_ = original.id();
  if (isSet && original.isPlain || !isSet && original.isNegated()) {
    isSatisfied_ = true;
  } else {
    isSet_ = false;
  }
}
const Term& MaxWSatSolver::LiveTerm::originalTerm() const { return original; }
uint32_t MaxWSatSolver::LiveTerm::id() const { return id_; }
bool MaxWSatSolver::LiveTerm::isSatisfied() const { return isSet_; }
bool MaxWSatSolver::LiveTerm::isSet() const { return isSet_; }
bool MaxWSatSolver::LiveTerm::isUnset() const { return !isSet_; }
void MaxWSatSolver::LiveTerm::set() {
  if (isSet_) return;
  isSet_ = true;
  isSatisfied_ = !isSatisfied_;
}
void MaxWSatSolver::LiveTerm::unset() {
  if (!isSet_) return;
  isSet_ = false;
  isSatisfied_ = !isSatisfied_;
}
void MaxWSatSolver::LiveTerm::flip() {
  isSet_ = !isSet_;
  isSatisfied_ = !isSatisfied_;
}
// ===================== EndLiveTerm =====================

void MaxWSatSolver::setConfig(SatConfig& config) {
  variables.clear();
  clauses.clear();

  config_ = EvaluatedWSatConfig(*instance_, SatConfig(config), 0, false, 0);
}

MaxWSatSolver::MaxWSatSolver(
    MaxWSatInstance& instance, SatConfig& initialConfig
) {
  instance_ = &instance;
  setConfig(initialConfig);
}

const EvaluatedWSatConfig& MaxWSatSolver::currentConfiguration() const {
  return config_;
}

EvaluatedWSatConfig MaxWSatSolver::exportConfiguration() const {
  return EvaluatedWSatConfig(config_);
}

const MaxWSatInstance& MaxWSatSolver::instance() const { return *instance_; }