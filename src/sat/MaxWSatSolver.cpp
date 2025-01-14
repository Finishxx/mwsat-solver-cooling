#include "MaxWSatSolver.h"

// ===================== LiveTerm =====================
MaxWSatSolver::LiveTerm::LiveTerm(Term original, bool isSet)
    : isSet_(isSet), original(original) {
  if ((isSet && original.isPlain()) || (!isSet && original.isNegated())) {
    isSatisfied_ = true;
  } else {
    isSatisfied_ = false;
  }
}
const Term& MaxWSatSolver::LiveTerm::originalTerm() const { return original; }
uint32_t MaxWSatSolver::LiveTerm::id() const { return original.id(); }
bool MaxWSatSolver::LiveTerm::isSatisfied() const { return isSatisfied_; }
bool MaxWSatSolver::LiveTerm::isSet() const { return isSet_; }
bool MaxWSatSolver::LiveTerm::isUnset() const { return !isSet_; }
void MaxWSatSolver::LiveTerm::set() {
  if (!isSet_) {
    isSet_ = true;
    // Satisfied if set and the original was plain
    isSatisfied_ = original.isPlain();
  }
}

void MaxWSatSolver::LiveTerm::unset() {
  if (isSet_) {
    isSet_ = false;
    // Satisfied if unset and the original was negated
    isSatisfied_ = original.isNegated();
  }
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