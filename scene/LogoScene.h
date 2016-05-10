#ifndef __LOGO_SCENE_H__
#define __LOGO_SCENE_H__

#include "AppUI.h"
#include "common/CursorTextField.h"
#include <spine/spine-cocos2dx.h>
#include <thread>
#include <functional>
using namespace cocos2d;
using namespace cocos2d::extension;

class ArmatureEventDataMgr;
class ArmatureEventData;

class LogoScene : public CScene
{
public:
	DEFINE_SCENE_CLASS(LogoScene);
	LogoScene();
	~LogoScene();
	virtual void onCreate();
	virtual void onEnter();
	virtual void onExit();
	void start(float dt);
	void testRests();
	void testAnimation(int modeid);
	void updateAction(float dt);
	void callResume(CCObject* ob);
	void onClick(CCObject* ob);
	void TestAction();
	void TestModel();
	void animationEvent(CCArmature *armature,MovementEventType movementType, const char *movementID);
	void hueChange(CCObject *pObj, CCControlEvent controlEvent);
private:
	CCSize m_LogoSize;
	CLayout* m_XmlLayer;
	string m_PauseAction;
	CursorTextField* m_InputModeId;
	CursorTextField* m_InputScale;
	CursorTextField* m_InputMap;
	CursorTextField* m_InputAction;
	ArmatureEventDataMgr* m_armatureEventDataMgr;
	ArmatureEventData* m_armatureEventData;
	CCArmature * m_Armature;
	CCSprite* m_Body;
	vector<int> m_VecModeId;
	vector<string> m_VecAction;
	int m_ModeIndex;
	int m_ActionIndex;
	bool m_RunAction;
	int m_lastFrame;
	int m_ModeID;
	string m_ActionKey;
};

enum TextAction
{
	RoleEffEctFront_Tag = 1,
	RoleEffEctBack_Tag,
	ReleaseSourse,
	MODENAME ,
	ERRORTIPS,
	ACTIONNUM,
	GotoLogin,
	ARMATURE_TAG,
	MSGTTF,
	TEST_Data,
	UpModel,
	NetModel,
	UpAction,
	NextAction,
	RepeatCurrAction,
	ReLoadData,
	TESTACTION,
	ModeID,
	ScaleNum,
	mapTest,
	mapImage,
};
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
#include <future>				//多线程有返回值情况
#endif
class SpineboyExample : public cocos2d::CCLayerColor {
public:
	SpineboyExample();
	~SpineboyExample();
	virtual bool init ();
	void DefaultCreat(int modeID);
	void LoadSpineData(int ModelID,bool isModel = true);
	void ThreadTest();
	int AddFun(int a,int b,int c);
	CREATE_FUNC (SpineboyExample);
	void InitLayer(CLayout* layer);
	void onClick(CCObject* ob);
	void CreateSkeletonByData(int id);
	int LoadSpine(int Num,SpineboyExample* ob);
	std::pair<spSkeletonData*,spAtlas*>* getData(std::string name);
	void showThread();
	void ActionEnd(int count , int trackIndex);
	bool SpineThread(vector<int>& Vec,bool LoadData = false);					//加载spine
	void SpineAsyncLoadImage(int ModelID,bool isModel = true);
	void SpineThreadCallBack(CCObject* ob);
	void AddSpineData(const char* name,spSkeletonData* skeletonData,spAtlas*atlas);
	void SpineRoleAndEffect(int id);
	void CreateAnimation(int id);

	void SpineActionEvent(int trackIndex,spEvent* Event);
	void SpineComplete(int trackIndex,int loopCount);
private:
	vector<int> m_VecSpine;
	vector<string> m_ActionName;
	vector<string> m_VecName;
	int EndCount;
	int m_index;
	CCNode* m_Node;
	CLayout* m_Layer;
	CursorTextField* m_InputIndex;
	map<std::string, std::pair<spSkeletonData*,spAtlas*>> m_MapData;
	spine::SkeletonAnimation* m_skeletonNode;
	spine::SkeletonAnimation* m_Effect;
	int									m_LoadSpineNum;						//记录已加载的spine图片的个数
	int									m_TotalSpineNum;					//spine中总共需要加载的图片个数
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	std::future<bool>					pFuture;
#endif
	int									m_CreateID;
};

enum Spine_TAG{
	Login_Spine = 1,
	SizeScale_Spine,
	SpeedScale_Spine,
	ActionIndex_Spine,
	ModelID_Spine,
	Load_Spine,
	Release_Spine,
	Next_Spine,
	Previous_Spine,
	ErrorLab_Spine,
	ActionCountLab_Spine,
	ActionNameLab_Spine,
	Create_Spine,
	Remove_Spine,
	ShowLoad_Spine,
	Repeat_Spine,

	Input_SizeScale = 50,
	Input_SpeedScale,
	Input_ActionIndex,
	Input_ModelID,

	get_SizeScale = 100,
	get_SpeedScale,
	get_ActionIndex,
	get_ModelID,

	Spine_Animation = 200
};


#endif	//__LOGO_SCENE_H__