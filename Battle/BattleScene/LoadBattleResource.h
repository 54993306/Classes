#ifndef _LOADWAR_SCENE_H_
#define _LOADWAR_SCENE_H_
#include "AppUI.h"
/************************************************************************/
/*                           
						预加载战斗资源//  [10/30/2014 xc_lin]
*/
/************************************************************************/
namespace BattleSpace
{
	enum WARLoadResType
	{
		LOAD_FrameAnimation = 1,		//模型
		LOAD_BackMusic = 2,				//背景音乐
		LOAD_EffectMusic = 3,			//特效音乐
		LOAD_Font = 4,					//字体
		LOAD_CocosBone = 5,				//cocos骨骼动画
	};
	//资源加载的速度是很快的，如果使用每一帧加载一个资源的情况，那么看起来进度条就会卡顿很多。
	enum loadTime
	{
		LOAD_PUBLIC_EFFECT			= 5,		
		LOAD_ROLE					= 8,		
		total_time					= 35,		
		public_time					= 30,		//预计加载公共资源的帧数
	};
	enum class ResourceType
	{
		Load_Role = 1,
		Load_Spine,
		Load_Effect,
		Load_Terrain,
		Load_Buff,
	};
	enum struct sLoadType
	{
		eNuLL,
		eFrameAnimation,
		eEffect,
		eCocosBone,
		eSpine,
	};
	enum SceneType
	{
		SkipcityScene =	0,		//主城
		SkipLoginScene,			//登陆界面
		skipSelectHero,
	};

	enum LoadResType
	{
		LOAD_RES_MODEL			= 1,
		LOAD_BACKGROUND_SND		= 2,
		LOAD_EFFECT_SND			= 3,
		LOAD_FONT				= 4
	};

	struct LoadResourceInfo
	{
		string FileName;
		string FilePath;
		sLoadType Loadtype;
		LoadResourceInfo():Loadtype(sLoadType::eNuLL),FileName(""),FilePath(""){}
	};
	class BaseRoleData;
	class LoadBattleResource : public CScene
	{
	public:
		LoadBattleResource();
		DEFINE_SCENE_CLASS(LoadBattleResource);
		virtual void onCreate();
		virtual void onEnter();
		virtual void onExit();
		virtual void onEnterTransitionDidFinish();
		void updateTips(float fdetal);												//刷新提醒
		void DataParse();															//总的加载数据
		void TrapParse();
		void ResourceDispose(float delta);											//资源加载方法
		void setRelease(bool isrelease,int sceneType);								//设置是否释放资源并跳转到哪个场景
		void CocosBoneThread(int ModeID);											//角色加载处理

		void LoadPublic();															//加载公共资源
		void LoadEffect();															//加载技能特效
		void BackImage();															//背景图片处理
		void TextureThreadCallBack(CCObject* pSender);								//图片异步加载回调

		void CocosBoneCallBack(float dt);
		void TextureThread(const char* url, const char* model, sLoadType type = sLoadType::eEffect);		//异步加载处理
		void SkillParse(const BaseRoleData* pRole,vector<int>&VecEffect,vector<int>&VecBuff);
		void LoadBeingAnimation();
		void ProgressEnd();
		void LoadCocosEffect();
		void updateProgress();
		void releaseResource();
	protected:
		CProgressBar*						m_progress;
		CCSprite*							m_pZombieSprite;					//小僵尸
		map<ResourceType,vector<int>>		m_WarResouse;	
		int									m_CurrIndex;						//当前已加载的行数
		int									m_publicNum;						//公共资源
		int									m_totalNum;							//得到总行数
		CCLabelTTF*							m_tip;
	private:
		bool								m_Release;							//当前状态
		int									m_SceneType;						//跳转到哪一个场景
		vector<LoadResourceInfo>			m_resVec;
		int									m_loadResNum;
		CLayout*							m_Layer;
	};
};
#endif // !_LOADWAR_SCENE_H_