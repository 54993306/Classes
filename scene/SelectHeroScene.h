#ifndef _SELECT_HERO_SCENE_H_
#define _SELECT_HERO_SCENE_H_

#include "AppUI.h"
#include "net/MessageHandler.h"
#include "common/CursorTextField.h"
#include "model/RoleNameGM.h"
#include "update/CDownloadPackageDelegate.h"

const int SelectHeroScene_role[3] = {346, 146, 541};

namespace BattleSpace
{
	class ArmatureEventDataMgr;
	class ArmatureEventData;
}

class SelectHeroScene : public CScene,public MessageHandler, public DownloadPackageDelegate
{
public:
	SelectHeroScene();
	DEFINE_SCENE_CLASS(SelectHeroScene);
	virtual void onCreate();
	virtual void onEnter();
	virtual void onExit();

	bool ProcessMsg(int type, google::protobuf::Message *msg);

	//virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

	void movementCallBack(CCArmature *armature, MovementEventType type, const char *data);

	virtual void downloadPackageEnd(bool bAnswer);

protected:
	void onOk(CCObject* ob);
	void onClose(CCObject* ob);
	void onSelect(CCObject* ob);

	void roleAnimaiton(int tag, bool isSelect);

	void addFuwenAnimation(char* animaName, const CCPoint& pos, float rotate, int zOrder);

	void onRandowName(CCObject* pSender);

	void runCloudAction();
	void relocation(CCNode* pSender);

	void showFirstIn();

	void showFirstInCallBack();

	void selectLayerShowThree();

	void selectLayerShowOne();

	void showArmature();

	void showHeroAnimation();

	void showHeroAnimationCallBack();

	void hideHeroAnimation();

	void hideHeroAnimationCallBack();

	void updateInputLayer();

	void showShake();

	void showShakeCallBack();

	void showNameAction(CCNode* pNode);

	void showBlackEffect(CCSprite* pSprite);

	void updateFrame(float dt);

	void onClickDownloadPackage(CCObject* pSender);

protected:

	int m_selectRaceType;
	CursorTextField* m_textFieldName;

	CLayout *m_ui;
	CLayout *m_selectLayer;
	CLayout *m_InputLayer;

	RoleNameGM m_nameGm;

	CCPoint m_cloudPos;
	CCPoint m_cloudAPos;
	CLayout* m_fogLay;
	CLayout* m_fogLay1;

	CCPoint m_roleBaePos[3];
	float m_roleScaleX[3];
	float m_roleScaleY[3];

	bool m_bTouchLock;
	CCArmature* m_pArmature[3];

	CCNode* m_pClose;

	BattleSpace::ArmatureEventDataMgr* m_armatureEventDataMgr;
	BattleSpace::ArmatureEventData* m_armatureEventData;
	int m_lastFrame;
};
#endif // !_SELECT_HERO_SCENE_H_
