#pragma once

#include "AppUI.h"

using namespace std;

#define FRAME_ACTION_HEAD_CHILD_TAG 11
#define FRAME_ACTION_TEXT_CHILD_TAG 12

struct CFrameActionData
{
	int iModelId;
	CCPoint pos[2];
	int iOpacity[2];
	float fScale;
	bool bFlipX;
	int iRoate;
	float fTime;
	string sHeadPic;
	string sTextPic;
};

typedef vector<CFrameActionData> CFrameActionDataList;	

class CPlayerSkillData
{
public:
	CPlayerSkillData();
	void reloadFile();
	const CFrameActionData& getDataById(int iId);
private:
	CFrameActionDataList _data;
};