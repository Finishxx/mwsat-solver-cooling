#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>

// Uncomment this line to enable debug prints
#define DEBUG_ENABLED

#ifdef DEBUG_ENABLED
    #define DEBUG_PRINT(x) std::cout << x << std::endl;
#else
    #define DEBUG_PRINT(x)
#endif

#endif // DEBUG_H