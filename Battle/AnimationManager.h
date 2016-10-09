#ifndef _ANIMATION_MANAGER_H_
#define _ANIMATION_MANAGER_H_
#include "cocos2d.h"
#include <string>
#include <map>
/************************************************************* 
 *
 *
 *		Data : 2016.9.2
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 动画管理器
 *
 *
 *************************************************************/
using namespace std;
using namespace cocos2d;
namespace BattleSpace
{
	enum E_ModelType
	{
		eEffectSkill,				//特效
		eEffectPNG,					//png特效
		eEffectJPG,					//Jpg特效
		eFrameRole					//角色
	};
	class ActionModel;
	class SpecialEffectData;
	class AnimationManager : public CCObject
	{
	private:
		AnimationManager();
		static AnimationManager* mAnimationManage;
	public:
		virtual ~AnimationManager();
		static AnimationManager* sharedAction();
		CCAnimation* getAnimation(const char* name,const char* action=nullptr);			//根据名称(人物则加动作名)动画
		void parseRoleAction(const char* name);											//解析角色动画
		void ParseAnimation(const char* name,int type = eEffectSkill);						//解析动画(统一接口)
		ActionModel* getAction(const char* name);		
		void clearAnimationManage();
		void ReleaseAnimation(const char* name);										//释放资源的时候移除资源使用
		BattleSpace::SpecialEffectData* getSpcialEffectData(){return mSpecialdata;}
	protected:
		void addAction(ActionModel* action);
		map<string,ActionModel*> mMapAction;
	private:
		SpecialEffectData* mSpecialdata;
		ActionModel* mDefault;
	};
	CCAnimation* createAnimationFromSpriteFrameName(const char* Filepath);

	CCSprite * createAnimationSprite(const char* strName,CCPoint pos,CCAnimation *animation,bool isLoop);
};
#endif // !_ANIMATION_MANAGER_H_
