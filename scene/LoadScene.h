#ifndef _LOAD_SCENE_H_
#define _LOAD_SCENE_H_
#include "AppUI.h"
#include "Battle/BattleScene/LoadBattleResource.h"
/************************************************************************/
//       
//								加载公共资源
//
/************************************************************************/
namespace BattleSpace
{
	class LoadScene : public CScene
	{
	public:
		LoadScene();
		DEFINE_SCENE_CLASS(LoadScene);
		virtual void onCreate();
		virtual void onEnter();
		virtual void onExit();
		void loadResource(float fdetal);
	protected:
		int mCurrIndex;
		void loadAsyncImg(CCObject* pSender);
	private:
		void loadResouceEnd();
	private:
		CLayout* m_ui;
		CCSprite* m_pZombieSprite;
		int mAsynLoadNum;
		vector<LoadResourceInfo> mResourceVec;
		CProgressBar* mProgress;
	};
}
#endif // !_LOAD_SCENE_H_
