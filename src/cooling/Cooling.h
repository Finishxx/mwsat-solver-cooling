#pragma once

#include <cmath>
#include <concepts>
#include <cstdint>

#include "Rng.h"

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
  Criteria(const Criteria& other);
  [[nodiscard]] bool isValid() const;
  bool operator<(const Criteria& other) const;
  [[nodiscard]] double howMuchWorseThan(const Criteria& other) const;
};
class Configuration {
 public:
  Configuration(const Configuration& other);
  bool operator==(const Configuration& other) const;
  bool operator!=(const Configuration& other) const;
};
class Problem {
  [[nodiscard]] Configuration currentConfiguration() const;
  [[nodiscard]] Configuration getRandomConfiguration() const;
  [[nodiscard]] Configuration getRandomNeighbor(
      const Configuration& configuration
  ) const;
  [[nodiscard]] Criteria evaluateConfiguration(
      const Configuration& configuration
  ) const;
};

template <typename T>
concept Configurable = std::equality_comparable && std::copy_constructible;

template <typename T>
concept Criteriable = std::copy_constructible<T> && requires(T t1, T t2) {
  { t1.isValid() } -> std::convertible_to<bool>;
  { t1 < t2 };
  { t1 >= t2 } -> std::convertible_to<bool>;
  { t1.howMuchWorseThan(t2) } -> std::convertible_to<double>;
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
template <Configurable Configuration, Criteriable Criteria, Problemable Problem>
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
  bool isFrozen() const {
    return temperature <= schedule.stopTemperature ||
        schedule.stopAfterTotalSteps <= stepsTotal ||
        schedule.stopAfterNoChange <= stepsSinceChange ||
        schedule.stopAfterNoBetterment <= stepsSinceBetterment;
  }
  bool notFrozen() const { return !isFrozen(); }
  const CoolingSchedule& coolingSchedule() const { return schedule; }
  ///@}

  /// @name Search execution
  ///@{
  /** Does one step in equilibrium @return true if search not over */
  bool step() {
    if (isFrozen()) return false;

    return true;
    // TODO: Rest
  }
  /** Does as many step as necessary to end the search */
  void simulateCooling() {
    while (notFrozen()) {
      // Is equilibrium over?
      if (stepsInEquilibrium >= schedule.equilibrium) {
        temperature = temperature * schedule.coolingFactor;
        stepsInEquilibrium = 0;
        continue;
      }

      // Update steps
      stepsInEquilibrium++;
      stepsTotal++;
      stepsSinceBetterment++;
      stepsSinceChange++;

      Configuration candidate = problem.getRandomNeighbor(candidate);
      Criteria candidateCriteria = problem.evaluateConfiguration(candidate);
      if (candidate == currentConfig) {
        continue;
      }

      // Swap current with candidate
      if (candidateCriteria >= currentCriteria) {
        swapCandidate(candidate, candidateCriteria);
        continue;
      }

      // else: decide if we want to apply the less good candidate anyway
      uint32_t difference = candidateCriteria.howMuchWorseThan(currentCriteria);
      if (Rng::nextDoublePercent() < std::exp(-(difference / temperature))) {
        swapCandidate(candidate, candidateCriteria);
      }
    }
  }

  void swapCandidate(
      const Configuration& candidate, const Criteria& candidateCriteria
  ) {
    currentConfig = candidate;
    currentCriteria = candidateCriteria;
    stepsSinceChange = 0;
    if (candidateCriteria >= bestCriteria) {
      bestConfig = candidate;
      bestCriteria = candidateCriteria;
      stepsSinceBetterment = 0;
    }
  }
  ///@}

  /// @name Search state
  /// Return copies of values regarding current search state
  ///@{
  Configuration copyCurrentConfiguration() const {
    return Configuration(currentConfig);
  }
  Configuration copyBestConfiguration() const {
    return Configuration(bestConfig);
  }
  Criteria copyCurrentCriteria() const { return Criteria(currentCriteria); }
  Criteria copyBestCriteria() const { return Criteria(bestCriteria); }
  ///@}
};
