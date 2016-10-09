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

#include "AnimationManager.h"
#include "tools/ToolDefine.h"
#include "Battle/EffectData.h"
#include "Battle/ActionNameDefine.h"
#include "Battle/ActionModel.h"
#define MAX_EFFECT_FRAME 40
#define MAX_ACTION_FRAME 20
namespace BattleSpace
{
	AnimationManager* AnimationManager::mAnimationManage = nullptr;

	AnimationManager::AnimationManager():mSpecialdata(nullptr)
	{
		mSpecialdata = SpecialEffectData::create();
		mSpecialdata->retain();
		mDefault = ActionModel::create();
		mDefault->retain();
	}

	AnimationManager* AnimationManager::sharedAction()
	{
		if( mAnimationManage == nullptr )
			mAnimationManage = new AnimationManager();
		return mAnimationManage;
	}

	AnimationManager::~AnimationManager() 
	{ 
		clearAnimationManage(); 
		CC_SAFE_RELEASE(mSpecialdata);
		mSpecialdata = nullptr;
	}

	void AnimationManager::clearAnimationManage()
	{
		for (auto tPair : mMapAction)
		{
			CC_SAFE_RELEASE(tPair.second);
		}
		mMapAction.clear();
	}

	void AnimationManager::ReleaseAnimation(const char* name)
	{
		map<string,ActionModel*>::iterator iter = mMapAction.find(name);
		if(iter != mMapAction.end())
		{
			CC_SAFE_RELEASE(iter->second);
			mMapAction.erase(iter);
		}
	}
	//判断容器中是否存在同名动画，如果不存在则加入容器当中
	void AnimationManager::addAction(ActionModel* action)
	{
		ActionModel* tmpAction = getAction(action->getName().c_str());
		if(strcmp(tmpAction->getName().c_str(),""))
		{
			CCLOG("[ *TIPS ] AnimationManager::addAction Repead ActionMode name=%s",action->getName().c_str());
			return;
		}
		action->retain();
		mMapAction[action->getName()] = action;
	}
	ActionModel* AnimationManager::getAction(const char* name)
	{
		map<string,ActionModel*>::iterator iter = mMapAction.find(name);
		if(iter != mMapAction.end())
			return iter->second;
		return mDefault;
	}
	//根据特效名称得到特效动画
	CCAnimation* AnimationManager::getAnimation(const char* name,const char* action/*=nullptr*/)
	{
		ActionModel* tAction = getAction(name);
		if (!strcmp(tAction->getName().c_str(),""))
			CCLOG("[ *ERROR ] AnimationManager::getAnimation Lost %s",name);
		return tAction->getAnimation(action);
	}							
	//武将动作解析 enemy1_attack_1.png
	void AnimationManager::parseRoleAction(const char* name)
	{
		ActionModel* action = ActionModel::create();
		const char* actionKey[] = {Walk_Action,Attack_Action,Hit_Action,Stand_Action,Skill_Action};	//添加一种动作j的值要相应加1，j对应当前数组的下标
		float speed[] = {0.2f,0.1f,0.2f,0.2f,0.2f};													//动作对应的帧率(帧动画应做成可由策划配置)
		for(int j = 0; j < sizeof(actionKey)/sizeof(char);j++)										//5( sizeof(actionKey)/sizeof(char) )个动作
		{
			string strFormat = string(name) + "_" + string(actionKey[j])+"_%d.png";
			int totalFrames = 0;
			char str[100] = {0};
			for(int i = 0; i < MAX_ACTION_FRAME; ++i)			
			{
				sprintf(str,strFormat.c_str(),i);
				CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(str);
				if( !frame )break;
				++totalFrames;										//每个动作帧数
			}
			action->setActionSpeed(actionKey[j],speed[j]);			//每个动作每一帧的时间
			action->setActionFrames(actionKey[j],totalFrames);		//每个动作对应的帧数
		}
		action->setName(name);
		action->setType(eFrameRole);
		addAction(action);											//添加到管理器当中
	}
	//特效解析
	void AnimationManager::ParseAnimation(const char* name,int type /*=EF_Skill*/)
	{
		ActionModel* tAction = ActionModel::create();
		char str[60] = {0};
		float speed = 0.1f;								//默认帧率
		if (mSpecialdata->JudgeSpcialEffect(atoi(name)))
		{
			SpecialEffectInfo* spefdata = mSpecialdata->getSpecialEffect(atoi(name));
			if (spefdata)
			{
				speed = spefdata->getspeed();									
			}else{
				CCLOG("[ *TIPS ] AnimationManager::ParseAnimation Can't Find Effect Info name = %s",name);			//没有写入specialEffect则无法获取(名称需为数字类)
			}
		}
		int totalFrame = 0;
		for(int i = 0; i < MAX_EFFECT_FRAME; ++i)
		{
			switch (type)
			{
			case eEffectSkill:
				{
					int index = 10000 + i;					//所有特效的命名都是从1万开始的
					sprintf(str,"%s_%d.png",name,index);
				}break;
			case eEffectPNG:	{		sprintf(str,"%s_%d.png",name,i+1);    }  break;		//图片从1开始
			case eEffectJPG:	{		sprintf(str,"%s_%d.jpg",name,i+1);    }  break;
			case eFrameRole:
				{
					parseRoleAction(name);
					return;
				}
				break;
			default:
				break;
			}
			CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(str);
			if( !frame )break;
			++totalFrame;
		}
		tAction->setEffectSpeed(speed);
		tAction->setEffectFrames(totalFrame);
		tAction->setType(type);
		tAction->setName(name);
		addAction(tAction);											//添加到管理器当中
	}
	//根据文件路径创建一个动画返回
	CCAnimation* createAnimationFromSpriteFrameName(const char* Filepath)
	{
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(Filepath);
		CCSpriteFrame* frame = NULL;
		CCArray* frameArray = CCArray::create();

		string str = Filepath;
		int pi = str.rfind(".plist");		//字符串查找 返回 第一个字符的索引
		str = str.substr(0,pi);			//取得从0
		int index = str.rfind("/");
		string subFile = str.substr(index+1);

		for (int i = 0; i < 50; i++)
		{
			frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(CCString::createWithFormat("%s_%d.png", subFile.c_str(), 10000+i)->getCString());
			if (!frame)break;
			frameArray->addObject(frame);
		}
		return CCAnimation::createWithSpriteFrames(frameArray);  
	}

	CCSprite * createAnimationSprite(const char* strName,CCPoint pos,CCAnimation *animation,bool isLoop)
	{
		if (!animation)
			return nullptr;
		CCSprite *spr = CCSprite::create(strName);
		spr->setPosition(pos);
		if (isLoop)
		{
			spr->runAction(CCRepeatForever::create(CCAnimate::create(animation)));
		}else{
			spr->runAction(CCAnimate::create(animation));
		}
		return spr;
	}
};