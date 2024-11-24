#pragma once
#include <sat.h>

#include <istream>

SatInput parseDimacsFile(std::istream input);