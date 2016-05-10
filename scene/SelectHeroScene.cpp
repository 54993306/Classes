#include "SelectHeroScene.h"
#include "Global.h"
#include "model/DataCenter.h"
#include "net/CNetClient.h"
#include "common/CGameSound.h"
#include "tools/CCShake.h"
#include "warscene/ArmatureEventDataMgr.h"
#include "warscene/BattleTools.h"
#include "scene/alive/ActionDef.h"
#include "update/CDownloadPackage.h"
#include "scene/CPopTip.h"
#include "tools/ShowTexttip.h"

#define getm_ui() (CLayout*)this->getChildByTag(1)->getChildByTag(1)

SelectHeroScene::SelectHeroScene()
{
	m_bTouchLock = false;
	m_pArmature[0] = nullptr;
	m_pArmature[1] = nullptr;
	m_pArmature[2] = nullptr;
	m_pClose = nullptr;
	m_lastFrame = 0;
	m_armatureEventDataMgr = nullptr;
}

void SelectHeroScene::onCreate()
{
	CWidgetWindow* layout = CWidgetWindow::create();
	layout->setTag(1);
	addChild(layout);

	m_ui = LoadComponent("SelectRole.xaml");
	m_ui->setPosition(VCENTER);
	layout->addChild(m_ui);

	m_selectLayer = (CLayout *)(m_ui->findWidgetById("select"));

	m_InputLayer = (CLayout *)(m_ui->findWidgetById("input"));

	CCScale9Sprite *mask = (CCScale9Sprite*)m_ui->findWidgetById("mask");
	mask->setVisible(false);

	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(m_InputLayer->convertToNodeSpace(ccp(VLEFT+50, VTOP-50)));
	pClose->setOnClickListener(this,ccw_click_selector(SelectHeroScene::onClose));
	m_ui->addChild(pClose, 999);
	m_pClose = pClose;

	//输入角色框
	m_textFieldName = CursorTextField::textFieldWithPlaceHolder(GETLANGSTR(1112), FONT_NAME, 15,CCSizeMake(200,50),ccWHITE); 
	CCNode *node = (CCNode *)m_InputLayer->findWidgetById("roleInput");
	m_textFieldName->setPosition(ccp(node->getPositionX()-100, node->getPositionY()));
	m_textFieldName->setLimitNum(20);
	m_textFieldName->setAnchorPoint(ccp(0,0.5));
	m_textFieldName->setPriority(-11);
	m_InputLayer->addChild(m_textFieldName);
	string name = m_nameGm.getRandowName();
	m_textFieldName->setTextInput(name);

	m_armatureEventDataMgr = ArmatureEventDataMgr::create();
	m_armatureEventDataMgr->retain();
}
void SelectHeroScene::onEnter()
{
	CScene::onEnter();

	CCSize size = CCDirector::sharedDirector()->getWinSize();

	//隐藏人物信息
	m_InputLayer->setVisible(false);
	m_InputLayer->setPositionY(m_InputLayer->getPositionY()-size.height);

	//三个人物供选择
	for (int i = 0; i < 3; i++)
	{
		CImageView *selImg = (CImageView*)(m_selectLayer->getChildByTag(i+1));
		selImg->setEnabled(true);
		selImg->setTouchEnabled(true);
		selImg->setOnClickListener(this,ccw_click_selector(SelectHeroScene::onSelect));

		selImg->setAnchorPoint(ccp(0.5f, 0.5f));
		selImg->setPosition(ccp(selImg->getPositionX()+selImg->getScaleX()*selImg->getContentSize().width/2, selImg->getPositionY()+ selImg->getScaleY()*selImg->getContentSize().height/2));
		
		m_roleBaePos[i] = selImg->getPosition();
		m_roleScaleX[i] = selImg->getScaleX();
		m_roleScaleY[i] = selImg->getScaleY();
	}

	//确定人物选择
	CButton *ok = (CButton *)(m_InputLayer->findWidgetById("ok"));
	ok->setOnClickListener(this, ccw_click_selector(SelectHeroScene::onOk));

	CNetClient::getShareInstance()->registerMsgHandler(this);

	//背景渐隐渐现
	CCSprite *bgSpr = (CCSprite*)(m_ui->findWidgetById("bg"));
	CCFadeTo *out = CCFadeTo::create(1.2f,180);
	CCFadeTo *in = CCFadeTo::create(1.2f,255);
	bgSpr->runAction(CCRepeatForever::create(CCSequence::create(CCDelayTime::create(0.1f),out,in,nullptr)));


	//符文动画
	addFuwenAnimation("daofu01", ccp(100,640),0, 1);
	addFuwenAnimation("daofu01", ccp(900,690),-25, 1);
	addFuwenAnimation("daofu02", ccp(300,640),0, 1);
	addFuwenAnimation("daofu02", ccp(752,730),25, -1);
	addFuwenAnimation("daofu03", ccp(550,685),0, 1);
	addFuwenAnimation("daofu01", ccp(1000,640),-8, 1);
	addFuwenAnimation("daofu01", ccp(200,640),-20, -1);
	addFuwenAnimation("daofu01", ccp(900,640),10, -1);

	//第一次出现动画
	showFirstIn();

	//随机名称按钮
	CButton *randowName = (CButton*)(m_ui->findWidgetById("rand"));
	randowName->setTouchEnabled(true);
	randowName->getSelectedImage()->setScale(1.1f);
	randowName->setOnClickListener(this,ccw_click_selector(SelectHeroScene::onRandowName));

	//雾
	m_fogLay = (CLayout *)m_ui->findWidgetById("fog2");
	m_fogLay1 =(CLayout*)m_ui->findWidgetById("fog3");
	this->runCloudAction();
}


void SelectHeroScene::showBlackEffect( CCSprite* pSprite )
{
	CCSprite* pBlack = CCSprite::createWithTexture(pSprite->getTexture());
	pBlack->setScaleX(pSprite->getScaleX());
	pBlack->setScaleY(pSprite->getScaleY());
	pBlack->setPosition(pSprite->getPosition());
	pSprite->getParent()->addChild(pBlack, pSprite->getZOrder()+1);
	pBlack->setColor(ccc3(0, 0, 0));
	pBlack->setOpacity(0);
	pBlack->runAction(CCSequence::create(CCFadeIn::create(0.15f), CCFadeOut::create(0.1f),CCRemoveSelf::create(), nullptr));
}

void SelectHeroScene::showFirstIn()
{
	////UI界淡入
	//m_ui->setCascadeOpacityEnabled(true);
	//m_ui->setOpacity(0);
	//CCFadeTo *fade = CCFadeTo::create(2.5f,255);
	//m_ui->runAction(fade);

	m_bTouchLock = true;

	m_pClose->setVisible(false);

	CLayout *tipLab = (CLayout*)m_selectLayer->findWidgetById("tips");
	tipLab->setOpacity(0);

	//三个人物出场
	int iLine[3] = {1, 0, 2};
	for (int a = 0; a < 3;a++)
	{
		int i = iLine[a];
		CImageView *selImg = (CImageView*)(m_selectLayer->getChildByTag(i+1));
		CImageView *shadow = (CImageView*)(m_selectLayer->findWidgetById(CCString::createWithFormat("yin_%d", i+1)->getCString()));
		CImageView *tip = (CImageView*)(m_selectLayer->findWidgetById(CCString::createWithFormat("name%d", i+1)->getCString()));

		selImg->setPositionY(selImg->getPositionY()+640);
		selImg->runAction(CCSequence::create(
			CCDelayTime::create(0.15f*a),
			CCEaseSineIn::create(CCMoveBy::create(0.2f, ccp(0, -640))),
			nullptr));

		shadow->setOpacity(0);
		shadow->runAction(CCSequence::create(
			CCDelayTime::create(0.15f*a),
			CCEaseSineIn::create(CCFadeIn::create(0.2f)),
			nullptr));

		tip->setVisible(false);
		tip->runAction(CCSequence::create(
			CCDelayTime::create(0.6f+0.3f*a),
			CCCallFuncN::create(this, callfuncN_selector(SelectHeroScene::showNameAction)),
			nullptr));
	}

	this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.3f), CCCallFunc::create(this, callfunc_selector(SelectHeroScene::showFirstInCallBack))));
}

void SelectHeroScene::showFirstInCallBack()
{
	m_bTouchLock = false;

	//for(unsigned int i=0; i<3; i++)
	//{
	//	int iArmatureId = SelectHeroScene_role[i];
	//	CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(CCString::createWithFormat("BoneAnimation/%d.ExportJson", iArmatureId)->getCString());
	//}

	CLayout *tipLab = (CLayout*)m_selectLayer->findWidgetById("tips");
	tipLab->runAction(CCFadeIn::create(0.1f));
}


void SelectHeroScene::onRandowName(CCObject* pSender)
{
	string name = m_nameGm.getRandowName();
	m_textFieldName->setTextInput(name);
}

void SelectHeroScene::onOk(CCObject* ob)
{
	UserData* userData = DataCenter::sharedData()->getUser()->getUserData();
	userData->setRaceType(m_selectRaceType);
	
	if (strcmp(m_textFieldName->getTextInput()->c_str(),""))
	{
		//创建角色消息
		CNetClient::getShareInstance()->sendCreateRole(m_selectRaceType, m_textFieldName->getTextInput()->c_str());
	}
}

void SelectHeroScene::onSelect(CCObject* ob)
{
	if(m_bTouchLock) return;

	if(m_InputLayer->isVisible()) return;

	m_InputLayer->setVisible(true);

	PlayEffectSound(SFX_Button);

	CImageView *btn = (CImageView *)ob;
	int tag = btn->getTag();
	m_selectRaceType = tag;

	m_armatureEventData = m_armatureEventDataMgr->getArmatureEventData(SelectHeroScene_role[tag-1]);

	int rid = 0;


	int iArmatureId = SelectHeroScene_role[m_selectRaceType-1];
	CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(CCString::createWithFormat("BoneAnimation/%d.ExportJson", iArmatureId)->getCString());

	//更新id
	rid = SelectHeroScene_role[m_selectRaceType-1];

	//更新信息层
	updateInputLayer();

	//动画
	showHeroAnimation();

	//roleAnimaiton(tag+1, true);
}


void SelectHeroScene::selectLayerShowOne()
{
	int iIndex = m_selectRaceType-1;

	float scale[3] = {1.8f, 2.0f, 2.2f};
	CCPoint aimPos[3] = {ccp(310, 290), ccp(260, 250), ccp(300, 240)};
	//先其他所有UI元素（名字，人物，tip）
	//三个人物供选择
	for (int i = 0; i < 3; i++)
	{
		CImageView *role = (CImageView*)(m_selectLayer->getChildByTag(i+1));
		CImageView *shadow = (CImageView*)(m_selectLayer->findWidgetById(CCString::createWithFormat("yin_%d", i+1)->getCString()));
		CImageView *tip = (CImageView*)(m_selectLayer->findWidgetById(CCString::createWithFormat("name%d", i+1)->getCString()));
		shadow->runAction(CCFadeOut::create(0.2f));
		tip->runAction(CCFadeOut::create(0.2f));
		
		//选择好的人移动到目标为位置
		if(i == iIndex)
		{
			role->runAction(CCSequence::create(
				CCDelayTime::create(0.15f), 
				CCCallFunc::create(this, callfunc_selector(SelectHeroScene::showArmature)),
				CCSpawn::createWithTwoActions(CCScaleTo::create(0.3f, m_roleScaleX[i]*scale[i], m_roleScaleY[i]*scale[i]), CCMoveTo::create(0.3f, aimPos[i])), nullptr));
		}
		else
		{
			showBlackEffect(role);
			role->runAction(CCFadeOut::create(0.2f));
		}
	}

	CLayout *tipLab = (CLayout*)m_selectLayer->findWidgetById("tips");
	tipLab->runAction(CCFadeOut::create(0.2f));
	CCScale9Sprite *mask = (CCScale9Sprite*)m_ui->findWidgetById("mask");
	mask->setOpacity(0);
	mask->setVisible(true);
	mask->runAction(CCFadeTo::create(0.2f, 175));

	//动作回调后震屏，然后信息板块出现

}

void SelectHeroScene::selectLayerShowThree()
{
	int iIndex = m_selectRaceType-1;

	//先其他所有UI元素（名字，人物，tip）
	//三个人物供选择
	for (int i = 0; i < 3; i++)
	{
		CImageView *role = (CImageView*)(m_selectLayer->getChildByTag(i+1));
		CImageView *shadow = (CImageView*)(m_selectLayer->findWidgetById(CCString::createWithFormat("yin_%d", i+1)->getCString()));
		CImageView *tip = (CImageView*)(m_selectLayer->findWidgetById(CCString::createWithFormat("name%d", i+1)->getCString()));
		shadow->runAction(CCFadeIn::create(0.2f));
		tip->runAction(CCFadeIn::create(0.2f));

		//选择好的人移动到初始位置
		if(i == iIndex)
		{
			role->runAction(CCSequence::create(CCSpawn::createWithTwoActions(CCScaleTo::create(0.3f, m_roleScaleX[i], m_roleScaleY[i]), CCMoveTo::create(0.3f, m_roleBaePos[i])), nullptr));
		}
		else
		{
			role->runAction(CCFadeIn::create(0.2f));
		}
	}

	CLayout *tipLab = (CLayout*)m_selectLayer->findWidgetById("tips");
	tipLab->runAction(CCFadeIn::create(0.2f));
	CCScale9Sprite *mask = (CCScale9Sprite*)m_ui->findWidgetById("mask");
	mask->runAction(CCFadeOut::create(0.2f));

	runAction(CCSequence::create(CCDelayTime::create(0.3f), CCCallFunc::create(this, callfunc_selector(SelectHeroScene::hideHeroAnimationCallBack)),nullptr));
}


void SelectHeroScene::updateInputLayer()
{
	//描述文字
	CCSprite *descImg = (CCSprite *)m_InputLayer->findWidgetById("desc");
	descImg->setOpacity(0);
	{
		descImg->getParent()->removeChildByTag(911);
		CCSprite *newImg = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("selectRole/%d.png",m_selectRaceType)->getCString()));
		newImg->setPosition(descImg->getPosition());
		newImg->setTag(911);
		descImg->getParent()->addChild(newImg, descImg->getZOrder());
	}

	//左边类型
	CCSprite *fontImg = (CCSprite *)m_InputLayer->findWidgetById("fontRole");
	fontImg->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("selectRole/fontRole_%d.png",m_selectRaceType)->getCString()));
	fontImg->setVisible(false);

	//名字
	CCSprite * name = (CCSprite *)m_InputLayer->findWidgetById("name");
	name->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("selectRole/name_%d.png",m_selectRaceType)->getCString()));

	//球
	CCSprite * ball = (CCSprite *)m_InputLayer->findWidgetById("ball");
	ball->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("selectRole/ball_%d.png",m_selectRaceType)->getCString()));


	//角色特性
	CImageView *selImg;
	for (int i = 0; i < 3; i++)
	{
		CImageView *grapImg = (CImageView*)(m_InputLayer->getChildByTag(i+90));
		if (i+1==m_selectRaceType)
		{	
			grapImg->setVisible(true);
			selImg = grapImg;
		}
		else
		{
			grapImg->setVisible(false);
		}
	}

	selImg->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCFadeTo::create(0.6f, 155), CCFadeTo::create(0.4f, 255))));
}


void SelectHeroScene::showHeroAnimation()
{
	m_bTouchLock = true;

	//选人界面动画
	selectLayerShowOne();


	//动作回调后震屏，然后信息板块出现
}


void SelectHeroScene::onExit()
{
	CScene::onExit();
	//CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CNetClient::getShareInstance()->unRegisterMsgHandler(this);

	for(unsigned int i=0; i<3; i++)
	{
		int iArmatureId = SelectHeroScene_role[i];
		CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo(CCString::createWithFormat("BoneAnimation/%d.ExportJson", iArmatureId)->getCString());
		CCTextureCache::sharedTextureCache()->removeUnusedTextures();
	}

	CC_SAFE_RELEASE(m_armatureEventDataMgr);
}

bool SelectHeroScene::ProcessMsg(int type, google::protobuf::Message *msg)
{
	this->setVisible(true);

	switch (type)
	{
	case CreateResponseMsg:
		{
			CreateResponse *response = (CreateResponse*)msg;
			if (response->createrecode()==0)
			{
				for (int i= 0; i<response->rolelist_size(); i++)
				{
					protos::common::Role role =  response->rolelist(i);
					printf("id = %d ,name = %s",role.roleid(), role.rolename().c_str());
					CNetClient::getShareInstance()->sendInGameReq(role.roleid());
				    //CNetClient::getShareInstance()->sendRoleList(1);
					//CSceneManager::sharedSceneManager()->replaceScene(CCTransitionSplitRows::create(1.0f,GETSCENE(CityScene)));
					break;
				}
			}
			else
			{
				ShowPopTextTip(GETLANGSTR(1011));
			}
		}
		return true;
		//进入游戏
	case InGameResponseMsg:
		{
			InGameResponse *inRespon = (InGameResponse*)msg;
			protos::common::Role role = inRespon->myrole();
			printf("role id:%d  rolename:%s, rolelv: %d", role.roleid(), U8(role.rolename().c_str()),role.rolelv());

			UserData* userData = DataCenter::sharedData()->getUser()->getUserData();
			userData->setRaceType(role.rolenation());
			if(role.rolenation()==RaceType_1)
			{
				//userData->setAliveModel("hero6");
			}else if(role.rolenation()==RaceType_2)
			{
				//userData->setAliveModel("hero2");
			}else
			{
				//userData->setAliveModel("hero5");
			}
	
			userData->setAliveID(role.roleid());
			userData->setRoleID(role.roleid());
			userData->setLevel(role.rolelv());
			userData->setName(role.rolename().c_str());
			userData->setCoin(role.rolecoin());
			userData->setExp(role.roleexp());
			userData->setNextExp(role.nextexp());
			userData->setRoleAction(role.roleaction());
			userData->setRoleGold(role.rolegold());
// 			userData->setMapId(role.mapid());
			userData->read(role);

			DataCenter::sharedData()->getUser()->test();


#if CC_TARGET_PLATFORM != CC_PLATFORM_WIN32
			//战斗引导完了，弹框告知是否下载更新包
			if(CDownloadPackage::checkIsNeedPackage())
			{
				CPopTip *pop = CPopTip::create();
				LayerManager::instance()->pop();
				pop->setVisible(true);
				pop->addContentTip(GETLANGSTR(1156));
				pop->setTouchEnabled(true);
				pop->setTouchPriority(-100);
				pop->setButtonLisener(this, ccw_click_selector(SelectHeroScene::onClickDownloadPackage));
				CCDirector::sharedDirector()->getRunningScene()->addChild(pop, 899);
				return true;
			}
#endif

			CSceneManager::sharedSceneManager()->replaceScene(GETSCENE(LoadScene));
			
			return true;
		}
		break;
	default:
		break;
	}
	return false;
}

void SelectHeroScene::onClose(CCObject* ob)
{
	PlayEffectSound(SFX_Ensure);

	if(m_bTouchLock) return;

	
	hideHeroAnimation();

}

void SelectHeroScene::roleAnimaiton(int tag, bool isSelect)
{
	CCSprite *role1 = (CCSprite*)m_selectLayer->getChildByTag(1);
	CCSprite *role2 = (CCSprite*)m_selectLayer->getChildByTag(2);
	CCSprite *role3 = (CCSprite*)m_selectLayer->getChildByTag(3);

	CCSprite *yin1 = (CCSprite*)m_selectLayer->findWidgetById("yin_1");
	CCSprite *yin2 = (CCSprite*)m_selectLayer->findWidgetById("yin_2");
	CCSprite *yin3 = (CCSprite*)m_selectLayer->findWidgetById("yin_3");

	CCMoveBy *movl = CCMoveBy::create(0.4f,ccp(-VCENTER.x*2,0));
	
	if (isSelect)
	{
		CCMoveBy *moveLeft = CCMoveBy::create(0.7f,ccp(-VCENTER.x*2,0));
		CCMoveBy *moveRight = CCMoveBy::create(0.7f, ccp(VCENTER.x*2,0));
		role1->runAction(moveLeft);
		role3->runAction(moveRight);
		role2->setVisible(false);
		m_InputLayer->runAction(movl);
		yin2->setVisible(false);
		yin3->setVisible(false);
	}
	else
	{
		CCMoveBy *moveLeft = CCMoveBy::create(0.5f,ccp(-VCENTER.x*2,0));
		CCMoveBy *moveRight = CCMoveBy::create(0.5f, ccp(VCENTER.x*2,0));
		role1->runAction(moveRight);
		role3->runAction(moveLeft);
// 		role2->setVisible(true);
		m_InputLayer->runAction(movl->reverse());
		yin2->setVisible(true);
		yin3->setVisible(true);
	}

// 	m_selectLayer->findWidgetById()

}

void SelectHeroScene::addFuwenAnimation(char* animaName, const CCPoint& pos, float rotate, int zOrder)
{
	CCString *pngStr = CCString::createWithFormat("effAnim/%s/%s0.png",animaName,animaName);
	CCString *pngList = CCString::createWithFormat("effAnim/%s/%s0.plist",animaName,animaName);
	CCString *pngJson = CCString::createWithFormat("effAnim/%s/%s.ExportJson",animaName,animaName);

	CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(pngStr->getCString(),pngList->getCString(),pngJson->getCString());
	CCArmature *armature = CCArmature::create(animaName);

	armature->getAnimation()->playWithIndex(0,-1,-1,100);
	armature->setPosition(pos);
	armature->setZOrder(zOrder);
	armature->setRotation(rotate);
	m_ui->addChild(armature);

	if (zOrder>0)
	{
		armature->setScale(1.1f);
	}
}

void SelectHeroScene::runCloudAction()
{
	//云运动
	// 	CCSprite *cloud  =  (CCSprite *)(m_fogLay->findWidgetById("cloud"));

	float width = 1308;//cloud->getContentSize().width;

	CCMoveBy* move = CCMoveBy::create(60.0f,ccp(-width,0));
	CCMoveBy* moveb = CCMoveBy::create(60.0f,ccp(-width,0));
	CCSequence *sequence = CCSequence::create(move,CCCallFuncN::create(this,callfuncN_selector(SelectHeroScene::relocation)),moveb,nullptr);
	m_fogLay->runAction(CCRepeatForever::create(sequence));

	m_cloudPos = m_fogLay->getPosition();

	// 	CCSprite *cloud1  =  (CCSprite *)(m_fogLay->findWidgetById("cloud1"));
	m_cloudAPos = m_fogLay1->getPosition();

	CCMoveBy* move1 = CCMoveBy::create(120.0f,ccp(-width*2,0));
	CCSequence *sequence1 = CCSequence::createWithTwoActions(move1,CCCallFuncN::create(this,callfuncN_selector(SelectHeroScene::relocation)));
	m_fogLay1->runAction(CCRepeatForever::create(sequence1));
}

void SelectHeroScene::relocation(CCNode* pSender)
{
	CLayout *cloud = (CLayout*)pSender;
	float width = 1308;/*cloud->getContentSize().width;*/
	CCPoint pos; 
	if (strcmp(cloud->getId(),"fog2")==0)
	{
		pos = ccpAdd(m_cloudPos,ccp(width,0));
		cloud->setPosition(pos);
	}
	else
	{
		cloud->setPosition(m_cloudAPos);
	}
}

void SelectHeroScene::movementCallBack( CCArmature *armature, MovementEventType type, const char *data )
{
	switch (type)
	{
	case cocos2d::extension::START:
		{
			if(strcmp(data, "skill")==0)
			{
				schedule(schedule_selector(SelectHeroScene::updateFrame));
			}
		}
		break;
	case cocos2d::extension::COMPLETE:
		{
			if(strcmp(data, "skill")==0)
			{
				unschedule(schedule_selector(SelectHeroScene::updateFrame));
			}

			{
				//人物设置隐藏
				m_pArmature[m_selectRaceType-1]->runAction(CCFadeOut::create(0.2f));
				//震动
				showShake();
				CCSize winSize = CCDirector::sharedDirector()->getWinSize();
				//弹版块
				m_InputLayer->setVisible(true);
				m_InputLayer->runAction(CCSequence::createWithTwoActions(
					CCMoveBy::create(0.25f, ccp(0, winSize.height)), 
					CCCallFunc::create(this, callfunc_selector(SelectHeroScene::showHeroAnimationCallBack))));

				PlayEffectSound(SFX_425);
			}
		}
		break;
	case cocos2d::extension::LOOP_COMPLETE:
		break;
	default:
		break;
	}
}

void SelectHeroScene::showShake()
{
	m_ui->setScale(1.02f);
	m_ui->runAction(CCSequence::createWithTwoActions(CCShake::create(0.3f, 10.0f), CCCallFunc::create(this, callfunc_selector(SelectHeroScene::showShakeCallBack))));
}

void SelectHeroScene::showShakeCallBack()
{
	m_ui->setPosition(VCENTER);
	m_ui->setScale(1.0f);
}

void SelectHeroScene::showHeroAnimationCallBack()
{
	m_bTouchLock = false;

	m_pClose->setVisible(true);

	//CCNode* pNode = (CCNode*)m_ui->findWidgetById("fontRole");
	//pNode->setVisible(true);
	//showNameAction(pNode);
}

void SelectHeroScene::hideHeroAnimation()
{
	m_bTouchLock = true;

	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	m_pClose->setVisible(false);

	CCNode* pNode = (CCNode*)m_ui->findWidgetById("fontRole");
	pNode->setVisible(false);

	//震动
	showShake();

	//弹版块
	m_InputLayer->runAction(CCSequence::create(
		CCMoveBy::create(0.25f, ccp(0, -winSize.height)), 
		CCHide::create(),
		CCCallFunc::create(this, callfunc_selector(SelectHeroScene::selectLayerShowThree)),
		nullptr));

}


void SelectHeroScene::hideHeroAnimationCallBack()
{
	m_bTouchLock = false;
}

void SelectHeroScene::showNameAction( CCNode* pNode )
{
	pNode->setVisible(true);
	pNode->setScale(4.0);
	pNode->runAction(CCEaseBackOut::create(CCScaleTo::create(0.3f, 1.0f)));

	PlayEffectSound(SFX_425);
}

void SelectHeroScene::showArmature()
{
	int iIndex = m_selectRaceType-1;
	//然后人物出现，播放动作
	CCPoint armaturePos[3] = {ccp(776, 204), ccp(528, 204), ccp(638, 204)};
	float fScale[3] = {1.0f, 0.7f, 0.8f};
	if(m_pArmature[iIndex] == nullptr)
	{
		int iArmatureId = SelectHeroScene_role[iIndex];
		//CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(CCString::createWithFormat("BoneAnimation/%d.ExportJson", iArmatureId)->getCString());
		CCArmature* pArmautre = CCArmature::create(CCString::createWithFormat("%d", iArmatureId)->getCString());
		pArmautre->setPosition(armaturePos[iIndex]);
		pArmautre->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(SelectHeroScene::movementCallBack));
		pArmautre->setScale(fScale[iIndex]);
		m_ui->addChild(pArmautre, 999);
		m_pArmature[iIndex] = pArmautre;
	}
	m_pArmature[iIndex]->setOpacity(255);
	m_pArmature[iIndex]->setVisible(true);
	m_pArmature[iIndex]->getAnimation()->play("skill");

}

void SelectHeroScene::updateFrame( float dt )
{
	int iCurrentFrameIndex = m_pArmature[m_selectRaceType-1]->getAnimation()->getCurrentFrameIndex();
	if(m_lastFrame!=iCurrentFrameIndex)															//骨骼帧率与游戏帧率不同例如：游戏2帧骨骼才跑了1帧的情况(高效)
	{	
		for(unsigned int frameIndex = m_lastFrame+1; frameIndex<=iCurrentFrameIndex; frameIndex++)	//防止偶尔的掉帧情况出现处理
		{
			vector<ArmatureEvent*>& frameEvents = m_armatureEventData->getEventVector("skill", frameIndex);	//获取当前帧的所有事件
			for(unsigned int i=0; i<frameEvents.size(); i++)
			{
				const ArmatureEvent& armatureEvent = *(frameEvents.at(i));
				string sEventName = armatureEvent.name;
				if(sEventName.size()>0)
				{
					if(sEventName.compare(EVE_SOUND)			==0 )		
						PlaySound_Event(armatureEvent.sound);				//播放声音
				}
			}
		}
		m_lastFrame = iCurrentFrameIndex;
	}
}

void SelectHeroScene::onClickDownloadPackage( CCObject* pSender )
{
	CButton* pBtn = (CButton*)pSender;

	if(pBtn->getTag()==PopTipConfirm)
	{
		CDownloadPackage* pLayer = CDownloadPackage::create();
		pLayer->setDownloadPacakgeDelegate(this);
		CCDirector::sharedDirector()->getRunningScene()->addChild(pLayer, 1147);
		pLayer->downLoadPackage();
	}
	else
	{
		//不更新，重启游戏，重复引导
		//CJniHelper::getInstance()->restartGame();
		CCDirector::sharedDirector()->end();
	}

	((CPopTip*)(pBtn->getParent()->getParent()))->removeFromParentAndCleanup(true);
}

void SelectHeroScene::downloadPackageEnd( bool bAnswer )
{
	if(bAnswer)
	{
		CSceneManager::sharedSceneManager()->replaceScene(GETSCENE(LoadScene));
	}
}
