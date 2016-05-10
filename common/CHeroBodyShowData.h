#pragma once

#include "cocos-ext.h"
#include "cocos2d.h"

USING_NS_CC;
USING_NS_CC_EXT;

using namespace std;

struct CHeroBodyShowInfo
{
	int iModelId;
	CCPoint pos;
	float fScale;
	bool bFlipX;
	int iRoate;
	string sPic;
};

typedef vector<CHeroBodyShowInfo> CHeroBodyShowDataList;	

class CHeroBodyShowData
{
public:
	CHeroBodyShowData();
	void reloadFile();
	const CHeroBodyShowInfo& getDataById(int iId);
private:
	CHeroBodyShowDataList _data;
};