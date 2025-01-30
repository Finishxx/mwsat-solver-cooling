#include <SatCooling.h>

#include <CLI/CLI.hpp>
#include <filesystem>
#include <iostream>

#include "Cooling.h"
#include "Rng.h"
#include "dimacsParsing.h"

int main(int argc, char** argv) {
  CLI::App app{"App description"};

  std::filesystem::path inputPath;
  app.add_option("-f,--file", inputPath, "A help string")->required();

  std::string seedStr;
  uint64_t seed;
  app.add_option("-s,--seed", seedStr, "64-bit hex seed")->required();

  double startTemperature;
  app.add_option("-t,--startTemperature", startTemperature)->required();
  double endTemperature;
  app.add_option("-T,--endTemperature", endTemperature)->required();

  double cooling;
  app.add_option("-c,--cooling", cooling, "Cooling coefficient")->required();

  uint32_t equilibrium;
  app.add_option("-e,--equilibrium", equilibrium)->required();

  std::filesystem::path debugPath;
  app.add_option(
      "-d,--debug",
      debugPath,
      "Where to write <stepNum> <currentSatisfied> <currentWeight> "
      "<maxWeight> on each line per step"
  );

  uint32_t maxIterations;
  app.add_option("-i,--maxIterations", maxIterations, "Iterations before end")
      ->required();

  uint32_t withoutGain;
  app.add_option("-g,--gain", withoutGain, "End after steps without gain")
      ->required();

  uint32_t withoutChange;
  app.add_option(
         "-a,--adjustment", withoutChange, "End after steps without change"
  )
      ->required();

  CLI11_PARSE(app, argc, argv);

  // Check input path
  if (!exists(inputPath)) {
    std::cerr << "Input file " << inputPath << " does not exist" << std::endl;
    return EXIT_FAILURE;
  }
  // Check seed
  try {
    seed = std::stoi(seedStr);
  } catch (...) {
    std::cerr << "Invalid seed: " << seedStr << std::endl;
    return EXIT_FAILURE;
  }
  Rng::initWithSeed(seed);

  std::ifstream stream(inputPath.c_str());
  ParsedDimacsFile input = parseDimacsFile(stream);
  CoolingSchedule schedule(
      equilibrium,
      cooling,
      startTemperature,
      endTemperature,
      maxIterations,
      withoutChange,
      withoutGain
  );
  SatCooling satCooling(input.clauses, input.weights);
  Cooling<SatConfig, SatCriteria, SatCooling> simulatedCooling(
      satCooling, schedule
  );
  std::cout << "Here" << std::endl;

  simulatedCooling.simulateCooling();

  SatCriteria criteria = simulatedCooling.copyBestCriteria();
  std::cout << "Criteria: " << criteria.satisfied() << std::endl;
  std::cout << "Weight: " << criteria.weight() << std::endl;

  return 0;
}