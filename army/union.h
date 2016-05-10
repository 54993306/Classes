#ifndef __UNIONDATA_
#define __UNIONDATA_

#include "bag/bagData.h"
#include "model/BattleData.h"
struct CUnion
{
	vector<CHero> heroList;
	void read(const UnionResponse& nion);
};

#endif