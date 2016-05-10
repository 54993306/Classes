#pragma once

#include "cocos-ext.h"
#include "cocos2d.h"

USING_NS_CC;
USING_NS_CC_EXT;

using namespace std;

struct CWholeBodyShowInfo
{
	int iModelId;
	CCPoint pos;
	float fScale;
	bool bFlipX;
	int iRoate;
	string sPic;
};

typedef vector<CWholeBodyShowInfo> CHeroWholeBodyShowDataList;	

class CWholeBodyShowData
{
public:
	static CWholeBodyShowData* getInstance();
	static void destroy();
	CWholeBodyShowData();
	void reloadFile();
	const CWholeBodyShowInfo& getDataById(int iId);
private:
	CHeroWholeBodyShowDataList _data;
	static CWholeBodyShowData* m_pInstance;
};