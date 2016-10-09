/************************************************************* 
*
*
*		Data : 2016.9.2
*	
*		Name : ActionModel
*
*		Author : Lin_Xiancheng
*
*		Description : 
*
*
*************************************************************/

#ifndef __ActionModel_H_
#define __ActionModel_H_
#include "cocos2d.h"
using namespace cocos2d;
using namespace std;
namespace BattleSpace
{
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
};
#endif