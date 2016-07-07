#ifndef _RECRUITRESULT_h_
#define _RECRUITRESULT_h_

#include "AppUI.h"
#include "RecruitData.h"
#include "scene/layer/LayerManager.h"
#include<map>

using namespace std;

class CNewHero;
class CMonsterInfo;
class CItemInfo;

class RecruitResult : public BaseLayer
{
public:
	RecruitResult();
	~RecruitResult();
	CREATE_LAYER(RecruitResult);
	virtual bool init();
	virtual void onEnter();
	virtual void onExit();
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent);
	void setResult(vector<HeroLotteryData>& result,int type,int num);
	void onesRecruit();
	void tensRecruit();
	void turnDispose(bool turn = false);
	void setHeroIcon(CCSprite* icon,CCSprite*quality,CLabel* name,HeroLotteryData& data,CCSprite* newicon = nullptr);
	void ResBtnCallBack(CCObject* ob);

	void netGetRes(float dt);

	CCObject* EFviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	CCObject* HeroviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void EFAddCell(unsigned int uIdx, CGridViewCell* pCell);
	void HeroAddCell(unsigned int uIdx, CGridViewCell* pCell);
	void BuyAgain();
	void AgainUpRoleData();
	void ReceiveInfo(int type, google::protobuf::Message *msg);
	void turnSureRes(float dt);

	void openMovementEvent(CCArmature *pArmature, MovementEventType iType, const char *name);
	void openFrameEvent(CCBone *pBone, const char* sKey, int iA, int iB);

	void showHeroCall(CCNode *pNode);
	void heroInfoAnim();

	void playNewHeroAnim(float dt);

	void continueMoveLight(float dt);

	void checkRank();

	void showCoffinWithRank();

	void showArrowAndButton();

	void touchCoffin(CCObject* pSender);

	void pressCoffin(CCObject* pSender, CTouchPressState iState);

	void showHero();

	void resetArmature();

	void cellEffectCallBack(CCNode* pSender);

	void showNewHeroAnimation();

	void showNewHeroAnimationCallBack();

	void showHeroCardCallBack();

	void flySoundCallBack();
	void onComfirmOneTime(CCObject* pSender);
	void onComfirmTenTime(CCObject* pSender);
	void updateCostNum(int num);

	void onPress(CCObject* pSender, CTouchPressState iState);

	void roleUpdate(const TMessage& tMsg);
	void callbackForSound();

	void callBackForCloesed();

	void showHand();
	void hideHand();
	void setHeroViewVis(bool isVisible);
private:
	int m_Type;
	int m_needNum;
	bool m_BtnTurn;
	CLayout* m_ui;
	CLayout* m_cell1;
	CLayout* m_cell2;
	CLayout* m_Res;
	CLayout* m_Turn;
	CGridView* m_EFView;
	CGridView* m_HeroView;
	vector<HeroLotteryData> m_LotteryResult;
	CCArmature *m_armature;
	int  m_cellIndex;
	int m_iLotteryRank;
	CNewHero* m_pNewHeroEffect;

	CMonsterInfo* m_pMonsterInfo;
	CItemInfo* m_pItemInfo;

	CCArmature* m_pHand;

	bool m_bIsSuccess;
};

#endif