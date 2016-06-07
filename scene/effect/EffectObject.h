#ifndef _EFFECT_OBJECT_H_
#define _EFFECT_OBJECT_H_
#include "cocos2d.h"
#include <string>
/************************************************************************/
/*              
					特效对象
*/
/************************************************************************/
using namespace std;
USING_NS_CC;

namespace BattleSpace{
	enum struct PLAYERTYPE
	{
		once,
		Repeat,
		RepeatNum,
		Delay,
		Duration,
	};

	class EffectObject:public CCNode
	{
	public:
		EffectObject();
		virtual ~EffectObject();
		static EffectObject* create(const char* model,PLAYERTYPE type = PLAYERTYPE::once);
		virtual void onEnter();
		virtual bool init();
		void play();
		bool isActionDone();
		void setEffAnchorPoint(float x, float y);
		void update(float dt);
		void playeEffectMusic(int musicId);
		void initEffect();
		void ShakeMessage();
		void PlayerMusic();
		CC_SYNTHESIZE(string,m_model,Model);					//特效名称
		CC_SYNTHESIZE(bool,m_skewing,Skewing);					//是否启用配置数据
		CC_SYNTHESIZE(float,m_TotalTime,TotalTime);				//播放总时间
		CC_SYNTHESIZE(float,m_DurationTime,DurationTime);		//播放总时间
		CC_SYNTHESIZE(float,m_Playtime,Playtime);				//已播放时间
		CC_SYNTHESIZE(float,m_Delaytime,Delaytime);				//延迟播放时间
		CC_SYNTHESIZE(PLAYERTYPE,m_Type,PlayerType);			//特效类型
		CC_SYNTHESIZE(int,m_Music,Music);						//特效类型
		CC_SYNTHESIZE(int,m_LoopNum,LoopNum);					//循环次数
		CC_SYNTHESIZE(float,m_LoopInterval,LoopInterval);		//循环间隔
		CC_SYNTHESIZE(bool,m_Shake,Shake);						//是否震动
		void setShaderEffect(CCGLProgram* pGl);
	protected:
		CCSprite* m_effect;
		bool m_bIsNullEffect;
	};
};

#endif // !_EFFECT_OBJECT_H_