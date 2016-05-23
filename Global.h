#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "Resources.h"
static const char* DEFAULT_FONT = "Arial";

#define CSV_ROOT(__URL__) ("csv/" __URL__)

#define WINSIZE (CCDirector::sharedDirector()->getWinSize())

#define random_range(low, high) ((rand() % (high - low + 1)) + low)

#define abs(a) ((a)>0?(a):-(a))

#endif // !_GLOBAL_H_
