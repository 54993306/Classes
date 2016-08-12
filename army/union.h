#ifndef __UNIONDATA_
#define __UNIONDATA_

#include "bag/bagData.h"
#include "model/CRoleData.h"
struct CUnion
{
	vector<CHero> heroList;
	void read(const UnionResponse& nion);
};

#endif