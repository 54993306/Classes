﻿
#include "Battle/RoleObject/RoleObject.h"
#include "tools/StringUtil.h"
#include "model/WarManager.h"
#include "model/DataCenter.h"
#include "model/BuffManage.h"
#include "Battle/BaseRole.h"
#include "model/MapManager.h"
#include "Battle/ConstNum.h"
#include "Battle/EffectData.h"
#include "Battle/CombatTask.h"
#include "Battle/RoleObject/HPObject.h"
#include "common/CommonFunction.h"
#include "Battle/AnimationManager.h"
#include "Battle/EffectObject.h"
#include "Battle/ArmatureEventDataMgr.h"
#include "common/ShaderDataHelper.h"
#include "Battle/BattleMessage.h"
#include "Battle/BattleTools.h"
#include "Battle/MoveObject.h"
#include "Battle/BaseRoleData.h"
#include "Battle/StateManage/GameTransition.h"
#include "Battle/StateManage/GameStateManager.h"
#include "Battle/StateManage/PostureState.h"
#include "Battle/StateManage/StateManager.h"
#include "Battle/ActionNameDefine.h"
//#include <spine/AnimationState.h>
namespace BattleSpace{
	RoleObject::RoleObject()
		:m_MapData(nullptr),m_armatureEventData(nullptr), m_lastFrame(-1),m_Reset(false)
	{}

	RoleObject::~RoleObject()
	{
		NOTIFICATION->removeAllObservers(this);
	}
	bool RoleObject::init()
	{
		if (!AliveObject::init())
			return false;
		m_MapData = DataCenter::sharedData()->getMap()->getCurrWarMap();
		m_Manage = DataCenter::sharedData()->getWar();
		return true;
	}

	void RoleObject::setAlive( BaseRole* pRole )
	{
		AliveObject::setAlive(pRole);
		initStateManage(); 
		if (!mRole->getAliveID())
			return;															//挂在层显示拖动的目标(可以单独抽出一个方法来)
		mRole->setRoleObject(this);										//逻辑对象与显示对象绑定
		setEnemy(pRole->getEnemy());
		setModel(mRole->getModel());
		setHp(nullptr);
		getBody()->setScale(mRole->getZoom());
		if (mRole->getCloaking())											//潜行怪物处理
			getArmature()->setOpacity(125);
		mRole->roleIntoBattle();
	}

	void RoleObject::initStateManage()
	{
		GameStateManager* stateManage = GameStateManager::create();						//继承实现状态管理器
		GameStateTransition* transition = GameStateTransition::create();				//继承实现状态切换列表对象
		this->setStateManager(stateManage);												//设置状态机的状态管理器
		this->setTransition(transition);												//设置状态切换对象
		transition->RoleTransition(mRole->getEnemy());
	}

	void RoleObject::setModel(int pModel)//设置人物对应模型
	{
		mModel = pModel;
		if ( m_Manage->isSpine(mModel) )
		{
			m_IsSpine = true;
			initSpineModel();
		}else{
			m_IsSpine = false;
			initCocosModel();
		}
		m_Armature->setPosition(ccp(0,-GRID_HEIGHT/2));
		m_Body->addChild(m_Armature);
		this->getStateManage()->initState(this);							//设置人物的初始动作为站立也可为其他						
	}

	void RoleObject::setActionKey(string ActionKey)//设置人物对应动作(动作切换)然后刷新人物运行动作
	{
		m_ActionKey = ActionKey;
		if (m_ActionKey == Walk_Action)			//我方突然中断整个战斗流程的情况(玩家操纵拖动武将)
			toWlkActionDispose();
		if (m_IsSpine)
		{
			setSpineAction();
		}else{
			setCocosAction();
		}
	}

	void RoleObject::toWlkActionDispose()
	{
		NOTIFICATION->postNotification(B_CancelDrawAttackArea,mRole);//取消绘制攻击范围(针对性的取消绘制)
		if (mRole->getNorAtk()&&!mRole->getSpeAtk()&&!mRole->getCriAtk())
		{
			mRole->ResetAttackState();
			mRole->setNorAtk(true);					//普通攻击情况下,移动不需重置攻击时间
		}else{
			mRole->ResetAttackState();
		}
	}

	void RoleObject::setSpineAction()
	{
		SkeletonAnimation* Skeleton = (SkeletonAnimation*)m_Armature;
		hideSpineEffect();
		for (int i = 0;i<Skeleton->state->data->skeletonData->animationsCount;i++)
		{
			if ( !strcmp(Skeleton->state->data->skeletonData->animations[i]->name,m_ActionKey.c_str()))
			{
				if ( !strcmp(m_ActionKey.c_str(),Die_Action))
					((SkeletonAnimation*)m_Armature)->setAnimation(0,m_ActionKey.c_str(),false);
				else
					((SkeletonAnimation*)m_Armature)->setAnimation(0,m_ActionKey.c_str(),true);
				setSpineEffectAction();
				return;
			}
		}
		lostAction();
	}

	void RoleObject::setSpineEffectAction()
	{
		if ( !m_Skeleton)
			return;
		for (int i = 0;i<m_Skeleton->skeleton->data->animationsCount;i++)
		{
			if ( !strcmp(m_Skeleton->skeleton->data->animations[i]->name,m_ActionKey.c_str()))
			{
				m_Skeleton->setVisible(true);
				m_Skeleton->setAnimation(0,m_ActionKey.c_str(),false);
				return;
			}
		}
		hideSpineEffect();
	}

	void RoleObject::hideSpineEffect()
	{
		if (!m_Skeleton)
			return;
		m_Skeleton->setToSetupPose();
		m_Skeleton->setVisible(false);
	}

	void RoleObject::lostAction()
	{
		if (m_ActionKey.compare(Start_Action)==0)
		{
			mRole->setAliveStat(COMMONSTATE);
		}else if ( !strcmp(m_ActionKey.c_str(),Stand_Action))
		{
			CCLOG("[ **ERROR ] ActObject::lostAction Lost StandAction");
			return;
		}else{
			CCLOG("[ *ERROR ] ActObject::lostAction Lost Action=[%s],model=[%d]",m_ActionKey.c_str(),mModel);
		}
		this->TurnStateTo(E_StateCode::eStandState);
	}

	void RoleObject::setCocosAction()
	{
		CCArmature* Armature = (CCArmature*) m_Armature;
		CCMovementData* MovementData = Armature->getAnimation()->getAnimationData()->getMovement(m_ActionKey.c_str());
		if (MovementData)				//判断动作是否存在
		{
			Armature->getAnimation()->play(m_ActionKey.c_str(),0.01f);
		}else{
			lostAction();
		}
	}

	void RoleObject::initSpineEffect()
	{
		char efName[60] = {0};
		sprintf(efName,"%d_texiao",mModel);
		SpData* EFdata = m_Manage->getSpineData(efName);
		if (EFdata)
		{
			m_Skeleton = SkeletonAnimation::createWithData(EFdata->first);
			m_Skeleton->setPosition(ccp(0,-GRID_HEIGHT/2));
			m_Body->addChild(m_Skeleton,1);
		}else{
			CCLOG("[ *TIPS ]  ActObject::setModel Spine Model=%d IS NULL",mModel); 
		}
	}

	void RoleObject::initSpineModel()
	{
		SpData* data = m_Manage->getSpineData(ToString(mModel));
		if (!data)
		{
			data = m_Manage->getSpineData("146");
			CCLOG("[ *ERROR ]  ActObject::setModel Spine Model=%d IS NULL",mModel); 
		}
		SkeletonAnimation*  Animation = SkeletonAnimation::createWithData(data->first);
		CCAssert(Animation,"ActObject::setModel Spine NULL");
		Animation->endListener = std::bind(&RoleObject::SpineActionEnd,this,std::placeholders::_1);
		Animation->completeListener = std::bind(&RoleObject::SpineComplete,this,std::placeholders::_1,std::placeholders::_2);
		Animation->eventListener = std::bind(&RoleObject::SpineActionEvent,this,std::placeholders::_1,std::placeholders::_2);
		m_Armature = Animation;
		initSpineEffect();
	}

	void RoleObject::initCocosModel()
	{
		CCAnimationData *animationData = CCArmatureDataManager::sharedArmatureDataManager()->getAnimationData(ToString(mModel));
		if (!animationData)													//模型容错性处理
		{
			CCLOG("[ *ERROR ]  ActObject::setModel Animation Model=%d IS NULL",mModel); 
			mModel = 516;
		}	
		CCArmature* Armature = CCArmature::create(ToString(mModel));
		Armature->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(RoleObject::AnimationEvent));		//动作结束回调
		m_Armature = Armature;
		m_armatureEventData = m_Manage->getArmatureDataMgr()->getArmatureEventData(mModel);
	}
	//动作结束回调
	void RoleObject::AnimationEvent(CCArmature *armature, MovementEventType movementType, const char *movementID)
	{
		std::string ActionName = movementID;
		ActionName = strRemoveSpace(ActionName);						//去空格
		if (movementType == LOOP_COMPLETE || movementType == COMPLETE)	//动作结束的标记(分为是否循环动作两种)
			ActionEnd(ActionName.c_str());
	}
	//在end里面set动作会崩掉所以引入了Complete方法
	void RoleObject::SpineActionEnd( int trackIndex )
	{
		spTrackEntry*TrackEntry = ((SkeletonAnimation*)m_Armature)->getCurrent(trackIndex);
		if (TrackEntry)
		{
			if (strcmp(TrackEntry->animation->name,Die_Action) == 0)
				ActionEnd(Die_Action);
		}else{
			CCLOG("[ *ERROR ] ActObject::SpineEnd spTrackEntry NULL");
		}
	}
	//在循环回调的时候判断动作结束处理
	void RoleObject::SpineComplete( int trackIndex,int loopCount )
	{
		spTrackEntry*TrackEntry = ((SkeletonAnimation*)m_Armature)->getCurrent(trackIndex);
		if (TrackEntry)
		{
			if (m_Skeleton)
			{
				m_Skeleton->setToSetupPose();
				m_Skeleton->setVisible(false);
			}
			ActionEnd(TrackEntry->animation->name);
		}else{
			CCLOG("[ *ERROR ] ActObject::SpineComplete spTrackEntry NULL");
		}
	}

	void RoleObject::attackActionEnd()
	{
		TurnStateTo(E_StateCode::eStandState);
		if (mRole->getCritEffect()&&!mEnemy)		
			NOTIFICATION->postNotification(B_CritEnd,mRole);
		if (mRole->getHp()<=0)
			this->AliveDie();									//释放完技能后自己死亡的情况处理
		else if (mRole->getBaseData()->getCallType() == AutoSkill||
			mRole->getBaseData()->getCallType() == OnlyOnce)				//陨石类释放攻击后死亡OnlyOnces
			this->AliveDie();	
		AtkEnd_Event();	
	}

	void RoleObject::ActionEnd( const char* ActionName )
	{
		if (strcmp(ActionName,Attack_Action) == 0		||			//普攻转站立
			strcmp(ActionName,SpAttack_Action) == 0		||			//特攻转站立			
			strcmp(ActionName,Skill_Action) == 0		||			//必杀技转站立
			strcmp(ActionName,Win_Action)==0		)				//胜利动作转站立		
		{
			attackActionEnd();												
		}else if ( !strcmp(ActionName,Hit_Action) ||				//受击转站立
			!strcmp(ActionName,Dizzy_Action)		)			//眩晕转站立
		{
			TurnStateTo(E_StateCode::eStandState);
		}else if (!strcmp(ActionName,Die_Action))
		{
			dieActionEnd();
		}else if (!strcmp(ActionName,Start_Action))
		{
			TurnStateTo(E_StateCode::eStandState);
			mRole->setAliveStat(COMMONSTATE);
		}else{
			if (strcmp(ActionName,Stand_Action)==0||strcmp(ActionName,Walk_Action)==0)
				return;
			CCLOG("[ Tips ] ActObject::animationEvent Action=%s Not Dispose",ActionName);
		}
	}

	void RoleObject::SpineActionEvent( int trackIndex,spEvent* Event )
	{
		switch ((E_ActionEvent)Event->intValue)
		{
		case E_ActionEvent::eBeginEvevt:
			{
				AtkBegin_Event();			//开始攻击(受击数组受伤)
			}break;
		case E_ActionEvent::eHitEvent:
			{
				HpChange_Event();			//显示受击方血量处理
				AtkBegin_Event();			//开始攻击(受击数组受伤)
			}break;
		case E_ActionEvent::eShark:
			{
				NOTIFICATION->postNotification(B_Shark,nullptr);
			}break;
		case E_ActionEvent::eMask:
			{
				NOTIFICATION->postNotification(B_SKILL_MASK,mRole);
			}break;
		default:
			break;
		}									//动画确定播放点,策划确定播放内容
		if (Event->intValue >= 200)			//音效从200号文件开始播放
			PlaySound_Event(Event->intValue);
	}

	void RoleObject::actionEvent( int pFremIndex )
	{
		for(unsigned int frameIndex = m_lastFrame+1; frameIndex<=pFremIndex; frameIndex++)	//防止偶尔的掉帧情况出现处理
		{
			vector<ArmatureEvent*>& frameEvents = m_armatureEventData->getEventVector(m_ActionKey.c_str(), frameIndex);	//获取当前帧的所有事件
			for(unsigned int i=0; i<frameEvents.size(); i++)
			{
				const ArmatureEvent& armatureEvent = *(frameEvents.at(i));
				string sEventName = armatureEvent.name;
				if(sEventName.size()>0)
				{
					sEventName = strRemoveSpace(sEventName);											//去空格
					if(sEventName.compare(EVE_ATKBEGIN)			==0	)		AtkBegin_Event();			//开始攻击(受击数组受伤)
					if(sEventName.compare(EVE_LOSTHP)			==0	)		HpChange_Event();			//显示受击方血量处理
					if(sEventName.compare(EVE_ANIMAT)			==0 )		PlayAnimat_Event(armatureEvent.extraInfo);			//播放动画
					if(sEventName.compare(EVE_SOUND)			==0 )		PlaySound_Event(armatureEvent.sound);				//播放声音
					if(sEventName.compare(EVE_SHAKE)			==0	)
						NOTIFICATION->postNotification(B_Shark,nullptr);
					if (sEventName.compare(EVE_MASK)			==0 )
						NOTIFICATION->postNotification(B_SKILL_MASK,mRole);
				}
			}
		}
		m_lastFrame = pFremIndex;
	}

	void RoleObject::updateFrameEvent( float dt )
	{
		if (m_IsSpine)
			return;
		if( !m_armatureEventData					||
			m_ActionKey.compare(Stand_Action)==0	||
			m_ActionKey.compare(Hit_Action)==0		||
			m_ActionKey.compare(Dizzy_Action)==0)
			return ;
		int iCurrentFrameIndex = ((CCArmature*)m_Armature)->getAnimation()->getCurrentFrameIndex();
		if(m_lastFrame ==iCurrentFrameIndex)															//骨骼帧率与游戏帧率不同例如：游戏2帧骨骼才跑了1帧的情况(高效)
			return;
		actionEvent(iCurrentFrameIndex);
	}

	void RoleObject::dieActionEnd()
	{
		if (mRole->getCritEffect()&&!mEnemy)					//放技能的过程中把自己炸死的特殊情况
			NOTIFICATION->postNotification(B_CritEnd,mRole);
		CCFadeOut* fadeout = CCFadeOut::create(0.5f);
		m_Armature->runAction(fadeout);
		getHp()->setVisible(false);
		CCDelayTime* dtime = CCDelayTime::create(0.6f);
		CCCallFuncN* callfunc = CCCallFuncN::create(this ,callfuncN_selector(RoleObject::removeAct));
		CCSequence* sqe = CCSequence::create(dtime,callfunc,NULL); 
		this->runAction(sqe);
	}

	void RoleObject::removeAct( CCNode* node )
	{
		RoleObject* act = (RoleObject*) node;
		BaseRole* alive = act->getAlive();
		NOTIFICATION->postNotification(B_ActObjectRemove,alive);		//我方主帅死亡,战斗失败
		if (m_Reset)
		{
			m_Reset = false;		
		}else{
			mRole->setRoleObject(nullptr);
		}
		act->removeFromParentAndCleanup(true); 
		if (CCDirector::sharedDirector()->getScheduler()->getTimeScale() < 1)
			CCDirector::sharedDirector()->getScheduler()->setTimeScale(1);
	}

	void RoleObject::updatePosition(float dt)
	{
		E_StateCode ActionCode = this->getCurrActionCode();
		do{
			if (ActionCode != E_StateCode::eWalkState&&ActionCode!=E_StateCode::eHitState)
				break;
			int grid = m_MapData->getGridIndex(this->getPosition() - this->getoffs());
			if (grid == mRole->getGridIndex()||grid == INVALID_GRID)break;
			mRole->setGridIndex(grid);
			break;
		} while (true);

		if (getMoveState() != E_StateCode::eNullState)
			MoveUpdate(dt);
	}
	//改进方向，使用像素计算速度，而不是使用格子来计算速度。格子只能用于计算距离
	bool RoleObject::isDistination(float pDt)
	{
		CCPoint pPosition = m_MapData->getPoint(mRole->getMoveGrid());
		CCPoint uPosition = this->getPosition() - this->getoffs();

		float pLength = 0;
		float dtLenght = 0;
		if (this->getSpeed().x)
		{
			pLength = fabs(pPosition.x-uPosition.x);									//有一边超过就表示超过了
			dtLenght = fabs(ccpMult(this->getSpeed(),pDt).x);							//每次移动的大小
		}else{
			pLength = fabs(pPosition.y-uPosition.y);									//有一边超过就表示超过了
			dtLenght = fabs(ccpMult(this->getSpeed(),pDt).y);							//每次移动的大小
		}
		if (pLength<=dtLenght)
			return true;
		return false;
	}

	void RoleObject::moveEnd()
	{
		CCPoint tPosition = m_MapData->getPoint(mRole->getMoveGrid());
		this->setPosition(tPosition+this->getoffs());
		if (!mEnemy&&mRole->getCallType()!=AutoMoveType)
			this->TurnStateTo(E_StateCode::eStandState);											//站立时会自动将武将方向调转回来
		this->setSpeed(CCPointZero);
		this->setMoveState(E_StateCode::eNullState);
	}

	void RoleObject::MoveUpdate( float dt )
	{
		if (mRole->getMoveGrid() == INVALID_GRID)
		{
			this->setMoveState(E_StateCode::eNullState);
			return;
		}
		if (isDistination(dt))
		{
			moveEnd();
		}else{
			this->setPosition(this->getPosition()+ccpMult(this->getSpeed(),dt));
		}
	}

	bool RoleObject::firstBattle( CCPoint& p )
	{
		if (mRole->getBattle() || mRole->getEnemy())								//第一次召唤武将处理(未上阵的武将都瞬移)
			return false;
		m_MoveState = E_StateCode::eNullState;
		mRole->setAliveStat(INVINCIBLE);
		this->TurnStateTo(E_StateCode::eEnterState);
		NOTIFICATION->postNotification(B_RolrLogInBattlefield,mRole);//武将上阵
		mRole->setGridIndex(mRole->getMoveGrid());
		this->setSpeed(CCPointZero);
		this->setPosition(p);
		return true;
	}

	void RoleObject::setMoveState(E_StateCode pStateCode)
	{
		m_MoveState = pStateCode;
		if (m_MoveState == E_StateCode::eNullState)
			return;	
		if (!mRole->getMove())														//定身状态
		{
			this->TurnStateTo(E_StateCode::eStandState);
			m_MoveState = E_StateCode::eNullState;
			return;
		}
		if (this->getCurrActionCode() != pStateCode)
			this->TurnStateTo(pStateCode);
		roleMoveSpeed();
	}

	float RoleObject::getMoveTime()
	{
		int row = abs((mRole->getMoveGrid() % C_GRID_ROW) - (mRole->getGridIndex() % C_GRID_ROW));
		int col = abs((mRole->getMoveGrid() / C_GRID_ROW) - (mRole->getGridIndex() / C_GRID_ROW));
		float tMovetime = max(row,col) * mRole->getMoveSpeed();						//一格多少时间
		if (m_MoveState == E_StateCode::eHitState||!tMovetime)	//被击退的情况
		{
			tMovetime = 0.2f;	
			if (m_MoveState == E_StateCode::eHitState&&this->getCurrActionCode() != E_StateCode::eHitState)
				mRole->setGridIndex(mRole->getMoveGrid());							//动作没有切换成功，瞬间更新受击武将位置(攻击状态无法切换受击动作,无法更新武将位置了)
		}
		return tMovetime;
	}

	void RoleObject::roleMoveSpeed()
	{
		int MoveGrid = mRole->getMoveGrid();											//格子相等的时候可能存在格子大小的误差，应以点的位置来进行判断
		if (MoveGrid == INVALID_GRID)return;
		CCPoint p = m_MapData->getPoint(MoveGrid) + getoffs();							//目标点
		if (firstBattle(p))
			return ;
		if (mRole->getBaseData()->getRoleRow()==1&&mRole->getBaseData()->getRoleCol()==1&&mEnemy)
			p = CCPoint(p.x+CCRANDOM_0_1()*15+5,p.y);									//防止武将叠在一起
		walkDirection(p);
		this->setSpeed((p - getPosition())/getMoveTime());
	}

	void RoleObject::walkDirection( CCPoint& p )
	{
		if (this->getCurrActionCode() != E_StateCode::eWalkState)		
			return ;	
		if ( (mEnemy&&getPosition().x > p.x)  || 
			(!mEnemy&&getPosition().x < p.x) )				//移动翻转问题
		{
			if (mEnemy)									//改变武将方向
			{
				this->setRoleDirection(Ditection_Left);
			}else{
				this->setRoleDirection(Ditection_Right);
			}
		}else{
			if (mEnemy)									//改变武将方向
			{
				this->setRoleDirection(Ditection_Right);
			}else{
				this->setRoleDirection(Ditection_Left);
			}
		}
	}

	void RoleObject::initMoveObject( CCNode* pMoveParent )
	{
		if (mRole->getEnemy()	||
			mRole->getCaptain()||
			mRole->getCallType() != CommonType)
			return;
		MoveObject* tMoveObj = MoveObject::create();
		tMoveObj->setRowCol(mRole->getBaseData()->getRoleRow(),mRole->getBaseData()->getRoleCol());
		tMoveObj->setMoveAlive(mRole);
		tMoveObj->setOffs(getoffs().x,getoffs().y);
		tMoveObj->initMoveSprite();
		tMoveObj->setgrid(mRole->getGridIndex());
		mRole->setMoveGrid(mRole->getGridIndex());
		mRole->setMoveObject(tMoveObj);
		setMoveObject(tMoveObj);
		pMoveParent->addChild(tMoveObj);
	}

	void RoleObject::setActMoveGrid( int pGird )
	{
		getMoveObject()->setgrid(pGird);
		mRole->setMoveGrid(pGird);
		setMoveState(E_StateCode::eWalkState);
	}

	void RoleObject::AliveDie()
	{
		AliveObject::AliveDie();
		TurnStateTo(E_StateCode::eDeathState);
	}

}