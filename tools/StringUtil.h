#ifndef __STRING_UTIL_H__
#define __STRING_UTIL_H__

#include "cocos2d.h"
#include <string>
using namespace std;

USING_NS_CC;

//用分割符分割字符串，结果存放到一个列表中，列表中的对象为CCString
CCArray* strSplit(const char* strStr, const char* strSymbol);
//字符串替换功能
string& strReplace(string& str,const string&old_value,const string& new_value);
//出去字符串中空格
string& strRemoveSpace(string& str);

#endif	//__STRING_UTIL_H__
