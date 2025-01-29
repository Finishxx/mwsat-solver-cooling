#include "Cooling.h"

CoolingSchedule::CoolingSchedule(
    uint32_t equilibrium,
    double coolingFactor,
    double startTemperature,
    double stopTemperature,
    uint32_t stopAfterTotalSteps,
    uint32_t stopAfterNoChange,
    uint32_t stopAfterNoBetterment
)
    : startTemperature(startTemperature),
      coolingFactor(coolingFactor),
      equilibrium(equilibrium),
      stopTemperature(stopTemperature),
      stopAfterTotalSteps(stopAfterTotalSteps),
      stopAfterNoChange(stopAfterNoChange),
      stopAfterNoBetterment(stopAfterNoBetterment) {}