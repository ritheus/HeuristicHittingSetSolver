#pragma once

#include <iostream>

//#define LOGGING
//#define LOGGINGBNR


#ifdef LOGGING
#define LOG(x) std::cerr << x << std::endl;
#else
#define LOG(x)
#endif

#ifdef LOGGINGBNR
#define LOGBNR(x) std::cerr << x << std::endl;
#else
#define LOGBNR(x)
#endif