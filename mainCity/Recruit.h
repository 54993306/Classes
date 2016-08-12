#pragma once


#include "AppUI.h"
#include "model/CRoleData.h"
#include "RecruitData.h"
#include<map>


class CMonsterInfo;
class CItemInfo;

using namespace std;
class CRecruit : public BaseLayer
{
public:
	CRecruit();
	virtual ~CRecruit();
	CREATE_LAYER(CRecruit);
	virtual void onEnter();
	virtual void onExit();
	virtual bool init();

	void initRecruiData(int type, google::protobuf::Message *msg);

	void showFreeTime();

	void HeroLotteryInfo(int type, google::protobuf::Message *msg);

	void onPress(CCObject* pSender, CTouchPressState iState);

	void show(CCObject* pObj);
	void hide(CCObject* pObj);
	void updateCostNum(int num);

	void roleUpdate(const TMessage& tMsg);	
	void showRedTip(bool isVisable);
protected:
	void InitRecrui(int num1,int num2,int num3);
	void ReturnCity(CCObject* ob);
	void gridChangeImg(int index);
	void LotteryBtn(CCObject* ob);
	void upRoleData();
	void onComfirmTenTime(CCObject* pSender);
	void onComfirmOneTime(CCObject* pSender);
	void updateTime(float dt);
protected:
	CLayout* m_ui;
	CLayout* m_RecruiLayer;
	vector<HeroLotteryData> m_LotteryData;
	bool m_Recruit;												//是否选择可招募
	int m_HeroType;												//武将类型
	int m_SendType;												//记录抽奖类型
	CLayout* m_heroLayer;

	CMonsterInfo* m_pMonsterInfo;
	CItemInfo* m_pItemInfo;
};
