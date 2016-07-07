#pragma once

/******************************************************
*文件名称:	CHeroBodyShowData.h
*编写日期:	2016-6-14-11:38
*编写作者:	YPF
*功能描述:	英雄立绘位置配置
*******************************************************/

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