#ifndef __Cultivate_LAYER_
#define __Cultivate_LAYER_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
//培植类
#include "CityData.h"
class CCultivateLayer:public BaseLayer
{
public:
	CCultivateLayer():m_selectHero(0),
		m_time(nullptr),m_second(0),m_pCoinBatch(nullptr),
		m_iCurrentGetMoney(0),m_iCurrentMoneyRank(0),
		m_bAddMoneyScheduleLock(false),m_fCultivateTime(0),m_iBase(0),m_iNow(0),m_iMax(0),m_fSpeed(0)
	{}
	~CCultivateLayer(){}
	CREATE_LAYER(CCultivateLayer);
	virtual bool init();
	void onEnter();
	void onClickButton(CCObject* pSender);
	void onExit();

	void showBuildInfo(const CCity& ct);

	void recBuildInfo(int type, google::protobuf::Message *msg);

	void updateHero(const TMessage& tMsg);

	void updateGetMoneyHeap(const TMessage& tMsg);

	void onClose(CCObject* pSender);

	void updateToAddCoin(float dt);

	void updateTime(float dt);

	void updateLabel(float dt);

protected:
	void runFadeAction(const CCity &ct);
	void timeDelay(float delt);
	void waitTime();

	void showTime();

	//计算轮子旋转时间
	float calculateCircleActionTime(int iHeroSize);

	void updateCircle(int iHeroSize);

	//计算金钱罐收集等级
	int calculateMoneyHeapRank(int iProgress);
	//展示金钱罐等级
	void showMoneyHeap(int iRank);//0空， 1少量， 2中量，3大量
	void showGetMoneyHeapEffect();
	void showGetMoneyHeapEffectCallBack();

	void showGiftWave( CCString* pStr, CCPoint pBasePos, CCPoint pAimPos );

	void updateGetMoneyButtonStatus();

private:
	CLayout *m_ui;
	CCity m_city;
	int  m_selectHero;
	CLabel *m_time;
	int m_second;
	CCSpriteBatchNode* m_pCoinBatch;
	int m_iCurrentGetMoney;
	int m_iCurrentMoneyRank;
	bool m_bAddMoneyScheduleLock;
	float m_fCultivateTime;
	int m_iBase;
	int m_iNow;
	int m_iMax;
	float m_fSpeed;
};



#endif // !__GOLDMINE_LAYER_