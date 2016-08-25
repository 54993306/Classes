#include "mainCityUI.h"
#include "common/RollLabel.h"
#include "model/DataCenter.h"
#include "common/color.h"
#include "Global.h"
#include "GMessage.h"
#include "common/ProgressLabel.h"
#include "tollgate/TollgateLayer.h"
#include "netcontrol/CPlayerControl.h"
#include "shop/ShopLayer.h"
#include "MainCityControl.h"
#include "bag/BagScene.h"
#include "task/CTaskLayer.h"
#include "mail/EmailLayer.h"
#include "mail/EmailControl.h"
#include "CCTK/scenemanager.h"
#include "hero/HeroControl.h"
#include "sign/SignLayer.h"
#include "roleImg/RoleInfo.h"
#include "friend/FriendLayer.h"
#include "vip/VipLayer.h"
#include "task/TaskControl.h"
#include "mainCity/BuyResource.h"
#include "activity/ActivityLayer.h"
#include "tollgate/TollgatePreview.h"
#include "Battle/BattleScene/LoadBattleResource.h"
#include "Battle/WarManager.h"
#include "Battle/AnimationManager.h"
#include "common/CGameSound.h"
#include "guide/GuideManager.h"
#include "common/ShaderDataHelper.h"
#include "activity/HttpLoadImage.h"
#include "sign/PopItem.h"
#include "common/CommonFunction.h"
#include "tools/ShowTexttip.h"
#include "tools/UICloneMgr.h"
#include "Global.h"
#include "Battle/BattleCenter.h"

#include "mail/EmailLayer.h"
#include "CAccessLayer.h"


bool CMainCityUI::init()
{
	if (BaseLayer::init())
	{
// 		MaskLayer* lay = MaskLayer::create("MaskLayer");
// 		LayerManager::instance()->push(lay);
		this->setIsShowBlack(false);
		m_ui = LoadComponent("MainCity.xaml");
		m_ui->setPosition(VCENTER);
		m_ui->setTag(1);
		this->addChild(m_ui);
		this->setVisible(true);
		return true;
	}
	return false;
}
using namespace BattleSpace;

void CMainCityUI::onEnter()
{
	BaseLayer::onEnter();

	UserData *user = DataCenter::sharedData()->getUser()->getUserData();
	
	//角色信息相对位置处理
	CLayout* headPart = (CLayout*)m_ui->findWidgetById("head_part");
	headPart->setPosition(m_ui->convertToNodeSpace(ccp(VLEFT+headPart->getContentSize().width/2, VTOP-headPart->getContentSize().height/2)));

	//显示角色名，等级，体力，金币
	CLabel *nameLabel = (CLabel*)headPart->getChildByTag(10);
	nameLabel->setString(user->getRoleName().c_str());
	nameLabel->setColor(RGB_ROLE_COLOR);	
	
	CLabel *level = (CLabel*)(headPart->findWidgetById("level"));
	level->setString(CCString::createWithFormat("Lv%d",user->getLevel())->getCString());
	
	CProgressBar *progressBar = (CProgressBar*)(headPart->getChildByTag(11));
	progressBar->setMaxValue(user->getNextExp());
	progressBar->startProgress(user->getExp(),0.3f);

	CLabelAtlas *vip_level = (CLabelAtlas*)(headPart->findWidgetById("level_vip"));
	vip_level->setString(ToString(user->getVip()));
	if (user->getVip()==0)
	{
		vip_level->setVisible(false);
		CLabelAtlas *vip_font = (CLabelAtlas*)(headPart->findWidgetById("vip_font"));
		vip_font->setVisible(false);
	}

	CImageView *headbg = (CImageView *)(headPart->findWidgetById("headbg"));
	headbg->setTouchEnabled(true);
	headbg->setOnClickListener(this, ccw_click_selector(CMainCityUI::onHeadImgBtn));

	CButton *button = nullptr;
	int i = 1;
	for (; i<=9; ++i)
	{
		CCNode *lay = dynamic_cast<CCNode*>( m_ui->getChildByTag(i));
		button = dynamic_cast<CButton*>(lay->getChildByTag(i));
// 			button->setEnabled(false);
		button->setOnClickListener(this, ccw_click_selector(CMainCityUI::onClickBtn));
		button->setSelectedTexture(((CCSprite*)(button->getNormalImage()))->getTexture());
		button->getSelectedImage()->setScale(1.1f);
		m_btnPos[i] = button->getPosition();
	}

	CCSprite* head = (CCSprite*)headPart->getChildByTag(15);

	if (user->getThumb()>0)
	{
		CCTexture2D *texture = CCTextureCache::sharedTextureCache()
		->addImage(CCString::createWithFormat("headIcon/%d.png", user->getThumb())->getCString());
		if (texture)
		{
			head->setTexture(texture);
		}
	}
	else
	{
		string fbName = user->getFbId()+".jpg";
		string fullName = CCFileUtils::sharedFileUtils()->fullPathForFilename(fbName.c_str());
		bool isFileExist = CCFileUtils::sharedFileUtils()->isFileExist(fullName);
		if(isFileExist)
		{
			CCSprite *headBg = (CCSprite *)(headPart->findWidgetById("headbg"));
			CCSprite *sp = CCSprite::create(fbName.c_str());
			CCSprite* spr = MakeFaceBookHeadToCircle(sp);
			spr->setPosition(headBg->getPosition());
			headPart->removeChild(head);
		    spr->setTag(15);
			headPart->addChild(spr);
		}
		else
		{
			HttpLoadImage::getInstance()->bindUiTarget(this);
			CCString *imgUrl = CCString::createWithFormat(FACEBOOKIMG_106,user->getFbId().c_str());
			HttpLoadImage::getInstance()->requestUrlImage(imgUrl->getCString(),user->getFbId().c_str());
		}
	}

	CImageView* vip = (CImageView*)headPart->findWidgetById("vip");
	vip->setTouchEnabled(true);
	vip->setOnClickListener(this,ccw_click_selector(CMainCityUI::onVip));

	//添加签到，充值等其他入口
	CAccessLayer* pAccess = CAccessLayer::create();
	this->addChild(pAccess, 99);

	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HERO,this,GameMsghandler_selector(CMainCityUI::updateRoleProperty));
	CSceneManager::sharedSceneManager()->addMsgObserver(TASK_NOTICE,this,GameMsghandler_selector(CMainCityUI::updateTaskNotice));
	CSceneManager::sharedSceneManager()->addMsgObserver(MAIL_NOTICE,this,GameMsghandler_selector(CMainCityUI::updateMailNotice));
	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_FUNCTIONOPEN,this,GameMsghandler_selector(CMainCityUI::updateFuctionOpen));
	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_GAMETIP,this,GameMsghandler_selector(CMainCityUI::updateGameTip));
	CSceneManager::sharedSceneManager()->addMsgObserver(SHOW_HEAD,this,GameMsghandler_selector(CMainCityUI::showHead));

	CCSprite *red = (CCSprite*)(m_ui->findWidgetById("redPoint"));

	CCSprite *mailPoint = (CCSprite*)(m_ui->findWidgetById("mailPoint"));

	if (user->getRoleAction()<user->getActionLimit())
	{
		this->schedule(schedule_selector(CMainCityUI::updateActionTime),user->getInterval()*60);
	}
    GetTcpNet->registerMsgHandler(FriendReqNumMsg,this,CMsgHandler_selector(CMainCityUI::processNetMsg));
	GetTcpNet->registerMsgHandler(CBExchangeMsg,this,CMsgHandler_selector(CMainCityUI::exchangeMsg));

	//正在引导，不自动弹窗
	if(user->getNewStep()==2||user->getNewStep()==17/*CGuideManager::getInstance()->getIsRunGuide()*/)
	{
		DataCenter::sharedData()->setCityActionType(CA_None);
	}
	else if(user->getNewStep()<=0 || user->getNewStep()>=100)
	{
		//自动弹签到
		runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.7f), CCCallFunc::create(this, callfunc_selector(CMainCityUI::autoShowSign))));
	}

	switch (DataCenter::sharedData()->getCityActionType())
	{
	case CA_Levelup:
		{
			CButton *btn = (CButton*)m_ui->getChildByTag(HeroInfo_Btn);
			btn->runAction(CCSequence::create(CCDelayTime::create(0.6f),
				CCCallFuncN::create(this, callfuncN_selector(CMainCityUI::onTimeWaitCityAction)),
				NULL));
			DataCenter::sharedData()->setCityActionType(CA_None);
		}
		break;
	case CA_GoToChapater:
		{
			CButton *btn = (CButton*)m_ui->getChildByTag(Battle_Btn);
			btn->runAction(CCSequence::create(CCDelayTime::create(0.2f),
				CCCallFuncN::create(this, callfuncN_selector(CMainCityUI::onTimeWaitCityAction)),
				NULL));
			m_bShowChapterFlag = true;
			DataCenter::sharedData()->setCityActionType(CA_None);
		}
		break;
	case CA_GoToStage:
		{
			this->runAction(CCSequence::create(CCDelayTime::create(0.8f),
				CCCallFunc::create(this, callfunc_selector(CMainCityUI::runTollgatepreviewCallBack)),
				NULL));
			DataCenter::sharedData()->setCityActionType(CA_None);
		}break;
	default:
		break;
	}

	//绑定场景隐藏和显示的消息
	NOTIFICATION->addObserver(this, callfuncO_selector(CMainCityUI::show), SHOW_MAIN_SCENE, nullptr);
	NOTIFICATION->addObserver(this, callfuncO_selector(CMainCityUI::hide), HIDE_MAIN_SCENE, nullptr);
	NOTIFICATION->addObserver(this, callfuncO_selector(CMainCityUI::checkShowActivity), "CheckShowActivity", nullptr);


	CCNode *lay = dynamic_cast<CCNode*>( m_ui->getChildByTag(6));
	button = dynamic_cast<CButton*>(lay->getChildByTag(6));
	CCAnimation *batAnim = AnimationManager::sharedAction()->getAnimation("9015");
	batAnim->setDelayPerUnit(0.1f);
	CCSprite *batLight = createAnimationSprite("skill/9015.png",button->getPosition(),batAnim,true);
	batLight->setScale(1.4f);
	m_ui->addChild(batLight);

	showNoticeTip(CTaskControl::getInstance()->getGameTips());
// 	CCSprite *spr = CCSprite::create("headImg/506.png");
// 	spr->setScale(1.2f);
// 	CCSprite *sp = maskedSprite(spr);
// 	CCSprite *headBg = (CCSprite *)(headPart->findWidgetById("headbg"));
// 	sp->setPosition(headBg->getPosition());
// 	headPart->addChild(sp);
}

void CMainCityUI::onVip(CCObject *pSender)
{
	CVipLayer *vipLayer= CVipLayer::create();
	if(LayerManager::instance()->push(vipLayer))
	{
		vipLayer->setOptionType(1);
		GetTcpNet->sendDataType(VipInfoMsg,true);
		GetTcpNet->sendDataType(VipShopMsg,true);
	}
}

void CMainCityUI::updateActionTime(float dt)
{
// 	UserData *data = DataCenter::sharedData()->getUser()->getUserData();
// 	if (data->getRoleAction() < data->getActionLimit())
// 	{
// 		data->setRoleAction(data->getRoleAction()+1);
// 		CLabel *action = (CLabel*)(m_ui->findWidgetById("action"));
// 		action->setString(CCString::createWithFormat("%d/%d",data->getRoleAction(),data->getActionLimit())->getCString());
// 		data->setActionTime(data->getActionTime() + data->getInterval()*60*60);
// 	}
// 	else
// 	{
// 		this->unschedule(schedule_selector(CMainCityUI::updateActionTime));
// 	}
}

void CMainCityUI::updateRoleProperty(const TMessage& tMsg)
{
	UserData *user = DataCenter::sharedData()->getUser()->getUserData();

	CLayout* headPart = (CLayout*)m_ui->findWidgetById("head_part");

	CLabel *nameLabel = (CLabel*)headPart->getChildByTag(10);
	nameLabel->setString(user->getRoleName().c_str());

	CLabel *level = (CLabel*)(headPart->findWidgetById("level"));
	level->setString(CCString::createWithFormat("Lv%d",user->getLevel())->getCString());

	CLabelAtlas *vip = (CLabelAtlas*)(headPart->findWidgetById("level_vip"));
	vip->setString(ToString(user->getVip()));

	if (user->getVip()==0)
	{
		vip->setVisible(false);
		CLabelAtlas *vip_font = (CLabelAtlas*)(headPart->findWidgetById("vip_font"));
		vip_font->setVisible(false);
	}

	CCSprite* head = (CCSprite*)headPart->getChildByTag(15);
	if (user->getThumb()>0)
	{
// 		CCTexture2D *texture = CCTextureCache::sharedTextureCache()
// 		->addImage(CCString::createWithFormat("headIcon/%d.png", user->getThumb())->getCString());
// 		if (texture)
// 		{
// 			head->setTexture(texture);
// 		}
// 		head->setFlipY(false);
		CCSprite *headBg = (CCSprite *)(headPart->findWidgetById("headbg"));
		CCString *strnName = CCString::createWithFormat("headIcon/%d.png", user->getThumb());
		CCSprite *sp = CCSprite::create(strnName->getCString());
		sp->setPosition(ccpAdd(headBg->getPosition(),ccp(0,10)));
		sp->setScale(0.75f);
		headPart->removeChild(head);
		sp->setTag(15);
		headPart->addChild(sp);
	}
	else
	{
		string fbName = user->getFbId()+".jpg";
		string fullName = CCFileUtils::sharedFileUtils()->fullPathForFilename(fbName.c_str());
		bool isFileExist = CCFileUtils::sharedFileUtils()->isFileExist(fullName);
		if(isFileExist)
		{
			CCSprite *headBg = (CCSprite *)(headPart->findWidgetById("headbg"));
			CCSprite *sp = CCSprite::create(fullName.c_str());
			CCSprite* spr = MakeFaceBookHeadToCircle(sp);
			spr->setPosition(headBg->getPosition());
			headPart->removeChild(head);
			spr->setTag(15);
			headPart->addChild(spr);
		}
		else
		{
			HttpLoadImage::getInstance()->bindUiTarget(this);
			CCString *imgUrl = CCString::createWithFormat(FACEBOOKIMG_106,user->getFbId().c_str());
			HttpLoadImage::getInstance()->requestUrlImage(imgUrl->getCString(),user->getFbId().c_str());
		}
	}
	
	CProgressBar *progressBar = (CProgressBar*)(headPart->getChildByTag(11));
	progressBar->setMaxValue(user->getNextExp());
	progressBar->startProgress(user->getExp(),0.3f);
}

void CMainCityUI::onExit()
{
	BaseLayer::onExit();
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HERO,this);
	CSceneManager::sharedSceneManager()->removeMsgObserver(TASK_NOTICE,this);
	CSceneManager::sharedSceneManager()->removeMsgObserver(MAIL_NOTICE,this);
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_FUNCTIONOPEN,this);
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_GAMETIP,this);
	CSceneManager::sharedSceneManager()->removeMsgObserver(SHOW_HEAD,this);

	GetTcpNet->unRegisterAllMsgHandler(this);
	NOTIFICATION->removeAllObservers(this);

	if (HttpLoadImage::getInstance()->getTarget()==this)
	{
		HttpLoadImage::getInstance()->bindUiTarget(NULL);
	}
}

void CMainCityUI::runMoveAction(int fromTag, int toTag,float moveTime)
{
	CCNode *sBtn =  m_ui->getChildByTag(fromTag);
	CButton *dBtn = dynamic_cast<CButton*>( m_ui->getChildByTag(toTag));

	CCArray *children = sBtn->getChildren();

	CCNode *child = nullptr;
	for (int i = 0; i < sBtn->getChildrenCount(); i++)
	{	
		child = (CCNode*)children->objectAtIndex(i);
		CCMoveBy *move = CCMoveBy::create(moveTime,ccp(dBtn->getPositionX() -child->getPositionX(),0));
		CCCallFuncN *hideCall = CCCallFuncN::create(this,callfuncN_selector(CMainCityUI::hideBtn));
		CCSequence *ccseque = CCSequence::create(move,hideCall,nullptr);
		child->runAction(ccseque);
	}
	m_moveX[fromTag-1] = dBtn->getPositionX() - child->getPositionX();
} 

void CMainCityUI::hideBtn(CCNode *pSender)
{
	CCNode *node = (CCNode*)pSender;
	node->setVisible(false);
}

void CMainCityUI::runMovePreAction(int tag)
{
	CCNode *sBtn = dynamic_cast<CCNode*>( m_ui->getChildByTag(tag));
	sBtn->setVisible(true);

	CCArray *children = sBtn->getChildren();
	for (int i = 0; i < sBtn->getChildrenCount(); i++)
	{	
		CCNode *child = (CCNode*)children->objectAtIndex(i);
		CCMoveBy *move = CCMoveBy::create(0.3f,ccp(-m_moveX[tag-1],0));
		child->runAction(move);
		child->setAnchorPoint(CCPointCenter);
		child->setVisible(true);
	}
}

void CMainCityUI::onHeadImgBtn(CCObject *pSender)
{
	PlayEffectSound(SFX_Button);

	CRoleInfo *roleInfo = CRoleInfo::create();
	roleInfo->setRoleInfoVal(DataCenter::sharedData()->getUser()->getUserData());
	LayerManager::instance()->push(roleInfo);
}

void CMainCityUI::onClickBtn(CCObject *pSender)
{
	int  tag = ((CButton*)pSender)->getTag();

	map<int,CCity>::iterator iter = m_cityMap.find(tag+20);
	if (iter!=m_cityMap.end()&&!iter->second.isOpen)
	{
		string str = GETLANGSTR(iter->second.cityId*1000);
		if (str=="")
		{
			str = GETLANGSTR(2000);
		}
		ShowPopTextTip(str.c_str());
		return;
	}

	switch (tag)
	{
	case Task_Btn:
		{
			CCSprite *red = (CCSprite*)(m_ui->findWidgetById("redPoint"));
			red->setVisible(false);

			CTaskLayer *task = CTaskLayer::create();
			LayerManager::instance()->push(task);
			CTaskControl::getInstance()->sendTaskList(1);//->sendDataType(TaskListMsg);
			CTaskControl::getInstance()->getGameTips()->taskTips = false;
		}
		break;
	
	case Bag_Btn:
		{
		     LayerManager::instance()->push(CBagLayer::create());
			 CPlayerControl::getInstance().sendRoleBag(1,true);
		}
		break;

	case Shop_Btn:
		{
			if( LayerManager::instance()->getLayer("CShopLayer") == nullptr )
			{
				CShopLayer *shpLayer = CShopLayer::create();
				shpLayer->setShopType(1);
				LayerManager::instance()->push(shpLayer);
				CMainCityControl::getInstance()->sendShopRequest(1);

				CCSprite *red = (CCSprite*)(m_ui->findWidgetById("shopPoint"));
				red->setVisible(false);
				CTaskControl::getInstance()->getGameTips()->shopTips = false;
			}
		}
		break;

	case Battle_Btn:
		{
			CTollgateLayer* pLayer = CTollgateLayer::create();
			LayerManager::instance()->push(pLayer);
			if(m_bShowChapterFlag)
			{
				m_bShowChapterFlag = false;

				//直接获取消息
				if(BattleManage->getNormal())
				{
					pLayer->setStory(true);
					pLayer->ProcessMsg(ChapterList, CNetClient::getShareInstance()->getSaveMsg(ChapterList));
					pLayer->selectChapter(BattleManage->getChapterCount(),BattleManage->getChapterIndex());
					DataCenter::sharedData()->setCityActionType(CA_None);
				}
				else
				{
					pLayer->setStory(false);
					pLayer->setLastChapter(BattleManage->getChapterIndex());
					CPlayerControl::getInstance().sendChapterList(1);
					GetTcpNet->sendStageList(BattleManage->getChapterIndex());
				}
				
			}
			else
			{
				CPlayerControl::getInstance().sendChapterList(0);
			}
			
// 			GetTcpNet->sendStageList(1);
		}
		break;

	case HeroInfo_Btn:
		{
			/*LayerManager::instance()->push(CHeroList::create());*/
			CHeroControl *heroctl = CHeroControl::create();
			LayerManager::instance()->push(heroctl);
			CPlayerControl::getInstance().sendHeroList(1);
			CCSprite *red = (CCSprite*)(m_ui->findWidgetById("heroPoint"));
			red->setVisible(false);
			CTaskControl::getInstance()->getGameTips()->heroTips = false;
 		}
		break;

	case Uion_Btn:
		{
		}
		break;

// 	case Mail_Btn:
// 		{
// 			CCSprite *mailPoint = (CCSprite*)(m_ui->findWidgetById("mailPoint"));
// 			mailPoint->setVisible(false);
// 
// 			CLabel *mailNum = (CLabel*)(m_ui->findWidgetById("mailNum"));
// 			mailNum->setVisible(false);
// 
// // 			CEmailControl *emailControl = CEmailControl::create();
// // 			LayerManager::instance()->push(emailControl);
// // 			GetTcpNet->sendDataType(MailListMsg);
// 
// 		}
// 		break;

	case Sign_Btn:
		{
			CCSprite *mailPoint = (CCSprite*)(m_ui->findWidgetById("mailPoint"));
			mailPoint->setVisible(false);
			CTaskControl::getInstance()->getGameTips()->mailTips = false;

			CMailLayer* pMail = CMailLayer::create();
			LayerManager::instance()->push(pMail);
			GetTcpNet->sendDataType(MailListMsg,true);
		}
		break;

	case Activity_Btn:
		{
			//提示红点和数量隐藏
			CCSprite *actPoint = (CCSprite*)(m_ui->findWidgetById("actPoint"));
			actPoint->setVisible(false);
			CTaskControl::getInstance()->getGameTips()->actTips = false;

			//弹出层
			CActivityLayer* pActivityLayer = CActivityLayer::create();
			LayerManager::instance()->push(pActivityLayer);

			//发送网络请求
			CActivityControl::getInstance()->askForActList(2);
			
			CTaskControl::getInstance()->getGameTips()->actTips = false;

			CCLOG("Activity btn touched!");
		}
		break;


	case Friend_Btn:
		{
			CFriendLayer *friendlayer = CFriendLayer::create();
			LayerManager::instance()->push(friendlayer);
			CPlayerControl::getInstance().sendFriendList(1);

			CCSprite *frdPoint = (CCSprite*)(m_ui->findWidgetById("friendPoint"));
			frdPoint->setVisible(false);
			
		}
		break;
	default:
		break;
	}
	setShowHeadPart(false);
}

void CMainCityUI::hideLabel(CCNode *node)
{
	node->setVisible(false);
}

void CMainCityUI::changeLabel(CLabel * label, int coin)
{
	label->setVisible(true);
	label->setString(ToString(coin));
	CProgressLabel *prolab = CProgressLabel::create();
	prolab->setLabel(label);
	prolab->changeValueTo(0,0.5f);
	this->addChild(prolab);

	CCDelayTime *delay = CCDelayTime::create(0.5f);
	CCCallFuncN *call = CCCallFuncN::create(this,callfuncN_selector(CMainCityUI::hideLabel));
	label->runAction(CCSequence::createWithTwoActions(delay,call));
}

void CMainCityUI::updateTaskNotice(const TMessage& tMsg)
{
	if (tMsg.nMsg>0)
	{
		CCSprite *red = (CCSprite*)(m_ui->findWidgetById("redPoint"));
		red->setVisible(true);
	}
}

void CMainCityUI::showHead(const TMessage& tMsg)
{
	setShowHeadPart(true);
}

void CMainCityUI::updateMailNotice(const TMessage& tMsg)
{
	if (tMsg.nMsg>0)
	{
		CCSprite *mailPoint = (CCSprite*)(m_ui->findWidgetById("mailPoint"));
		mailPoint->setVisible(true);
	}
}

void CMainCityUI::processNetMsg(int type, google::protobuf::Message *msg)
{
	if (type==FriendReqNumMsg)
	{
		FriendRes *res = (FriendRes*)msg;
		CCSprite *frdPoint = (CCSprite*)(m_ui->findWidgetById("friendPoint"));
		frdPoint->setVisible(true);
	}
}

void CMainCityUI::onAddResourceBtn(CCObject* pSender)
{
	CButton* btn = (CButton*)pSender;
	if (!strcmp(btn->getId(),"btn_add_gold"))
	{
		CVipLayer *vipLayer= CVipLayer::create();
		if(LayerManager::instance()->push(vipLayer))
		{
			GetTcpNet->sendDataType(VipInfoMsg,true);
			GetTcpNet->sendDataType(VipShopMsg,true);
		}
	}
	
	
// 	BuyResource* bufLayer = BuyResource::create();
// 	if (outPutERRORMsg("CMainCityUI::onAddResourceBtn",bufLayer))return;
// 	if (!strcmp(btn->getId(),"btn_add_food"))
// 	{
// 		bufLayer->setExchangeType(BUY_FOOD);
// 		LayerManager::instance()->push(bufLayer);
// 	}else if (!strcmp(btn->getId(),"btn_add_copper"))
// 	{
// 		bufLayer->setExchangeType(BUY_COIN);
// 		LayerManager::instance()->push(bufLayer);
// 	}else if (!strcmp(btn->getId(),"btn_add_soul"))
// 	{
// // 		bufLayer->initByType(buy_soul);
// 		LayerManager::instance()->push(bufLayer);
// 	}else if (!strcmp(btn->getId(),"btn_add_gold"))
// 	{
// 		LayerManager::instance()->pop();
// 	}else{
// 		LayerManager::instance()->pop();
// 		CCLOG("[ *ERROR ] CMainCityUI::onAddResourceBtn");
// 	}
}

void CMainCityUI::onTimeWaitCityAction(CCNode* pSender){
	CButton *btn = (CButton*)pSender->getChildByTag(pSender->getTag());
	if (btn != NULL)
	{
		btn->executeClickHandler(btn);
	}
}

void CMainCityUI::updateOpenState(CityData *cityData)
{ 
	int size = cityData->cityList.size();
	for (int i = 0; i <size; i++)
	{
		CCity *city = &cityData->cityList.at(i);
		//是否刚打开功
		bool justOpen = false;
		if (city->cityId>20)
		{
			justOpen = CMainCityControl::getInstance()->isJustOpen(city);
		}
		if ((city->cityId>20&&!city->isOpen)||justOpen)
		{
			CCNode *child = m_ui->getChildByTag(city->cityId-20);
			if (child)
			{
				CButton *btn = dynamic_cast<CButton*>(child->getChildByTag(city->cityId-20));
				if (btn)
				{
					//btn->setEnabled(false);
					btn->getNormalImage()->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
					btn->getSelectedImage()->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
				}	
			}
		}
		m_cityMap[city->cityId] = *city;
	}
}

void CMainCityUI::updateFuctionOpen(const TMessage& tMsg)
{
	int tag = tMsg.nMsg;
	if (tag>20)
	{	
		CCNode *child = m_ui->getChildByTag(tag-20);
		if (child)
		{
			CButton *btn = dynamic_cast<CButton*>(child->getChildByTag(tag-20));
			if (btn)
			{
				btn->setEnabled(true);
				btn->getNormalImage()->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderDefault));
				btn->getSelectedImage()->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderDefault));
			}	
		}
	}
}

void CMainCityUI::show( CCObject* pObj )
{
	setVisible(true);
}

void CMainCityUI::hide( CCObject* pObj )
{
	setVisible(false);
}

void CMainCityUI::updateGameTip(const TMessage& tMsg)
{
	CGameTips *ct = CTaskControl::getInstance()->getGameTips();
	showNoticeTip(ct);
}

void CMainCityUI::showNoticeTip(CGameTips * ct)
{
	if (ct->heroTips)
	{
		CCSprite *red = (CCSprite*)(m_ui->findWidgetById("heroPoint"));
		red->setVisible(true);
	}
	if (ct->mailTips)
	{
		CCSprite *mailPoint = (CCSprite*)(m_ui->findWidgetById("mailPoint"));
		mailPoint->setVisible(true);
	}
	if (ct->taskTips)
	{
		CCSprite *red = (CCSprite*)(m_ui->findWidgetById("redPoint"));
		red->setVisible(true);
	}

	if (ct->shopTips)
	{
		CCSprite *red = (CCSprite*)(m_ui->findWidgetById("shopPoint"));
		red->setVisible(true);
	}
	if (ct->actTips)
	{
		CCSprite *red = (CCSprite*)(m_ui->findWidgetById("actPoint"));
		red->setVisible(true);
	}
}

void CMainCityUI::runTollgatepreviewCallBack()
{
	int stage = BattleManage->getStageIndex(); 

	CTollgatePreview *preview = CTollgatePreview::create();
	LayerManager::instance()->push(preview);
	CPlayerControl::getInstance().sendStageInfo(stage);
	preview->setStage(stage, "");
	preview->setNormal(BattleManage->getNormal());
}

void CMainCityUI::setShowHeadPart(bool isSHow)
{
	CLayout *head_part = (CLayout*)m_ui->findWidgetById("head_part");
	head_part->setVisible(isSHow);
}

void CMainCityUI::imageLoadSuccessCallBack(string sTag, vector<char>* pBuffer)
{
	CCImage* img = new CCImage;
	img->initWithImageData((unsigned char*)pBuffer->data(), pBuffer->size());
	CCTexture2D* texture = new CCTexture2D();
    texture->initWithImage(img);
	CLayout* headPart = (CLayout*)m_ui->findWidgetById("head_part");

	CCSprite* head = (CCSprite*)headPart->getChildByTag(15);
//	head->removeFromParent();
	//head->setTexture(texture);

	string path = HttpLoadImage::getInstance()->getStoragePath("download/fbImg",sTag.c_str())+".jpg";
	string buff(pBuffer->begin(), pBuffer->end());
	CCLOG("path: %s", path.c_str());
	FILE *fp = fopen(path.c_str(), "wb+");
	fwrite(buff.c_str(), 1, pBuffer->size(),  fp);
	fclose(fp);

//	CCSprite *headbg = (CCSprite*)(m_ui->findWidgetById("headbg"));
// 	CCClippingNode *clip = CCClippingNode::create();	
// 	CCSprite *fbimg = CCSprite::createWithTexture(texture);
// 	clip->addChild(fbimg);
// 
// 	clip->setPosition(headbg->getPosition());
// 	clip->setContentSize(CCSizeMake(110,110));
// 
// 	static ccColor4F red ={1,0,1};
// 	float radius = 106;
// 	const int nCount = 200;
// 	const float angle = 2.0f* (float)M_PI/nCount;
// 	CCPoint circle[nCount];
// 	for(int i=0; i<nCount; i++)
// 	{
// 		float radian = i*angle;
// 		circle[i].x = radius * cosf(radian);
// 		circle[i].y = radius * sinf(radian); 
// 	}
// 	CCDrawNode *stencil= CCDrawNode::create();
// 	stencil->drawPolygon(circle,nCount,red,0,red);
// 	clip->setStencil(stencil);	
// 	m_ui->addChild(clip);
	CCSprite *headBg = (CCSprite *)(headPart->findWidgetById("headbg"));
	CCSprite *headSpr = MakeFaceBookHeadToCircle(CCSprite::createWithTexture(texture));
	headSpr->setTag(15);
	headSpr->setPosition(headBg->getPosition());
	headPart->addChild(headSpr);
	head->removeFromParent();
	img->release();
}

void CMainCityUI::autoShowSign()
{
	if(DataCenter::sharedData()->getUser()->getUserData()->getFirstLogin())
	{
		CSignLayer *sign = CSignLayer::create();
		LayerManager::instance()->push(sign);
		GetTcpNet->sendDataType(SignDataMsg,true);
	}
}


void CMainCityUI::exchangeMsg(int type, google::protobuf::Message *msg)
{
	CardExchangeRes *res= (CardExchangeRes*)msg;
	int  ret = res->result();
	if (ret==1)
	{
		CGetPrizeRes prizers;
		prizers.result =1;
		for (int i = 0; i < res->prize_list_size(); i++)
		{  
			CPrize prize;
			prize.read(res->prize_list(i));
			prizers.prizeList.push_back(prize);
		}

		//弹框
		CPopItem *popItem = CPopItem::create();
		LayerManager::instance()->push(popItem);
		popItem->popPrizeRes(&prizers);

		//收集货币
		collectMoneyFromPrize(prizers);

	}
	//2 兑换码已使用，3 已兑换过该奖励，4 账号密码错误
	else if (ret==2)
	{
		ShowPopTextTip(GETLANGSTR(281));
	}
	else if (ret==3)
	{
		ShowPopTextTip(GETLANGSTR(282));
	}
	else if (ret==4)
	{
		ShowPopTextTip(GETLANGSTR(165));
	}
}

void CMainCityUI::checkShowActivity( CCObject* pObj )
{
	if(DataCenter::sharedData()->getUser()->getUserData()->getFirstLogin())
	{
		CButton *btn = (CButton*)m_ui->getChildByTag(Activity_Btn);
		btn->runAction(CCCallFuncN::create(this, callfuncN_selector(CMainCityUI::onTimeWaitCityAction)));
		DataCenter::sharedData()->getUser()->getUserData()->setFirstLogin(false);
	}
}