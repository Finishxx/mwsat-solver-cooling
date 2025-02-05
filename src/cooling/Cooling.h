#pragma once

#include <cmath>
#include <concepts>
#include <cstdint>
#include <iostream>

#include "Rng.h"
#include "debug.h"

/** Data controlling the schedule of cooling and how long the search lasts */
struct CoolingSchedule {
  /// @name Temperature control
  ///@{
  double startTemperature;
  double coolingFactor;
  /** How many steps before cooling the temperature */
  uint32_t equilibrium;
  ///@}

  /// @name Stop control
  ///@{
  double stopTemperature;
  uint32_t stopAfterTotalSteps;
  uint32_t stopAfterNoChange;
  uint32_t stopAfterNoBetterment;
  ///@}

  CoolingSchedule(
      uint32_t equilibrium,
      double coolingFactor,
      double startTemperature,
      double stopTemperature,
      uint32_t stopAfterTotalSteps,
      uint32_t stopAfterNoChange,
      uint32_t stopAfterNoBetterment
  );
};
/**
 * Generic simulated cooling solver
 * Requires a Problem, Configuration and Optimization Criteria
 */
class Criteria {
 public:
  Criteria();
  Criteria(const Criteria& other);
  [[nodiscard]] bool isValid() const;
  bool operator<(const Criteria& other) const;
  bool operator>=(const Criteria& other) const;
  [[nodiscard]] double howMuchWorseThan(const Criteria& other) const;
};
class Configuration {
 public:
  Configuration();
  Configuration(const Configuration& other);
  bool operator==(const Configuration& other) const;
  bool operator!=(const Configuration& other) const;
};
class Problem {
 public:
  [[nodiscard]] Configuration getRandomConfiguration() const;
  [[nodiscard]] Configuration getRandomNeighbor(
      const Configuration& configuration
  ) const;
  [[nodiscard]] Criteria evaluateConfiguration(
      const Configuration& configuration
  ) const;
};

template <typename T>
concept Configurable =
    std::equality_comparable<T> && std::copy_constructible<T>;

template <typename T>
concept Criteriable = std::copy_constructible<T> && requires(T t1, T t2) {
  { t1.isValid() } -> std::convertible_to<bool>;
  { t1 < t2 };
  { t1 >= t2 } -> std::convertible_to<bool>;
  { t1.howMuchWorseThan(t2) } -> std::convertible_to<double>;
  { std::cout << t1 } -> std::same_as<std::ostream&>;
};

template <typename T, typename Configuration, typename Criteria>
concept Problemable = requires(T t, Configuration configuration) {
  { t.getRandomConfiguration() } -> std::convertible_to<Configuration>;
  { t.getRandomNeighbor(configuration) } -> std::convertible_to<Configuration>;
  { t.evaluateConfiguration(configuration) } -> std::convertible_to<Criteria>;
};

/**
 * Searches for best Criteria producing Configuration solving a given Problem
 * bounded by provided CoolingSchedule
 *
 * Ownership
 *  - Problem owns his own data and returns copies
 *  - Cooling owns his own copies of Problem data and returns copies
 *
 * What is not great:
 *  - Configuration != State
 *  - Probably missing abstraction search ending - at the moment a lot of values
 *  - Cool and frozen could be functors
 *  - Schedule seems too rigid
 */
template <Configurable Configuration, Criteriable Criteria, typename Problem>
  requires Problemable<Problem, Configuration, Criteria>
class Cooling {
 private:
  // Inputs
  CoolingSchedule schedule;
  Problem problem;

  // Search state
  Configuration currentConfig;
  Configuration bestConfig;
  Criteria currentCriteria;
  Criteria bestCriteria;
  double temperature;

  // Changes with equilibrium
  uint32_t stepsTotal = 0;
  // Changes with equilibrium
  uint32_t stepsInEquilibrium = 0;
  // Changes when we accept next candidate
  uint32_t stepsSinceChange = 0;
  // Changes when accepted candidate is better
  uint32_t stepsSinceBetterment = 0;

 public:
  [[nodiscard]] uint32_t getStepsTotal() const { return stepsTotal; }
  [[nodiscard]] uint32_t getStepsSinceChange() const {
    return stepsSinceChange;
  }
  [[nodiscard]] uint32_t getStepsSinceBetterment() const {
    return stepsSinceBetterment;
  }
  Cooling(Problem problem, Configuration start, const CoolingSchedule& schedule)
      : schedule(schedule),
        problem(problem),
        currentConfig(start),
        bestConfig(start),
        temperature(schedule.startTemperature) {
    currentCriteria = problem.evaluateConfiguration(currentConfig);
    bestCriteria = currentCriteria;
  }
  /** Starting config is chosen at random  */
  Cooling(Problem problem, const CoolingSchedule& schedule)
      : schedule(schedule),
        problem(problem),
        temperature(schedule.startTemperature) {
    currentConfig = problem.getRandomConfiguration();
    bestConfig = currentConfig;
    currentCriteria = problem.evaluateConfiguration(currentConfig);
    bestCriteria = currentCriteria;
  }

  /** @name Schedule */
  ///@{
  [[nodiscard]] bool isFrozen() const {
    if (temperature <= schedule.stopTemperature) {
      DEBUG_PRINT("Ended because of temperature")
    } else if (schedule.stopAfterTotalSteps <= stepsTotal) {
      DEBUG_PRINT("Ended because of max steps")
    } else if (schedule.stopAfterNoChange <= stepsSinceChange) {
      DEBUG_PRINT("Ended because of steps since change")
    } else if (schedule.stopAfterNoBetterment <= stepsSinceBetterment) {
      DEBUG_PRINT("Ended because of steps since betterment")
    } else {
      return false;
    }
    return true;
  }
  [[nodiscard]] std::string endedBecause() const {
    if (temperature <= schedule.stopTemperature) {
      return "temperature";
    }
    if (schedule.stopAfterTotalSteps <= stepsTotal) {
      return "max";
    }
    if (schedule.stopAfterNoChange <= stepsSinceChange) {
      return "change";
    }
    if (schedule.stopAfterNoBetterment <= stepsSinceBetterment) {
      return "gain";
    }
    return "unknown";
  }
  [[nodiscard]] bool notFrozen() const { return !isFrozen(); }
  [[nodiscard]] const CoolingSchedule& coolingSchedule() const {
    return schedule;
  }
  ///@}

  /// @name Search execution
  ///@{
  /** Does one step in equilibrium @return true if search not over */
  bool step() {
    if (isFrozen()) return false;
    // Is equilibrium over?
    if (stepsInEquilibrium >= schedule.equilibrium) {
      temperature = temperature * schedule.coolingFactor;
      stepsInEquilibrium = 0;
      return true;
    }

    // Update steps
    stepsInEquilibrium++;
    stepsTotal++;
    stepsSinceBetterment++;
    stepsSinceChange++;

    Configuration candidate = problem.getRandomNeighbor(currentConfig);
    Criteria candidateCriteria = problem.evaluateConfiguration(candidate);

    double candidateWorse = candidateCriteria.howMuchWorseThan(currentCriteria);

    DEBUG_PRINT("Candidate: " << candidateCriteria)
    DEBUG_PRINT("Current: " << currentCriteria)
    DEBUG_PRINT("CandidateWorse" << candidateWorse)

    // If better
    if (candidateWorse <= 0) {
      swapCandidate(candidate, candidateCriteria);
      return true;
    }
    // else: decide if we want to apply the less good candidate anyway

    double acceptChance = std::exp(-(candidateWorse / temperature));
    DEBUG_PRINT("Accept chance: " << acceptChance << "%")
    if (Rng::nextDoublePercent() < acceptChance) {
      swapCandidate(candidate, candidateCriteria);
    }
    return true;
  }

  /** Does as many step as necessary to end the search */
  void simulateCooling() {
    while (step()) {
    }
  }

 private:
  void swapCandidate(
      const Configuration& candidate, const Criteria& candidateCriteria
  ) {
    DEBUG_PRINT("Swapping")
    currentConfig = candidate;
    currentCriteria = candidateCriteria;
    stepsSinceChange = 0;

    double bestWorse = bestCriteria.howMuchWorseThan(currentCriteria);
    if (bestWorse > 0 && currentCriteria.isValid()) {
      bestConfig = candidate;
      bestCriteria = candidateCriteria;
      stepsSinceBetterment = 0;
    }
  }
  ///@}

 public:
  /// @name Search state
  /// Return copies of values regarding current search state
  ///@{
  const Configuration& getCurrentConfiguration() const { return currentConfig; }
  const Configuration& getBestConfiguration() const { return bestConfig; }
  Configuration copyCurrentConfiguration() const {
    return Configuration(currentConfig);
  }
  Configuration copyBestConfiguration() const {
    return Configuration(bestConfig);
  }
  const Criteria& getCurrentCriteria() const { return currentCriteria; }
  const Criteria& getBestCriteria() const { return bestCriteria; }
  Criteria copyCurrentCriteria() const { return Criteria(currentCriteria); }
  Criteria copyBestCriteria() const { return Criteria(bestCriteria); }
  ///@}
};
