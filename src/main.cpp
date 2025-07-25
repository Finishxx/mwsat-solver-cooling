#include <SatCooling.h>

#include <CLI/CLI.hpp>
#include <filesystem>
#include <iostream>
#include <ranges>

#include "Cooling.h"
#include "Rng.h"
#include "dimacsParsing.h"

int main(int argc, char** argv) {
  CLI::App app{
      "Solves maximum weighted sat instances in the MWSAT format using "
      "simulated cooling method.\n\n The result is printed in the format of "
      "<inputFileName> <weight> <variable1> <variable2> ... <variableN>"
  };

  std::string inputFileName;
  app.add_option(
         "-f,--file", inputFileName, "Path to instance in the MWSAT format"
  )
      ->required();

  std::string seedStr;
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
  CLI::Option* debugOption = app.add_option(
      "-d,--debug",
      debugPath,
      "Where to write <step> <satisfied> <weight> <bestWeight> on each "
      "line per step"
  );

  uint32_t maxIterations = 0;
  app.add_option(
      "-i,--maxIterations",
      maxIterations,
      "Iterations before end, if 0 then infinite"
  );

  uint32_t withoutGain = 0;
  app.add_option(
      "-w,--withoutGain",
      withoutGain,
      "End after steps without gain, if 0 then infinite"
  );

  uint32_t withoutChange = 0;
  app.add_option(
      "-W,--withoutChange",
      withoutChange,
      "End after steps without change, if 0 then infinite"
  );

  bool extendedOutput = false;
  app.add_option(
      "-E, --extendedOutput",
      extendedOutput,
      "Show extended output after completion in the format of: \n"
      "First line is normal <fileName> <weight> <variable1> ... <variableN>. \n"
      "Second line is <endedBecause> <isSatisfied> <satisfiedCount> \n"
      "<stepsTotal> <stepsSinceChange> <stepsSinceGain>, \n"
      "where endedBecause is one of: temperature|max|change|gain|unknown"
  );

  CLI11_PARSE(app, argc, argv);

  // Steps correction
  if (maxIterations == 0) maxIterations = UINT32_MAX;
  if (withoutChange == 0) withoutChange = UINT32_MAX;
  if (withoutGain == 0) withoutGain = UINT32_MAX;

  // Check input path
  auto hello = std::ranges::views::drop_while(inputFileName, [](char c) {
    return std::isspace(c);
  });
  std::string world(hello.begin(), hello.end());
  std::filesystem::path inputPath(world);
  if (!exists(inputPath)) {
    std::cerr << "Input file " << inputPath << " does not exist" << std::endl;
    return EXIT_FAILURE;
  }

  // Set seed
  Rng::deserializeSeed(seedStr);

  // Prepare cooling
  std::ifstream inputStream(inputPath.c_str());
  ParsedDimacsFile input = parseDimacsFile(inputStream);
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

  // Setup debug output
  bool debugEnabled = false;
  std::ofstream debugStream;
  if (*debugOption) {
    debugEnabled = true;
    debugStream = std::ofstream(debugPath.c_str());
  }

  // Simulated cooling main loop
  while (simulatedCooling.step()) {
    if (debugEnabled) {
      const SatCriteria& current = simulatedCooling.getCurrentCriteria();
      const SatCriteria& best = simulatedCooling.getBestCriteria();
      debugStream << simulatedCooling.getStepsTotal() << " "
                  << current.satisfied() << " " << current.weight() << " "
                  << best.weight() << std::endl;
    }
  }

  SatCriteria finalCriteria = simulatedCooling.copyBestCriteria();
#ifdef DEBUG_ENABLED
  std::cout << "SatisfiedCount: " << finalCriteria.satisfied() << std::endl;
  std::cout << "Weight: " << finalCriteria.weight() << std::endl;
  std::cout << "Ended after " << simulatedCooling.getStepsTotal()
            << " iterations" << std::endl;
  std::cout << "Steps since change: " << simulatedCooling.getStepsSinceChange()
            << std::endl;
  std::cout << "Steps since betterment: "
            << simulatedCooling.getStepsSinceBetterment() << std::endl;
#endif

  // Standard print
  std::cout << inputPath.filename().string() << " " << finalCriteria.weight()
            << " ";
  SatConfig config = simulatedCooling.copyBestConfiguration();
  for (int i = 1; i < config.underlying.size() + 1; i++) {
    if (config.underlying[i - 1] == true)
      std::cout << i;
    else
      std::cout << -i;
    if (i != config.underlying.size()) std::cout << " ";
  }

  if (extendedOutput) {
    std::cout << std::endl;
    std::cout << simulatedCooling.endedBecause() << " "
              << finalCriteria.isSatisfied() << " " << finalCriteria.satisfied()
              << " " << simulatedCooling.getStepsTotal() << " "
              << simulatedCooling.getStepsSinceChange() << " "
              << simulatedCooling.getStepsSinceBetterment() << std::endl;
  }

  return 0;
}