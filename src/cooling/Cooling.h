#pragma once

#include <concepts>
#include <cstdint>

/**
 * Generic simulated cooling solver
 * Requires a Problem, Configuration and Optimization Criteria
 *
 * Problem
 * - Interface with following methods:
 *  - Configuration getRandomConfiguration()
 *  - Configuration getRandomNeighbor(Configuration)
 *  - Criteria evaluateConfiguration(Configuration)
 *  - void applyConfiguration(Configuration)
 * Configuration
 *  - Just output class we know nothing about and is passed around
 * Optimization Criteria
 *  - bool {better|worse}[orEqual](Criteria)
 *  - double howMuch{better|worse}(Criteria)
 */
class Criteria {
 public:
  bool operator==(const Criteria& other) const;
  bool operator<(const Criteria& other) const;
  bool operator>(const Criteria& other) const;
  bool operator>=(const Criteria& other) const;
  bool operator<=(const Criteria& other) const;
  double howMuchBetterThan(const Criteria& other) const;
  double howMuchWorseThan(const Criteria& other) const;
};
class Configuration {};
class Problem {
  Configuration getRandomConfiguration();
  Configuration getRandomNeighbor(const Configuration& configuration);
  Criteria evaluateConfiguration(const Configuration& configuration);
  void applyConfiguration(const Configuration& configuration);
};

template <typename T>
concept Criteriable = std::totally_ordered<T> && requires(T t1, T t2) {
  { t1.howMuchWorseThan(t2) } -> std::convertible_to<double>;
  { t1.howMuchBetterThan(t2) } -> std::convertible_to<double>;
};

template <typename T, typename Configuration, typename Criteria>
concept Problemable = requires(T t, Configuration configuration) {
  { t.getRandomConfiguration() } -> std::convertible_to<Configuration>;
  { t.getRandomNeighbor(configuration) } -> std::convertible_to<Configuration>;
  { t.evaluateConfiguration(configuration) } -> std::convertible_to<Criteria>;
  { t.applyConfiguration(configuration) };
};

/** Data controlling the schedule of cooling and how long the search lasts */
struct CoolingSchedule {
  /** How many steps before ending the search*/
  uint32_t maxTries;
  double startTemperature;
  double stopTemperature;
  double coolingFactor;
  /** How many steps before cooling the temperature */
  uint32_t equilibrium;
  CoolingSchedule(
      uint32_t maxTries, uint32_t equilibrium, double coolingFactor,
      double startTemperature, double stopTemperature
  )
      : maxTries(maxTries),
        startTemperature(startTemperature),
        stopTemperature(stopTemperature),
        coolingFactor(coolingFactor),
        equilibrium(equilibrium) {}
};

/**
 * Searches for best Criteria producing Configuration solving a given Problem
 * bounded by provided CoolingSchedule
 *
 * Ownership
 *  - Problem owns his own data and returns copies
 *  - Cooling owns his own copies of Problem data and returns copies
 */
template <typename Configuration, Criteriable Criteria, typename Problem>
  requires Problemable<Problem, Configuration, Criteria>
class Cooling {
 private:
  CoolingSchedule schedule;
  Problem problem;

  Configuration bestConfiguration;
  Criteria bestCriteria;

  Configuration currentConfiguration;
  uint32_t equilibriumIteration = 0;
  uint32_t tries = 0;
  double temperature;

 public:
  Cooling(Problem problem, Configuration start, CoolingSchedule properties);
  /** Starting config is chosen at random  */
  Cooling(Problem problem, CoolingSchedule properties);

  /** @name Schedule */
  ///@{
  bool isOver() const {
    return schedule.maxTries >= tries || temperature < schedule.stopTemperature;
  }
  bool isNotOver() const { return !isOver(); }
  const CoolingSchedule& schedule();
  ///@}

  /// @name Search execution
  ///@{
  /** Does one step in equilibrium @return true if search not over */
  bool step();
  /** All necessary steps to next equilibrium @return true if search not over */
  bool runToNextEquilibrium();
  /** Does as many step as necessary to end the search */
  void runToEnd();
  ///@}

  /// @name Search state
  ///@{
  Configuration copyCurrentConfiguration() const;
  Configuration copyBestConfiguration() const;
  Criteria copyBestCriteria() const;
  Criteria copyCurrentCriteria() const;
  ///@}
};
