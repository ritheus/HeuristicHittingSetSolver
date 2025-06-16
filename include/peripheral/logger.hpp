#pragma once

#include <iostream>

//#define LOGGING
//#define LOGGINGBNR
//#define LOG_LOCALSEARCH


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

#ifdef LOG_LOCALSEARCH
#define LOG_LOCALSEARCH(x) std::cerr << x << std::endl;
#else
#define LOG_LOCALSEARCH(x)
#endif
