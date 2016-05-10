#ifndef _LOAD_SCENE_H_
#define _LOAD_SCENE_H_
#include "AppUI.h"
/************************************************************************/
//       
//								加载公共资源
//
/************************************************************************/
class EffectObject;
enum LoadResType
{
	LOAD_RES_MODEL = 1,
	LOAD_BACKGROUND_SND = 2,
	LOAD_EFFECT_SND = 3,
	LOAD_FONT = 4
};
enum struct LoadType
{
	LoadNuLL,
	Load_FrameAnimation,
	Load_Effect,
	Load_CocosBone,
	Load_Spine,
};
struct LoadResourceInfo
{
	string FileName;
	string FilePath;
	LoadType Loadtype;
	LoadResourceInfo():Loadtype(LoadType::LoadNuLL),FileName(""),FilePath(""){}
};

class LoadScene : public CScene
{
public:
	LoadScene();
	DEFINE_SCENE_CLASS(LoadScene);
	virtual void onCreate();
	virtual void onEnter();
	virtual void onExit();
	void load(float fdetal);
	void load2(float fdetal);
protected:
	int m_currNum;
	int m_totalNum;
	CCLabelTTF* m_tip;
	int m_currTip;
	int m_currTipNum;
	float m_time;
	void loadAsyncImg(CCObject* pSender);
private:
	void updatePercent(float dt);
private:
	CLayout* m_ui;
	CCSprite* m_pZombieSprite;
	int m_asynLoadNum;
	vector<LoadResourceInfo> m_resVec;
	CProgressBar* m_progress;
	float m_fPercent;
	int m_iPlistIndex;
};
#endif // !_LOAD_SCENE_H_
