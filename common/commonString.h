#ifndef __COMMONSTRING_H__
#define __COMMONSTRING_H__
#include "cocos2d.h"
#include <string>
using namespace std;
//1 头盔，2 盔甲，3 鞋子，4 武器，5 首饰，6 宝物
//static const string ARMORITEMTYPE[] = {"頭盔","盔甲","鞋子","武器","首飾","寶物"};
static const string ARMORITEMTYPE[] = {"头盔","盔甲","鞋子","武器","首饰","宝物"};
static string getArmorTypeName(int type)
{
	if (type>0&&type<7)
	{
		return ARMORITEMTYPE[type-1];
	}
	return "";
}

#endif