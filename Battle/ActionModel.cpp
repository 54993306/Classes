/************************************************************* 
 *
 *
 *		Data : 2016.9.2
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/
#include "Battle/ActionModel.h"
#include "Battle/AnimationManager.h"
#define MAX_EFFECT_FRAME 40
#define MAX_ACTION_FRAME 20
namespace BattleSpace
{
	ActionModel::ActionModel():m_name(""),m_Frames(0),m_type(0),m_Speed(0){}
	ActionModel::~ActionModel()
	{
		m_ActionFrames.clear();
		m_ActionSpeed.clear();
	}
	//得到 actionKey 帧数
	int ActionModel::getActionFrames(const char* actionKey)
	{
		map<string,int>::iterator iter = m_ActionFrames.find(actionKey);
		if(iter != m_ActionFrames.end())
			return iter->second;
		return 0;
	}
	//得到动作帧率
	float ActionModel::getActionSpeed(const char* actionKey)
	{
		map<string,float>::iterator iter = m_ActionSpeed.find(actionKey);
		if(iter != m_ActionSpeed.end())
			return iter->second;
		return 0;
	}
	//返回特效动画
	CCAnimation* ActionModel::getAnimation(const char* action /*=nullptr*/)
	{
		CCArray* animFrames = CCArray::create();
		char str[100] = {0};
		for(int i = 0; i < MAX_EFFECT_FRAME; ++i)
		{
			switch (m_type)
			{
			case eEffectSkill:
				{
					int index = 10000 + i;
					sprintf(str,"%s_%d.png",m_name.c_str(),index);
				}break;
			case eEffectPNG:
				{
					sprintf(str,"%s_%d.png",m_name.c_str(),i);
				}break;
			case eEffectJPG:
				{
					sprintf(str,"%s_%d.jpg",m_name.c_str(),i);
				}break;
			case eFrameRole:
				{
					CCArray* animFrames = CCArray::create();
					CCSpriteFrameCache *cache = CCSpriteFrameCache::sharedSpriteFrameCache(); 
					char str[100] = {};
					string strFormat = m_name + "_" + string(action)+"_%d.png";
					for(int i = 0; i < MAX_ACTION_FRAME; ++i)
					{
						sprintf(str,strFormat.c_str(),i);
						CCSpriteFrame* frame = cache->spriteFrameByName(str);
						if( frame ) animFrames->addObject(frame);
					}
					CCAnimation* animation = CCAnimation::createWithSpriteFrames(animFrames,getActionSpeed(action));
					return animation;
				}break;
			}
			CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(str);
			if( frame )
				animFrames->addObject(frame);
			else
				break;
		}
		CCAnimation* animation = CCAnimation::createWithSpriteFrames(animFrames,getEffectSpeed());
		return animation;
	}
	//可以配置动作或特效以第几帧作为当前特效的碰撞检测帧(或在动画运行时实时获取当前帧的范围进行判断)
	CCRect ActionModel::getRect()
	{
		CCRect rect;
		if(m_name == "") return rect;
		string frameName = m_name + "_" + string("standby") + "_0.png";
		CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName.c_str());
		if(frame)
			return frame->getRect();
		frameName = m_name + "_0.png";
		frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName.c_str());
		if(frame)
			return frame->getRect();
		return rect;
	}
}