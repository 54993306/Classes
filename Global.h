#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "Resources.h"
static const char* DEFAULT_FONT = "Arial";

#define CSV_ROOT(__URL__) ("csv/" __URL__)

#define WINSIZE (CCDirector::sharedDirector()->getWinSize())

#define random_range(low, high) ((rand() % (high - low + 1)) + low)

#define abs(a) ((a)>0?(a):-(a))

#define  FACEBOOKIMG "http://graph.facebook.com/%s/picture?width=92&height=92"

#define  FACEBOOKIMG_106 "http://graph.facebook.com/%s/picture?width=106&height=106"

#endif // !_GLOBAL_H_
