#include "AliveObject.h"
#include "scene/AnimationManager.h"
#include "common/CommonFunction.h"
#include "GMessage.h"
#include "warscene/ConstNum.h"
#include "ActObject.h"
#include "model/DataCenter.h"
#include "tools/StringUtil.h"
#include "scene/alive/HPObject.h"
#include "scene/alive/RageObject.h"
#include "scene/effect/EffectObject.h"
#include "Global.h"
#include "model/DataDefine.h"
#include "scene/layer/WarAliveLayer.h"
#include "model/WarManager.h"
#include "model/BattleData.h"
#include "common/CGameSound.h"
#include "common/ShaderDataHelper.h"
#include "warscene/CHeroSoundData.h"
#include "model/BuffManage.h"

#include "Battle/BattleMessage.h"
AliveObject::AliveObject()
	:m_Body(nullptr),m_Hp(nullptr),m_Name(""),m_ActionKey(""),m_MoveState(0)
	,m_NameLabel(nullptr),m_Armature(nullptr),m_DropItem(0),m_offs(CCPointZero),m_Speed(CCPointZero)
	,m_Enemy(false),m_EffectMusic(0),m_Front(false),m_DieState(false),m_IsSpine(false)
	,m_Direction(0),m_ID(0),m_Model(0),m_Alive(nullptr),m_MoveObj(nullptr)
	,m_AtkEffect(0),m_PlayerEffect(0),m_UpdateState(true),m_Skeleton(nullptr)
{}
AliveObject::~AliveObject()
{
	if(m_Hp) removeChild(m_Hp);
	if(m_NameLabel) removeChild(m_NameLabel);
	if(m_Body) removeChild(m_Body);
	CC_SAFE_RELEASE(m_Hp);
	CC_SAFE_RELEASE(m_NameLabel);
	CC_SAFE_RELEASE(m_Body);
	m_Hp = nullptr;
	m_NameLabel = nullptr;
	m_Body = nullptr;
}

bool AliveObject::init()
{
	setBody(nullptr);
	if (m_Body)return true;
	return false;
}

void AliveObject::setBody(CCSprite* body)
{
	m_Body = CCSprite::create();
	if(!m_Body)
	{
		CCLOG("[ *ERROR ] AliveObject::setBody");
		return;
	}
	m_Body->retain();
	this->addChild(m_Body);
}
CCSprite* AliveObject::getBody() { return m_Body; }

void AliveObject::setHp(HPObject* hp)
{
	CCLabelTTF* AliveID = CCLabelTTF::create(ToString(C_ALIVE_OB+m_Alive->getAliveID()),"arial",20);
	AliveID->setColor(ccc3(255,0,0));
	AliveID->setPosition(ccp(0,60));
	CCLabelTTF* AliveMode = CCLabelTTF::create(ToString(m_Model),"arial",20);
	AliveMode->setColor(ccc3(255,0,0));
	AliveMode->setPosition(ccp(0,30));
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	this->addChild(AliveMode);
	this->addChild(AliveID);
#endif

	if( m_Hp == nullptr )
	{
		m_Hp = HPObject::create();
		if( m_Hp == nullptr )
			return;
		m_Hp->retain();
	}
	if (m_Enemy)
	{
		m_Hp->setSkin(PUBLIC_ROOT("black.png"),"warScene/bar_yellow.png",m_Enemy);	
	}else{
		m_Hp->setSkin(PUBLIC_ROOT("black.png"),"warScene/bar_green.png",m_Enemy);
	}
	m_Hp->setMaxHp(m_Alive->getMaxHp());
	m_Hp->setHp(m_Alive->getHp());
	m_Hp->setScale(1/m_Alive->getZoom());
	m_Hp->setVisible(false);
	m_Hp->setPosition(ccp(0,-5));
	m_Armature->addChild(m_Hp, 100);
	initTypeIcon();
}
HPObject* AliveObject::getHp() { return m_Hp; }

void AliveObject::initTypeIcon()
{
	char str[60]={0};
	const HeroInfoData *c_data = DataCenter::sharedData()->getHeroInfo()->getCfg(m_Alive->role->thumb);
	if(c_data)
	{
		sprintf(str,"common/type_%d_%d.png", m_Alive->role->roletype, c_data->iType2);
	}else{
		sprintf(str,"common/type_1_1.png");
	}
	CCSprite* sp = CCSprite::create(str);
	if (!sp)
	{
		sp = CCSprite::create("common/type_1_1.png");
		CCLOG("AliveObject::setHp ERROR");
	}
	sp->setScale(0.5f);
	sp->setPosition(ccp(-m_Hp->getContentSize().width/2,0));			//在body处设置了翻转了
	m_Hp->addChild(sp);
}

void AliveObject::setNickName(string nickName)//设置武将名字
{
	if( nickName.c_str() == nullptr || m_Name == nickName ) return;
	m_Name = nickName;
	if(m_NameLabel == nullptr)
	{
		m_NameLabel = CCLabelTTF::create();
		if(m_NameLabel == nullptr) return;
		m_NameLabel->retain();
		m_NameLabel->setFontSize(20);
		m_NameLabel->setFontName("arial");
		m_Body->addChild( m_NameLabel );
	}
	m_NameLabel->setString(m_Name.c_str());
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	m_NameLabel->setVisible(true);
#else
	m_NameLabel->setVisible(false);
#endif
}
string AliveObject::getNickName() { return m_Name; }//设置名字颜色

void AliveObject::setDirection(int direction)//设置人物方向
{
	if( m_Direction == direction || !m_Armature)
		return;
	m_Direction = direction;
	if (m_IsSpine)
	{
		if (DataCenter::sharedData()->getRoleData()->isTurn(m_Model))
		{
			if(m_Direction == Ditection_Left)
			{
				if (m_Skeleton)
					m_Skeleton->setScaleX(1);//spine特效
				m_Armature->setScaleX(1);//spine人物
			}else{
				if (m_Skeleton)
					m_Skeleton->setScaleX(-1);//翻转
				m_Armature->setScaleX(-1);//spine人物
			}
		}else{
			if(m_Direction == Ditection_Left)
			{
				if (m_Skeleton)
					m_Skeleton->setScaleX(-1);//翻转
				m_Armature->setScaleX(-1);//spine人物
			}else{
				if (m_Skeleton)
					m_Skeleton->setScaleX(1);//翻转
				m_Armature->setScaleX(1);//spine人物
				
			}
		}
	}else{
		if(m_Direction == Ditection_Left)
			m_Armature->setScaleX(-1);//翻转
		else
			m_Armature->setScaleX(1);//翻转
	}
}
int AliveObject::getDirection() { return m_Direction; }
//这个方法不属于数据类，也不属于显示类，但是应该在武将自身的身上，每个武将的死亡处理都不一样。
void AliveObject::AliveDie()
{
	//判断武将当前状态是否可以死亡,(攻击目标的连击是否结束)
	if (!m_Alive->getActObject())
		return;
	for (auto alive:m_Alive->HittingAlive)
	{
		if (alive == m_Alive)
			continue;
		if (alive->getHp()<=0 && alive->getActObject() && !alive->getActObject()->getDieState())
		{
			alive->getActObject()->setDieState(true);
			alive->getActObject()->AliveDie();
		}
	}
	NOTIFICATION->postNotification(Msg_DropItem,this);
	m_Alive->setHp(0);
	m_Hp->setHp(0);
	if (m_Alive->getMoveObj())
		m_Alive->getMoveObj()->removeFromParentAndCleanup(true);
	m_Alive->setMoveObj(nullptr);
	m_MoveObj = nullptr;
	m_Alive->setDieState(true);
	m_Alive->getBuffManage()->Buffclear();
	//m_Alive->setActObject(nullptr);
	if (!m_DieState)
	{
		m_DieState = true;
		NOTIFICATION->postNotification(ALIVEDIE,m_Alive);
	}
	if (m_Alive->getEnemy())
		this->setUserObject(CCBool::create(true));						//死亡流程不被技能暂停处理
	m_Alive->getActObject()->TurnStateTo(Die_Index);
}
//做受击判断和一个冲锋0号格子特殊处理,攻击音效攻击特效播放,使用了受击数组但是未操作
void AliveObject::AtkBegin_Event()
{
	if(m_AtkEffect)
	{
		EffectObject* _Effect = EffectObject::create(ToString(m_AtkEffect));
		this->addChild(_Effect);						//播在人物身上，不会发生偏移，跟着人物走动的
		_Effect->setShaderEffect(m_Armature->getShaderProgram());
	}
	//NOTIFICATION->postNotification(Draw_SKILL_Area,m_Alive);//绘制攻击范围
}
//操作受击数组,播完掉血则移除当前回合受击数组
void AliveObject::HpChange_Event()
{
	NOTIFICATION->postNotification(ALIVEATTACK,m_Alive);
	if (m_PlayerEffect)
		NOTIFICATION->postNotification(B_SkilEffectInMap,this);
}
//攻击区域必杀技状态才绘制,攻击完毕可以将武将传出去,进行下一个效果的判断或是重置武将信息等
void AliveObject::AtkEnd_Event()
{
	if (!m_UpdateState)return;
	NOTIFICATION->postNotification(B_CancelDrawAttackArea,m_Alive);//取消绘制攻击范围(针对性的取消绘制)
	if (m_Alive->AtkAlive.size())
	{
		if (DataCenter::sharedData()->getWar()->NextEffect(m_Alive))
		{
			m_Alive->ExcuteNextEffect();
		}else{
			if (m_Alive->getCriAtk()||m_Alive->getSpeAtk())
			{
				m_Alive->ResetAttackState();	//逻辑处理完毕重置武将状态信息
			}else{
				m_Alive->ResetAttackState();	//逻辑处理完毕重置武将状态信息
				m_Alive->setAtkNum(1);			//攻击次数累加
			}
		}	
	}else{
		if (m_Alive->getCriAtk()||m_Alive->getSpeAtk())
		{
			m_Alive->ResetAttackState();	//逻辑处理完毕重置武将状态信息
		}else{
			m_Alive->ResetAttackState();	//逻辑处理完毕重置武将状态信息
			m_Alive->setAtkNum(1);								//攻击次数累加
		}
	}																					
}
//分解sData//0.动画名称1.scalex2.scaley3.offx4.offy5.offZorder
void AliveObject::PlayAnimat_Event(string sData)
{
	CCArray* pArr = strSplit(sData.c_str(), "_");
	string sAnimatName = ((CCString*)pArr->objectAtIndex(0))->m_sString;
	float fScaleX = atof(((CCString*)pArr->objectAtIndex(1))->getCString());
	float fScaleY= atof(((CCString*)pArr->objectAtIndex(2))->getCString());
	int iOffX = atoi(((CCString*)pArr->objectAtIndex(3))->getCString());
	int iOffY = atoi(((CCString*)pArr->objectAtIndex(4))->getCString());
	int iOffZorder = atoi(((CCString*)pArr->objectAtIndex(5))->getCString());

	//获取动画
	EffectObject* pAnimat = EffectObject::create(sAnimatName.c_str());
	pAnimat->setScaleX(fScaleX);
	pAnimat->setScaleX(fScaleY);
	pAnimat->setPosition(m_Armature->getPosition() + ccp(iOffX, iOffY));
	pAnimat->setZOrder(m_Armature->getZOrder()+iOffZorder);
	m_Armature->getParent()->addChild(pAnimat);
	pAnimat->setShaderEffect(m_Armature->getShaderProgram());
}

void AliveObject::playerNum( int num,int type )
{
	if (type == nullType)return;
	if (!num&&type!=typeface)
	{
		CCLOG(" [ Tips ] AliveObject::playerNum Num = 0!");
		return	;
	}
	m_Hp->playerNum(this,num,type);
	if (m_Alive->getAliveType() == AliveType::WorldBoss)														//boss的情况处理应该在血量条的内部,自己进行。
		NOTIFICATION->postNotification(B_WorldBoss_HurtUpdate,CCInteger::create(m_Hp->getHp()));	
}
