#ifndef _ANIMATION_MANAGER_H_
#define _ANIMATION_MANAGER_H_
#include "cocos2d.h"
#include<string>
#include <map>
/************************************************************************/
/*                      
动画管理器的原理：ActionModel这个类用于存储信息，存储包括人物动作的动画，和特效的动画两类。
而AnimationManager主要负责解析加载的文件，并把加载的信息存储到ActionModel这个对象当中。
再统一管理一个存储动画对象的map。 //  [11/3/2014 xc_lin]
*/
/************************************************************************/
namespace BattleSpace{
	using namespace std;
	USING_NS_CC;
	enum E_ModelType
	{
		eEffectSkill,				//特效
		eEffectPNG,					//png特效
		eEffectJPG,					//Jpg特效
		eFrameRole					//角色
	};

	//在CCAnimation动画类之上封装动画类
	class ActionModel : public CCObject
	{
	public:
		ActionModel();
		virtual ~ActionModel();
		inline bool init(){return true;}
		CREATE_FUNC(ActionModel);
	public:
		void setActionFrames(const char* action,int Frames){m_ActionFrames[action] = Frames;}			//设置动作帧数
		void setActionSpeed(const char* action,float speed){m_ActionSpeed[action] = speed;}				//设置动作帧率
		int getActionFrames(const char* action);														//获取动作帧数
		float getActionSpeed(const char* action);														//获取动作帧率
		float getActionTime(const char* action){return getActionFrames(action)*getActionSpeed(action);}	//动作时间

		CC_SYNTHESIZE(int,m_Frames,EffectFrames);												//特效帧数
		CC_SYNTHESIZE(float,m_Speed,EffectSpeed);												//特效帧率
		inline float getEffectTime(){return m_Speed* m_Frames;};								//获取 特效总时间
		CC_SYNTHESIZE(string,m_name,Name);														//将文件名设置为动画名
		CC_SYNTHESIZE(char,m_type,Type);														//设置动画类型
		CCRect getRect();																		//将第一帧的大小作为整个动画的大小返回
		CCAnimation* getAnimation(const char* action = nullptr);								//返回特效动画
	protected:					
		map<string,int> m_ActionFrames;													//动作-对应帧数
		map<string,float> m_ActionSpeed;												//动作-对应帧率
	};
	/**************************************/
	/***			动画管理器			***/
	/**************************************/
	class SpecialEffectData;
	class AnimationManager : public CCObject
	{
	public:
		AnimationManager();
		virtual ~AnimationManager();
		static AnimationManager* sharedAction();
		CCAnimation* getAnimation(const char* name,const char* action=nullptr);			//根据名称(人物则加动作名)动画
		void parseRoleAction(const char* name);											//解析角色动画
		void ParseAnimation(const char* name,int type = eEffectSkill);						//解析动画(统一接口)
		ActionModel* getAction(const char* name);		
		CCRect getAliveRect(const char* name);											//根据动画名称得到动画区域大小
		void clearAnimationManage();
		void ReleaseAnimation(const char* name);										//释放资源的时候移除资源使用
		BattleSpace::SpecialEffectData* getSpcialEffectData(){return m_spefdata;}
	protected:
		void addAction(ActionModel* action);
	public:
		map<string,ActionModel*> m_Actions;
	private:
		BattleSpace::SpecialEffectData* m_spefdata;
	};
	CCAnimation* createAnimationFromSpriteFrameName(const char* Filepath);

	CCSprite * createAnimationSprite(const char* strName,CCPoint pos,CCAnimation *animation,bool isLoop);
};
#endif // !_ANIMATION_MANAGER_H_
