#pragma once

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "model/BattleData.h"
#include <spine/spine-cocos2dx.h>

using namespace spine;
//请求世界BOSS数据
void inline AskForWorldBoss()
{
	GetTcpNet->sendDataType(BossDataMsg);
}

//购买鼓舞
void inline AskForBuyInspire()
{
	GetTcpNet->sendDataType(BuyInspireMsg);
}

//重置挑战时间
void inline AskForRestChallengeTime()
{
	GetTcpNet->sendDataType(ResetBattleTimeMsg);
}

class CSkillInfo;
class StoryData;

class CWBossLayer: public BaseLayer
{
public:
	CWBossLayer();
	~CWBossLayer();
	
	CREATE_LAYER(CWBossLayer);

    virtual bool init();
    void onEnter();
    void onExit();

	//网络回调
	void processNetMsg(int type, google::protobuf::Message *msg);

	int getBossId();

	void SpineComplete( int trackIndex,int loopCount );
private:
	void updateOneSecond(float dt);
	
	//更新数据
	void updateData(BossData* pData);

	void updateDataBossCome(BossData* pData);

	void updateDataBossWait(BossData* pData);

	std::string secondToString1(int iSecond);

	std::string secondToString2(int iSecond);

	std::string secondToString3(int iSecond);

	CCNode* packageWithClippingNode(CCNode* pNode);

	void onClickSkill(CCObject* pSender);
	void reset(CCObject* pSender);
	void resetConfirm(CCObject* pSender);

	void onBattle(CCObject* pSender);
	void inspire(CCObject* pSender);
	void inspireConfirm(CCObject* pSender);
	void showRank(CCObject* pSender);

	void showStory(CCObject* pSender);
	
	void pressSkill(CCObject* pSender, CTouchPressState iState);
	void pressRewardBox(CCObject* pSender, CTouchPressState iState);

	void updateBossWillLeaveLabel();
	void updateBossWillComeLabel();
	void updateResetFightLabel();
	void updateInspireLabel(bool bWithAction=false);

	void updateBossTexture();

	void movementCallBack(CCArmature* pAramture, MovementEventType type, const char* sData);

	void callBackForSetArmatureScale();

private:
	bool isBossCome();
private:
	CLayout* m_ui;
	int m_iBossCome;		//战斗状态(1 世界BOSS进行中，2 未开启，3 已结束)
	bool m_bFightedBoss;
	int m_iNextChallengeSecond;
	int m_iBossSecond;
	int m_iResetNeedGold;
	int m_iInspireAdd;
	int m_iInspireCost;
	CMonster m_pBoss;
	CLayout* m_pLayerShow;
	int m_iBuffId;
	CSkillInfo* m_pSkillInfo;
	CCNode* m_pArmature;
	StoryData* m_pStoryData;
};