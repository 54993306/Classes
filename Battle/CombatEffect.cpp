#include "CombatEffect.h"
#include "common/CommonFunction.h"
#include "Battle/BattleLayer/BattleRoleLayer.h"
#include "Battle/BufExp.h"
#include "Battle/ConstNum.h"
#include "Battle/WarAssist.h"
#include "tools/CCShake.h"
#include "Battle/EffectData.h"
#include "Battle/BaseRole.h"
#include "Battle/RoleObject/RoleObject.h"
#include "Battle/CombatTask.h"
#include "Battle/RoleObject/RageObject.h"
#include "Battle/BattleLayer/BattleMapLayer.h"
#include "Battle/EffectObject.h"
#include "Battle/BattleResult.h"
#include "Battle/WarControl.h"
#include "Battle/BattleScene/BattleScene.h"
#include "Battle/BuffManage.h"
#include "Battle/WarManager.h"
#include "Battle/MapManager.h"
#include "Battle/RoleObject/HPObject.h"
#include "Battle/ComBatLogic.h"
#include "Battle/CPlayerSkillData.h"
#include "common/ShaderDataHelper.h"
#include "common/CGameSound.h"
#include "Battle/BattleTools.h"
#include "Battle/HurtCount.h"
#include "Battle/BattleMessage.h"
#include "Battle/BaseRoleData.h"
#include "Battle/RoleSkill.h"
#include "Battle/skEffectData.h"
#include "Battle/BattleCenter.h"
#include "model/DataCenter.h"

namespace BattleSpace{

	CombatEffect::CombatEffect()
		:m_Scene(nullptr),_armaturePlayerSkill(nullptr),_playerSkillData(nullptr),m_PlayerNum(0)
		,mManage(nullptr)
	{}

	CombatEffect::~CombatEffect()
	{
		CC_SAFE_RELEASE(_armaturePlayerSkill);

		CC_SAFE_DELETE(_playerSkillData);

		NOTIFICATION->removeAllObservers(this);

		//释放资源
		CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo("warScene/playerSkill/confirm.ExportJson");
		CCTextureCache::sharedTextureCache()->removeTextureForKey("warScene/playeSkill/confirm0.png");
	}

	void CombatEffect::addEvent()
	{
		NOTIFICATION->addObserver(this,callfuncO_selector(CombatEffect::AttackNull),B_AttactNull,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(CombatEffect::BattleEffect),B_AttackResult,nullptr);
	}


	bool CombatEffect::init()
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();
		mManage = BattleManage;
		addEvent();
		//加载配置信息
		_playerSkillData = new CPlayerSkillData;
		_playerSkillData->reloadFile();

		//加载骨骼
		CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("warScene/playerSkill/confirm.ExportJson");

		//创建骨骼
		_armaturePlayerSkill = CCArmature::create("confirm");
		_armaturePlayerSkill->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(CombatEffect::armatureMovementEventCallFunc));
		_armaturePlayerSkill->setPosition(ccp(winSize.width*0.5f, winSize.height*0.5f));
		_armaturePlayerSkill->setVisible(false);
		CC_SAFE_RETAIN(_armaturePlayerSkill);

		//绑定裁切区域
		CCClippingNode* pClippingNode = CCClippingNode::create();
		pClippingNode->setPosition(ccp(-winSize.width/2, -winSize.height/2));

		CCDrawNode*stencil = CCDrawNode::create();
		CCPoint rectangle[4];
		rectangle[0]= ccp(0, 132);
		rectangle[1]= ccp(winSize.width, 312);
		rectangle[2]= ccp(winSize.width, winSize.height);
		rectangle[3]= ccp(0, winSize.height);
		//绘制一个矩形
		ccColor4F white= {1, 1, 1, 1};
		stencil->drawPolygon(rectangle, 4, white, 1, white);
		pClippingNode->setStencil(stencil);

		//添加内容
		_armaturePlayerSkill->addChild(pClippingNode, _armaturePlayerSkill->getBone("lightball3")->getZOrder()+1, FRAME_ACTION_HEAD_CHILD_TAG);
		return true;
	}

	void CombatEffect::setScene(BattleScene* scene)
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();
		m_Scene = scene;
		SpData* data = mManage->getSpineData(ToString(9999));
		m_skeletonNode = SkeletonAnimation::createWithData(data->first);
		m_skeletonNode->eventListener = std::bind(&CombatEffect::SpineActionEvent,this,std::placeholders::_1,std::placeholders::_2);
		m_skeletonNode->endListener = std::bind(&CombatEffect::SpineActionEnd,this,std::placeholders::_1);
		m_skeletonNode->setPosition(ccp(winSize.width*0.5f, winSize.height*0.5f));
		m_Scene->getWarUI()->addChild(m_skeletonNode,-1); 
	}

	void CombatEffect::SpineActionEnd( int trackIndex ){
		m_skeletonNode->setVisible(false);
		mManage->setLogicState(true);
	}
	//连击造成伤害效果
	void CombatEffect::SpineActionEvent( int trackIndex, spEvent* pEvent )
	{
		switch ((eActionEvent)pEvent->intValue)
		{
		case eActionEvent::eHitEvent:
			{
				continuousHurt();
			}break;
		case eActionEvent::eShark:
			{
				NOTIFICATION->postNotification(B_Shark,nullptr);
			}break;
		case eActionEvent::eAttackEventEnd:
			{
				CCArray* monsters = mManage->getMonsters();
				CCObject* mObj = nullptr;
				CCARRAY_FOREACH(monsters,mObj)
				{
					BaseRole* alive = (BaseRole*)mObj;
					if(alive->getHp()<=0 
						&& alive->getRoleObject() != nullptr
						&& alive->getAliveState())
						alive->roleDie();
				}
			}break;
		default:
			break;
		}									//动画确定播放点,策划确定播放内容
		if (pEvent->intValue >= 200)			//音效从200号文件开始播放
			PlaySound(pEvent->intValue);
	}
	//伤害(实际扣血) = 攻击力^2/( 攻击力+目标防御 )
	void CombatEffect::continuousHurt()
	{
		float tAttackNum = CountAliveAtk();
		vector<BaseRole*>* Vec = mManage->inBattleMonsters();
		for (auto tRole : *Vec)
		{
			RoleObject* tObject = tRole->getRoleObject();
			if (tRole->getGridIndex() < C_BEGINGRID)
				continue;
			if (!tObject)
				continue;
			tObject->TurnStateTo(sStateCode::eHitState); 
			int lostNum = pow(tAttackNum,2)/(tAttackNum+tRole->getDef());
			if (lostNum <= 0)
				lostNum = 1;
			tRole->setHp(tRole->getHp()-lostNum);
			tObject->playerNum(lostNum,generalType);	//受击武将播放血量变化
		}
	}
	//公式：	攻击力=近战英雄攻击力总和*0.05+远程英雄攻击力总和*0.04+辅助英雄攻击力总和*0.1+肉盾英雄攻击力总和*0.08		
	float CombatEffect::CountAliveAtk()
	{
		float tHurt = 0;
		CCArray* heros = mManage->getHeros();
		CCObject* obj = nullptr;
		CCARRAY_FOREACH(heros,obj)
		{
			BaseRole* tRole = (BaseRole*)obj;
			if (tRole->getFatherID())
				continue;
			const HeroInfoData *c_data = DataCenter::sharedData()->getHeroInfo()->getCfg(tRole->getBaseData()->getRoleModel());
			if (!c_data)
			{
				CCLOG("[ *ERROR ] CombatEffect::CountAliveAtk heroDesc.csv MoveID=%d");
				tHurt += tRole->getAtk()*0.04f;
				continue;
			}
			switch (c_data->iType2)		//近战，远战，坦克，辅助
			{
			case 1:
				{
					tHurt += tRole->getAtk()*0.05f;
				}break;
			case 2:
				{
					tHurt += tRole->getAtk()*0.04f;
				}break;
			case 3:
				{
					tHurt += tRole->getAtk()*0.08f;
				}break;
			case 4:
				{
					tHurt += tRole->getAtk()*0.1f;
				}break;
			}			
		}
		return tHurt;
	}

	void CombatEffect::BatterSpine( int index )
	{
		if (mManage->getbattleOver())			//战斗胜利不再播放效果
			return;
		if (index >= 2)
		{
			char str[60] = {0};
			sprintf(str,"attack0%d",index-1);
			m_skeletonNode->setVisible(true);
			m_skeletonNode->setToSetupPose();
			m_skeletonNode->setAnimation(0,str,false);
		}else{
			mManage->setLogicState(true);
		}
	}

	void CombatEffect::BattleEffect(CCObject* ob)
	{
		BattleResult* Result = (BattleResult*)ob;
		BaseRole*alive = Result->getAlive();
		RoleObject* aliveOb = alive->getRoleObject();
		const skEffectData* efInfo = alive->getCurrEffect();									//状态性的数据	
		EffectInfo* effectinfo = mManage->getEffData()->getEffectInfo(efInfo->getEffectID());	
		if (!effectinfo)
		{
			effectinfo = mManage->getEffData()->getEffectInfo(10000021);
			CCLOG("[ ERROR ] CombatEffect::BattleEffect EffectInfo NULL %d",efInfo->getEffectID());
		}
		if (Result->getusNum()&&Result->getusType())
			aliveOb->playerNum(Result->getusNum(),Result->getusType());						//攻击武将播放血量变化(吸血类效果有时应该差时而非同步播放)
		for(vector<unsigned int>::iterator iter = Result->m_HitTargets.begin();iter!=Result->m_HitTargets.end();++iter)
		{
			BaseRole* pAlive = Result->m_Alive_s[*iter];
			RoleObject* pAliveOb = pAlive->getRoleObject();
			if ( !pAlive|| !pAliveOb)continue;													
			EffectObject* SkillEffect = EffectObject::create(ToString(effectinfo->getfoeEft()));									//受击目标播放受击特效
			SkillEffect->setSkewing(true);
			SkillEffect->setScale(1/pAliveOb->getScale());
			pAliveOb->addChild(SkillEffect);
			SkillEffect->setShaderEffect(aliveOb->getArmature()->getShaderProgram());
			//       
			EffectObject* FloorEffect = EffectObject::create(ToString(effectinfo->getFloorEf()));	//受击目标播放受击特效
			WarMapData* map = ManageCenter->getMap()->getCurrWarMap();
			CCPoint p = pAlive->getRoleObject()->getPosition();
			CCPoint wp = m_Scene->getBattleRoleLayer()->convertToWorldSpace(p);
			CCPoint mp = m_Scene->getBattleMapLayer()->convertToNodeSpace(wp);
			FloorEffect->setSkewing(true);
			FloorEffect->setPosition(mp);															//特效加在地板上,但是位置应和人物实际站位重合
			m_Scene->getBattleMapLayer()->addChild(FloorEffect);	
			FloorEffect->setShaderEffect(aliveOb->getArmature()->getShaderProgram());

			if (effectinfo->getisShake())															//震屏处理
				NOTIFICATION->postNotification(B_Shark,nullptr);
			if (Result->m_LostHp[*iter].hitNum < 0)													//播放受击动作
			{
				pAliveOb->TurnStateTo(sStateCode::eHitState); 
				if (!alive->getOtherCamp())
					NOTIFICATION->postNotification(B_ContinuousNumber);			//刷新连击处理
			}
			pAliveOb->playerNum(Result->m_LostHp[*iter].hitNum,Result->m_LostHp[*iter].hitType);	

			if (Result->m_Repel[*iter] != pAlive->getGridIndex())
			{
				pAlive->setMoveGrid(Result->m_Repel[*iter]);
				pAliveOb->setMoveState(sStateCode::eHitState);
			}
		}
	}

	void CombatEffect::AttackNull(CCObject* ob)
	{
		BattleResult* Result = (BattleResult*)ob;
		BaseRole*tBaseRole = Result->getAlive();
		RoleObject* tRoleObject = tBaseRole->getRoleObject();
		const skEffectData* efInfo = tBaseRole->getCurrEffect();
		EffectInfo* effectinfo = mManage->getEffData()->getEffectInfo(efInfo->getEffectID());
		if (!effectinfo)
		{
			CCLOG("[ *ERROR ] CombatEffect::AttackNull");
			return;
		}
		if (!Result->m_HitTargets.size())							//未打中目标播放受击音效
		{
			tRoleObject->setEffectMusic(effectinfo->getfoeMusicId());
			if (effectinfo->getisblack())															//震屏处理
				NOTIFICATION->postNotification(B_Shark,nullptr);
		}
		tRoleObject->setPlayerEffect(effectinfo->getfoeEft());
	}

	void CombatEffect::PlayerSkill(BaseRole* alive)
	{
		CCSize size = CCDirector::sharedDirector()->getWinSize();
		CCNode* SkillEffect = CCNode::create();
		SkillEffect->setPosition(CCPointZero);
		RoleObject* aliveOb = alive->getRoleObject();
		CCNode* armature = aliveOb->getArmature();
		EffectObject* crit_eff = EffectObject::create("10031");
		crit_eff->setPosition(ccp(0,armature->getContentSize().height*0.5f));
		aliveOb->addChild(crit_eff);
		float time = crit_eff->getTotalTime();
		PlayEffectSound(SFX_503);

		NOTIFICATION->postNotification(B_ContinuousNumber,alive);			//刷新连击处理
		int offs_x = 200;												//武将距离屏幕边缘的距离
		CCNode* MoveLayer = m_Scene->getMoveLayer();
		CCPoint p = MoveLayer->getPosition();
		CCPoint p1 = aliveOb->getParent()->convertToWorldSpace(aliveOb->getPosition());
		WarMapData* m_map = ManageCenter->getMap()->getCurrWarMap();
		//敌方大技能特效
		if (alive->getEnemy())
		{
			alive->setAtkDelay(time);
			CCCallFuncN* callfunc = CCCallFuncN::create(this ,callfuncN_selector(CombatEffect::SkillEfHandleForEnemy));
			SkillEffect->runAction(callfunc);
			SkillEffect->setUserData(alive);
		}else{
			//我方大技能特效
			CCDelayTime* dtm = CCDelayTime::create(time);
			CCCallFuncO* callfunc = CCCallFuncO::create(this ,callfuncO_selector(CombatEffect::SkillEfHandle), alive);
			CCSequence* squ = CCSequence::create(dtm,callfunc,NULL);
			SkillEffect->runAction(squ);
			SkillEffect->setUserData(alive);

			SkillEffect->addChild(_armaturePlayerSkill);
			if (!alive->getOpposite())
				offs_x = CCDirector::sharedDirector()->getWinSize().width - offs_x;
			int move_x = p1.x-offs_x;					//目标点和显示点之间偏移量
			int newX = p.x-move_x;						//父节点如何移动才能显示目标点
			if( newX < MAP_MINX(m_map) )
				newX = MAP_MINX(m_map);
			if (newX > 0)newX = 0;
			CCMoveTo* moveTo = CCMoveTo::create(0.2f,ccp(newX,MoveLayer->getPositionY()));
			if (mManage->getNormal())																//非普通关卡技能不移动位置
				MoveLayer->runAction(moveTo);
			float dx = (newX - MoveLayer->getPositionX())*0.3f;
			float layx = 0;
			CLayout *lay = dynamic_cast<CLayout*>(m_Scene->getBattleMapLayer()->getMaplayer()->findWidgetById("yuanjing"));		//地图远景移动处理
			layx = lay->getPositionX();				//记录之前的位置
			CCMoveTo* moveTo2 = CCMoveTo::create(0.2f,ccp(layx-dx,lay->getPositionY()));
			lay->runAction(moveTo2);
		}
		WarControl* m_ui = m_Scene->getWarUI();
		m_ui->addChild(SkillEffect);
	}

	void CombatEffect::SkillEfHandleForEnemy( CCObject* ob )
	{
		CCNode* SkillEffect = (CCNode*)ob;

		//获取技能文本
		const SkillCfg* pSkillCfg = DataCenter::sharedData()->getSkill()->getCfg(((BaseRole*)SkillEffect->getUserData())->getCurrSkill()->getSkillID());
		if(!pSkillCfg)
		{
			return;
		}

		CCSize size = CCDirector::sharedDirector()->getWinSize();
		CCSprite* skillname = CCSprite::create("warScene/box4.png");//框
		skillname->setPosition(ccp(size.width/2,size.height*0.6f));
		SkillEffect->addChild(skillname);

		std::string text = pSkillCfg->name;
		CCSprite* pSprite = CCSprite::create(CCString::createWithFormat("warScene/playerSkill/%d.png", pSkillCfg->id)->getCString());
		if(!pSprite)
		{
			CCLOG("[ *ERROR ]CombatEffect::SkillEfHandleForEnemy Lost Image warScene/playerSkill/%d.png", pSkillCfg->id);
			pSprite = CCSprite::create("warScene/playerSkill/10000020.png");
			CCTextureCache::sharedTextureCache()->removeTextureForKey("warScene/playerSkill/10000020.png");
		}else{
			CCTextureCache::sharedTextureCache()->removeTextureForKey(CCString::createWithFormat("warScene/playerSkill/%d.png", pSkillCfg->id)->getCString());
		}
		bool isDoubleLine = pSprite->getContentSize().height>60;
		pSprite->setColor(ccc3(0, 0, 0));
		pSprite->setAnchorPoint(ccp(0.5f, 0.5f));
		pSprite->setScale(1.0f);
		pSprite->setPosition(ccp(skillname->getContentSize().width*0.5f, skillname->getContentSize().height*0.5f));
		skillname->addChild(pSprite);

		float scaleX = 1.0f;
		float scaleY = isDoubleLine?1.2f:1.0f;
		pSprite->setScaleY(isDoubleLine?0.83f:1.0f);
		skillname->setScale(3.0f);
		CCScaleTo* cs1 = CCScaleTo::create(0.1f, scaleX, scaleY);
		CCEaseBounceOut * es= CCEaseBounceOut::create(cs1);
		CCCallFuncO* cef = CCCallFuncO::create(this,callfuncO_selector(WarAssist::playerMusic),CCInteger::create(100));
		CCShake* sk = CCShake::create(0.3f,20.0f);
		CCCallFuncO* ef2 = CCCallFuncO::create(this,callfuncO_selector(WarAssist::shake), skillname);
		CCCallFuncO* callfunc = CCCallFuncO::create(this ,callfuncO_selector(CombatEffect::skillShowCallBack), SkillEffect);
		CCDelayTime * delay = CCDelayTime::create(0.5f);								//延时动作
		CCSequence* sqe = CCSequence::create(es,cef,sk,delay,callfunc,NULL);
		skillname->runAction(sqe);

		//白光
		CCSprite* ef = CCSprite::create("warScene/skillef.png");
		ef->setScale(2);
		ef->setZOrder(2);
		ef->setAnchorPoint(ccp(0,0));
		ef->setVisible(true);
		CCFadeTo* fato = CCFadeTo::create(0.25f,100);
		CCFadeTo* _fato = CCFadeTo::create(0.25f,200);
		ef->runAction(CCRepeatForever::create(CCSequence::create(fato,_fato,NULL)));
		SkillEffect->addChild(ef);
	}

	////技能特效结束执行回调
	void CombatEffect::SkillEfHandle(CCObject* ob) 
	{ 
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();
		BaseRole* pAlive = (BaseRole*)ob;
		_armaturePlayerSkill->setVisible(true);
		_armaturePlayerSkill->getAnimation()->playWithIndex(m_PlayerNum);
		PlayEffectSound(SFX_504);

		//根据模型ID获取数据
		const CFrameActionData& _frameData = _playerSkillData->getDataById(pAlive->getModel()); 
		CCString* pHeadStr = CCString::createWithFormat("warScene/playerSkill/%s", _frameData.sHeadPic.c_str());		//头像
		CCSprite* pNewHead = CCSprite::create(pHeadStr->getCString());
		if(!pNewHead)
		{
			CCLOG("ERROR  CombatEffect::SkillEfHandle");
			pNewHead = CCSprite::create("warScene/playerSkill/331.png");
		}
		pNewHead->setOpacity(_frameData.iOpacity[0]);
		pNewHead->setScale(_frameData.fScale);
		pNewHead->setScaleX(_frameData.bFlipX?-_frameData.fScale:_frameData.fScale);
		pNewHead->setRotation(_frameData.iRoate);
		pNewHead->setPosition(_frameData.pos[0]+ccp(winSize.width/2, winSize.height/2));
		pNewHead->runAction(CCSpawn::create(
			CCFadeTo::create(_frameData.fTime, _frameData.iOpacity[1]), 
			CCMoveTo::create(_frameData.fTime, _frameData.pos[1]+ccp(winSize.width/2, winSize.height/2)),
			nullptr
			));
		//添加到父节点
		CCNode* pNodeClipping = _armaturePlayerSkill->getChildByTag(FRAME_ACTION_HEAD_CHILD_TAG);
		pNodeClipping->addChild(pNewHead, FRAME_ACTION_HEAD_CHILD_TAG, FRAME_ACTION_HEAD_CHILD_TAG);

		//更换艺术字图片
		CCString* pTextStr = CCString::createWithFormat("warScene/playerSkill/%s", _frameData.sTextPic.c_str()); 
		CCSprite* pText = CCSprite::create(pTextStr->getCString());
		if(!pText)
		{
			CCLOG("ERROR CombatEffect::SkillEfHandle");
			pText = CCSprite::create("warScene/playerSkill/10351050.png");
		}
		pText->setPosition(ccp(810, 92));
		pText->setAnchorPoint(ccp(0.5, 0.5f));
		pText->runAction(CCSequence::create(CCMoveBy::create(0.3f, ccp(-570, -155)), CCMoveBy::create(1.0f, ccp(0, -5)), CCMoveBy::create(0.1f, ccp(-570, -120)),NULL));
		pText->setRotation(-10);
		pText->setScale(1.2f);

		//添加到父节点
		_armaturePlayerSkill->addChild(pText, _armaturePlayerSkill->getBone("heizi0")->getZOrder()+1, FRAME_ACTION_TEXT_CHILD_TAG);

		//移除图片
		//CCTextureCache::sharedTextureCache()->removeTextureForKey(pHeadStr->getCString());
		//CCTextureCache::sharedTextureCache()->removeTextureForKey(pTextStr->getCString());
	}

	void CombatEffect::armatureMovementEventCallFunc( CCArmature * pArmature, MovementEventType type, const char *data )
	{
		switch (type)
		{
		case cocos2d::extension::START:
			{}break;
		case cocos2d::extension::COMPLETE:
			{
				_armaturePlayerSkill->getParent()->removeFromParentAndCleanup(true);
				_armaturePlayerSkill->setVisible(false);
				_armaturePlayerSkill->getChildByTag(FRAME_ACTION_HEAD_CHILD_TAG)->getChildByTag(FRAME_ACTION_HEAD_CHILD_TAG)->removeFromParentAndCleanup(true);
				_armaturePlayerSkill->getChildByTag(FRAME_ACTION_TEXT_CHILD_TAG)->removeFromParentAndCleanup(true);
				_armaturePlayerSkill->removeFromParentAndCleanup(false);		
				mManage->setLogicState(true);
			}break;
		case cocos2d::extension::LOOP_COMPLETE:{}break;
		default:break;
		}
	}

	void CombatEffect::skillShowCallBack(CCObject* ob)
	{
		CCNode* pNode = (CCNode*)ob;
		pNode->removeFromParentAndCleanup(true);
	}

	void CombatEffect::showVsAnimate( CombatLogic *pCombatLogic )
	{
		WarControl* m_ui = m_Scene->getWarUI();
		CCLayer *pLayerShow = CCLayer::create();
		m_Scene->addChild(pLayerShow, 999);
		pLayerShow->setPosition(m_ui->convertToWorldSpace(CCPointZero));

		
		pLayerShow->setVisible(false);
		pLayerShow->runAction(CCSequence::create(
			CCDelayTime::create(0.3f),
			CCCallFuncN::create(this, callfuncN_selector(CombatEffect::showVsAnimateCallBack)),
			CCDelayTime::create(2.0f),
			CCRemoveSelf::create(),
			CCCallFuncO::create(pCombatLogic, callfuncO_selector(CombatLogic::moveStoryEnd), nullptr),
			nullptr
			));
	}

	void CombatEffect::showVsAnimateCallBack( CCNode *pSender )
	{
		CCLayer *pLayerShow = (CCLayer *)pSender;
		pLayerShow->setVisible(true);

		PlayEffectSound(SFX_515);

		//动画数据
		VsAnimateData &vsData = DataCenter::sharedData()->getVsAnimateData();

		//播放动画
		SkeletonAnimation *pLow = SkeletonAnimation::createWithFile("pvp/vs/pvp_vs_ani_1.json", "pvp/vs/pvp_vs_ani_1.atlas", 1);
		pLow->setPosition(ccp(DESIGN_WIDTH/2, 0));
		pLow->setAnimation(0, "stand", false);
		pLayerShow->addChild(pLow, 3);
		CCTextureCache::sharedTextureCache()->removeTextureForKey("pvp/vs/pvp_vs_ani_1.png");
		CCTextureCache::sharedTextureCache()->removeTextureForKey("pvp/vs/pvp_vs_ani_12.png");

		SkeletonAnimation *pHigh = SkeletonAnimation::createWithFile("pvp/vs/pvp_vs_ani_2.json", "pvp/vs/pvp_vs_ani_2.atlas", 1);
		pHigh->setPosition(ccp(DESIGN_WIDTH/2, 0));
		pHigh->setAnimation(0, "stand", false);
		pLayerShow->addChild(pHigh, 1);
		CCTextureCache::sharedTextureCache()->removeTextureForKey("pvp/vs/pvp_vs_ani_2.png");

		//上贴图和名字
		CCSprite *pBodyPic[2] = {nullptr, nullptr};
		int iBodyId[2] = {vsData.iEnemyLeader, vsData.iSelfLeader};
		CCPoint pAnchor[2] = { ccp(1.0f, 0), ccp(0.0f, 1.0f) };
		bool bFlipX[2] = { true, false };
		CCPoint pPosMove[2] = { ccp(0, 150), ccp(DESIGN_WIDTH, 516) };
		std::string sName[2] = {vsData.sEnemyRoleName, vsData.sSelfRoleName};
		CCPoint pNamePos[2] = { ccp(429, 288), ccp(709, 392) };
		float fRotation = -18.4f;
		for( int i=0; i<2; i++ )
		{
			//贴图
			CCSprite	*pBody = CCSprite::create( CCString::createWithFormat("warScene/playerSkill/%d.png", iBodyId[i])->getCString() );
			if(!pBody)
			{
				pBody = CCSprite::create("warScene/playerSkill/146.png");
				CCLOG("[ ERROR ] no file found warScene/playerSkil/%d", iBodyId[i]);
			}
			pLayerShow->addChild(pBody, 2);
			pBody->setOpacity(0);
			pBody->setAnchorPoint(pAnchor[i]);
			pBody->setFlipX(bFlipX[i]);
			pBody->setPosition( pPosMove[i] );
			pBody->setRotation( -18.4f);
			pBody->runAction(
				CCRepeat::create(
				CCSequence::create(
				CCDelayTime::create(0.1f),
				CCSpawn::create(
				CCMoveTo::create( 0.2f, pPosMove[1-i] ),
				CCFadeIn::create( 0.2f ), 
				nullptr
				),
				CCDelayTime::create(1.2f),
				CCSpawn::create(
				CCMoveTo::create( 0.2f, pPosMove[i] ),
				CCFadeOut::create( 0.2f ), 
				nullptr
				),
				nullptr)
				, 1)
				);
			CCTextureCache::sharedTextureCache()->removeTexture(pBody->getTexture());

			//名字
			CLabel *pName = CLabel::create(sName[i].c_str(), "Arial", 54);
			pLayerShow->addChild(pName, 4);
			pName->setPosition(pPosMove[i]);
			pName->setColor(ccc3(255, 255, 255));
			pName->setAnchorPoint(pAnchor[i]);
			pName->setRotation(fRotation);
			pName->runAction(
				CCRepeat::create(
				CCSequence::create(
				CCDelayTime::create(0.1f),
				CCSpawn::create(
				CCMoveTo::create( 0.2f, pNamePos[i] ),
				CCFadeIn::create( 0.2f ), 
				nullptr
				),
				CCDelayTime::create(1.2f),
				CCSpawn::create(
				CCMoveTo::create( 0.2f, pPosMove[i] ),
				CCFadeOut::create( 0.2f ), 
				nullptr
				),
				nullptr)
				, 1)
				);
		}
	}
};