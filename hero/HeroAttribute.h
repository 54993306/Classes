
#ifndef __HERO_ATTRIBUTE_
#define __HERO_ATTRIBUTE_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "mainCity/CityData.h"
#include "net/CNetClient.h"
#include "bag/bagData.h"

const int HERO_ARMOR_LOCK_LEVEL[4] = {1, 5, 10, 15};

class CHeroBodyShowData;
class CHeroDelegate;

class CHeroAttribute: public BaseLayer
{
public:
	CREATE_LAYER(CHeroAttribute);
	CHeroAttribute();
	~CHeroAttribute();

    virtual bool init();
    void onEnter();
    void onClose(CCObject* pSender);
    void onExit();
	void updateHeroAttr(CHero* hero);	
	void showBaseInfo(CHero* hero);

	void showArmor(CHero* hero);

	void updateRaiseAttr(CHero* hero);

	void setDelegate(CHeroDelegate *delegate);
	CHeroDelegate* getDelegate();
	void updateLeftRightBtn(bool isLeft, bool isVis);
	void showUpLevelAnimation();
	void hideHeroOperation();	
	void heroLevelUpTip();
	void levelSkillUp(CHero* hero,const Hero* pHero);

	void runAnimation(float dt, CCNode * node);

	void callbackForHideBackLayer();

	void movementCallBack(CCArmature* pAramture, MovementEventType type, const char* sData);

	void compaseSuccess(CCObject* pObj);
	void updateStarRank(CHero* hero);
protected:
	void onSelectHero(CCObject *pSender);
	void onSelectItem(CCObject *pSender);
	void onCheckBtn(CCObject *pSender, bool bChecked);
	void onCheckAdvanceBtn(CCObject *pSender, bool bChecked);
	void onExpbtn(CCObject* pSender);
	void onBreakBtn(CCObject* pSender);
	void onSelectArmor(CItem* pItem);
	void updateHeroInfo(const TMessage& tMsg);
	void ProcessMsg(int type, google::protobuf::Message *msg);
	void onLeftHero(CCObject* pSender);
	void onRightHero(CCObject* pSender);
	bool onlongClickBreak(CCObject* pSender, CCTouch* pTouch);
	void onlongClickEnd(CCObject* pSender, CCTouch* pTouch, float fDuration);
	void onAddFood(float dt);
	void isUseFood(CCObject* pSender);
	void onAddUseItem(float dt);
	void updateHeroExp(const TMessage& tMsg);
	void onRaiseExpBtn(CCObject *pSender, bool bChecked);
	void onItemProperity(CCObject *pSender);
	void showSelectRadioImg(int selIndex);

	void onTipsForUnlockArmor(CCObject *pSender);

	void bgHideOrShow(bool bShow);
	void updateBgColor(int iType);

	void showHeroBook(CCObject* pSender, bool bChecked);

private:
	//更新滑动区域
	void updateShowInfoScroll();

private:
	CLayout *m_ui;
	CHero *m_selectHero;
	CHeroDelegate *m_delegate;

	int  m_itemIndex;
	int  m_foodCount;
	vector<int> m_VecMode;

	bool m_bIsToLeft;
	CCArmature* m_pArmature;
	CHeroBodyShowData* m_heroBodyShowdData;

	CHero* m_pHeroTemporary;
	//展示滚动区域
	CScrollView* m_show_info_scroll;
	CLabel* m_pInfo1;//姓名（火）
	CLabel* m_pInfo2;//详细描述
};


#endif