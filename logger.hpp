#pragma once

#include <iostream>

#define LOGGING


#ifdef LOGGING
#define LOG(x) std::cerr << x << std::endl;
#else
#define LOG(x)
#endif
