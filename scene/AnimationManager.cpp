#include "AnimationManager.h"
#include "alive/AliveDefine.h"
#include "tools/ToolDefine.h"
#include "model/DataCenter.h"
#include "model/WarManager.h"
#include "warscene/EffectData.h"
#define MAX_EFFECT_FRAME 40
#define MAX_ACTION_FRAME 20

ActionModel::ActionModel()
	:m_name(""),m_Frames(0),m_type(0),m_Speed(0){}
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
		default:
			break;
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
	string frameName = m_name + "_" + string(Stand_Action) + "_0.png";
	CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName.c_str());
	if(frame)
		return frame->getRect();
	frameName = m_name + "_0.png";
	frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName.c_str());
	if(frame)
		return frame->getRect();
	return rect;
}
/************************************************************************/
//                                                                      
//
//							动画管理器对象
//
//
/************************************************************************/
AnimationManager::~AnimationManager() 
{ 
	clearAnimationManage(); 
	CC_SAFE_RELEASE(m_spefdata);
	m_spefdata = nullptr;
}

void AnimationManager::clearAnimationManage()
{
	for(map<string,ActionModel*>::iterator iter = m_Actions.begin();iter != m_Actions.end();++iter)
		iter->second->release();
	m_Actions.clear();
}

AnimationManager::AnimationManager():m_spefdata(nullptr)
{
	m_spefdata = SpecialEffectData::create();
	m_spefdata->retain();
}

AnimationManager* AnimationManager::sharedAction()
{
	static AnimationManager* m_actionManager = nullptr;
	if( m_actionManager == nullptr )
		m_actionManager = new AnimationManager();
	return m_actionManager;
}
//根据特效名称得到特效动画
CCAnimation* AnimationManager::getAnimation(const char* name,const char* action/*=nullptr*/)
{
	ActionModel* actionmodel = getAction(name);
	if(actionmodel)	return actionmodel->getAnimation(action);
	return nullptr;
}
//根据动画名称得到动画区域大小
CCRect AnimationManager::getAliveRect(const char* name)
{
	CCRect rect;
	ActionModel* action = getAction(name);
	if(action)	rect = action->getRect();
	return rect;
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
	if (getAction(name))
		return;
	ActionModel* action = ActionModel::create();
	char str[60] = {0};
	float speed = 0.1f;								//默认帧率
	if (m_spefdata->JudgeSpcialEffect(atoi(name)))
	{
		SpecialEffectInfo* spefdata = m_spefdata->getspEff(atoi(name));
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
	action->setEffectSpeed(speed);
	action->setEffectFrames(totalFrame);
	action->setType(type);
	action->setName(name);
	addAction(action);											//添加到管理器当中
}
//判断容器中是否存在同名动画，如果不存在则加入容器当中
void AnimationManager::addAction(ActionModel* action)
{
	ActionModel* tmpAction = getAction(action->getName().c_str());
	if(!tmpAction)
	{
		action->retain();
		m_Actions[action->getName()] = action;
	}else{
		CCLOG("[ *TIPS ] AnimationManager::addAction Repead ActionMode name=%s",action->getName().c_str());
	}
}
ActionModel* AnimationManager::getAction(const char* name)
{
	map<string,ActionModel*>::iterator iter = m_Actions.find(name);
	if(iter != m_Actions.end())
		return iter->second;
	return nullptr;
}

void AnimationManager::ReleaseAnimation(const char* name)
{
	map<string,ActionModel*>::iterator iter = m_Actions.find(name);
	if(iter != m_Actions.end())
	{
		CC_SAFE_RELEASE(iter->second);
		m_Actions.erase(iter);
	}
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
