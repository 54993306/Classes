#include "FriendLayer.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "mainCity/QuickProduct.h"
#include "tools/UICloneMgr.h"
#include "model/DataCenter.h"
#include "GMessage.h"

#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "Battle/AnimationManager.h"
#include "common/color.h"

#include "tools/StringUtil.h"
#include "common/MultLanguage.h"
#include "scene/CPopTip.h"
#include "platform/platform.h"
#include "common/CommonFunction.h"
#include "sdk/FaceBookSDK.h"
#include "jsonCpp/json.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "mainCity/PanelTips.h"

#define  FACEBOOKIMG "http://graph.facebook.com/%s/picture?width=92&height=92"

CFriendLayer::~CFriendLayer()
{
	CC_SAFE_RELEASE(m_cell);
}


bool CFriendLayer::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("FriendMaskLayer");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);
		
		m_ui = LoadComponent("Friend.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		m_selectType = 1;
		return true;
	}
	return false;
}

void CFriendLayer::onEnter()
{ 
	BaseLayer::onEnter();

	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CFriendLayer::onClose));
	this->addChild(pClose, 999);

	m_cell = (CLayout*)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
	m_ui->removeChild(m_cell);
	m_cell1 = (CLayout*)(m_ui->findWidgetById("Cell1"));
	m_cell1->retain();
	m_ui->removeChild(m_cell1);
	m_cell2 = (CLayout*)(m_ui->findWidgetById("Cell2"));
	m_cell2->retain();
	m_ui->removeChild(m_cell2);


	m_friendTable = (CTableView *)(m_ui->findWidgetById("scroll"));
	m_friendTable->setDirection(eScrollViewDirectionVertical);
	m_friendTable->setSizeOfCell(m_cell->getContentSize());
	m_friendTable->setCountOfCell(0);
	m_friendTable->setBounceable(false);
	m_friendTable->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CFriendLayer::friendDataSource));
	m_friendTable->reloadData();

	m_recommendTable = (CTableView *)(m_ui->findWidgetById("scroll1"));
	m_recommendTable->setDirection(eScrollViewDirectionVertical);
	m_recommendTable->setSizeOfCell(m_cell1->getContentSize());
	m_recommendTable->setCountOfCell(0);
	m_recommendTable->setBounceable(false);
	m_recommendTable->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CFriendLayer::recomFriendDataSource));
	m_recommendTable->reloadData();

	m_acceptTable = (CTableView *)(m_ui->findWidgetById("scroll2"));
	m_acceptTable->setDirection(eScrollViewDirectionVertical);
	m_acceptTable->setSizeOfCell(m_cell2->getContentSize());
	m_acceptTable->setCountOfCell(0);
	m_acceptTable->setBounceable(false);
	m_acceptTable->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CFriendLayer::friendDataSource));
	m_acceptTable->reloadData();

	m_friendView = (CLayout *)m_ui->findWidgetById("friend");
	m_friendView->setVisible(true);

	m_recommendView = (CLayout*)m_ui->findWidgetById("recomm");
	m_recommendView->setVisible(false);
	m_acceptView = (CLayout *)m_ui->findWidgetById("apply");
	m_acceptView->setVisible(false);
	
	CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_ui->getChildByTag(10);
	for (int i = 0; i < 3; i++)
	{
		CRadioButton *radioBtn= (CRadioButton*)(radioGroup->getChildByTag(i+1));
		radioBtn->setOnCheckListener(this,ccw_check_selector(CFriendLayer::onSwitchBtn));
	}

	
	GetTcpNet->registerMsgHandler(FriendListMsg,this,CMsgHandler_selector(CFriendLayer::friendListRes));
	GetTcpNet->registerMsgHandler(AddFriendMsg,this,CMsgHandler_selector(CFriendLayer::processNetMsg));
    GetTcpNet->registerMsgHandler(DeleteFriendMsg,this,CMsgHandler_selector(CFriendLayer::processNetMsg));
	GetTcpNet->registerMsgHandler(AcceptFriendMsg,this,CMsgHandler_selector(CFriendLayer::processNetMsg));

	m_textFieldName = CursorTextField::textFieldWithPlaceHolder(GETLANGSTR(21), FONT_NAME, 15,CCSizeMake(300,35),ccWHITE); 
	CCNode *node = (CCNode *)m_ui->findWidgetById("searchbg");
	m_textFieldName->setPosition(ccp(node->getPositionX()-165,node->getPositionY()));
	m_textFieldName->setLimitNum(15);
	m_textFieldName->setPriority(this->getTouchPriority());
	m_textFieldName->setAnchorPoint(ccp(0,0.5));
	m_ui->addChild(m_textFieldName);
	m_textFieldName->setVisible(true);

	CButton *search = dynamic_cast<CButton*>(m_ui->findWidgetById("search"));
	search->setOnClickListener(this,ccw_click_selector(CFriendLayer::onSearch));
	showSelectRadioImg(2);
//	this->schedule(schedule_selector(CFriendLayer::onUpdateSearch),2.0f);

// 	CCPoint destPos = m_ui->getPosition();
// 	m_ui->setPositionY(destPos.y + VCENTER.y);
// 	CCMoveTo *move = CCMoveTo::create(0.5f,destPos);
// 	CCEaseBounceOut *out = CCEaseBounceOut::create(move);
// 	m_ui->runAction(out);
	CLayout *inviteLay = (CLayout*)(m_ui->findWidgetById("invite"));
	CButton *invitbtn = (CButton*)inviteLay->findWidgetById("invitbtn");
	invitbtn->setOnClickListener(this,ccw_click_selector(CFriendLayer::onInviteFriend));
	UserData *user = DataCenter::sharedData()->getUser()->getUserData();
	CProgressBar *fbProgress = (CProgressBar*)(inviteLay->findWidgetById("progress"));
	fbProgress->setMaxValue(100);
	fbProgress->setValue(user->getInviteFriend());
	CLabel *num = (CLabel*)(inviteLay->findWidgetById("num"));
	num->setString(CCString::createWithFormat("%d/%d",user->getInviteFriend(),100)->getCString());

	GetTcpNet->registerMsgHandler(AccountBinMsg,this,CMsgHandler_selector(CFriendLayer::accountBinRes));
	bool isFbLogin = DataCenter::sharedData()->getUser()->getUserData()->getIsFBLogin();

	HttpLoadImage::getInstance()->bindUiTarget(this);
}

void CFriendLayer::onUpdateSearch(float dt)
{
	if (strcmp(m_textFieldName->getTextInput()->c_str(),"")==0&&m_textStr!="")
	{
		m_ui->removeChildByTag(100);
		if (m_selectType==1)
		{
			m_friendView->setVisible(true);
		}
		else
		{
			m_recommendView->setVisible(true);
		}
	}
	m_textStr = *m_textFieldName->getTextInput();
}

void CFriendLayer::onSearch(CCObject* pSender)
{
	if (strcmp(m_textFieldName->getTextInput()->c_str(),""))
	{	
		CPlayerControl::getInstance().sendFriendList(3,m_textFieldName->getTextInput()->c_str());
		m_isSearch = true;
	}
}

void CFriendLayer::onSwitchBtn(CCObject *pSender, bool bChecked)
{
	CRadioButton *btn = (CRadioButton*)pSender;
	int selIndex = 2*btn->getTag();
	m_isSearch = false;
	if (bChecked)
	{
		PlayEffectSound(SFX_Button);

		showFriendTip(false, nullptr);

		m_selectType = btn->getTag();
		m_friendView->setVisible(false);
		m_recommendView->setVisible(false);
		m_acceptView->setVisible(false);
		CLayout *inviteLay = (CLayout*)(m_ui->findWidgetById("invite"));
		inviteLay->setVisible(false);
		showSelectRadioImg(selIndex);
		CLayout *find = (CLayout*)(m_ui->findWidgetById("find"));
		find->setVisible(btn->getTag()==1);
		m_textFieldName->setVisible(btn->getTag()==1);

		if (btn->getTag()==1)
		{
			CPlayerControl::getInstance().sendFriendList(1);
			m_friendView->setVisible(true);
		}
		else if (btn->getTag()==2)
		{
			CPlayerControl::getInstance().sendFriendList(3);
			m_recommendView->setVisible(true);
		}
		else if(btn->getTag()==3)
		{				
			inviteLay->setVisible(true);
		}
	}
}

void CFriendLayer::showSelectRadioImg(int selIndex)
{
	for (int i=1; i<=4; ++i)
	{
		CCNode *spr = nullptr;
		if (i%2==0)
		{
			spr= (CCNode*)m_ui->findWidgetById(CCString::createWithFormat("switch%d",i)->getCString());
			spr->setVisible(true);
		}
		else
		{
			spr = (CCNode*)m_ui->findWidgetById(CCString::createWithFormat("switch%d",i)->getCString());
			spr->setVisible(false);
		}
		if (selIndex ==i)
		{
			spr->setVisible(false);
		}
		else if (selIndex==i+1)
		{
			spr->setVisible(true);
		}
	}
}


CCObject* CFriendLayer::friendDataSource(CCObject* pConvertCell, unsigned int uIdx)
{
	CTableViewCell *pCell = (CTableViewCell*)(pConvertCell);
	if (!pCell)
	{
		pCell = new CTableViewCell;
		pCell->autorelease();
		addTableCell(uIdx, pCell);
	}
	else
	{
		pCell->removeAllChildren();
		addTableCell(uIdx, pCell);
	}
	return pCell;
}

CCObject* CFriendLayer::recomFriendDataSource(CCObject* pConvertCell, unsigned int uIdx)
{
	CTableViewCell *pCell = (CTableViewCell*)(pConvertCell);
	if (!pCell)
	{
		pCell = new CTableViewCell;
		pCell->autorelease();
		addTableCell(uIdx, pCell);
	}
	else
	{
		pCell->removeAllChildren();
		addTableCell(uIdx, pCell);
	}
	return pCell;
}

void CFriendLayer::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

//邀请码 roleid+10000
void CFriendLayer::onInviteFriend(CCObject* pSender)
{
	bool isFbLogin = DataCenter::sharedData()->getUser()->getUserData()->getIsFBLogin();
// 	if (!isFbLogin)
// 	{
// 		FaceBookSDK::sharedInstance()->openAuthor();
// 		FaceBookSDK::sharedInstance()->setCallbackForUserInfo(this,callfuncO_selector(CFriendLayer::onFaceBookUserInfo));
// 	}
// 	else
	{
		FaceBookSDK::sharedInstance()->onOpenPickFriends();
	}	
}

void CFriendLayer::onFaceBookUserInfo(CCObject *object)
{
	CCString *strResult=dynamic_cast<CCString *>(object);
	if(strResult!=NULL)
	{
		Json::Value jsValue;
		if(Json::Reader().parse(strResult->getCString(),jsValue))
		{
			string m_FaceBookUserID =jsValue["id"].asCString();
			CPlayerControl::getInstance().sendAccountBind(m_FaceBookUserID.c_str());
		}
	}
}

void CFriendLayer::onExit()
{
	GetTcpNet->unRegisterAllMsgHandler(this);
	CSceneManager::sharedSceneManager()->removeMsgObserver("updateHero",this);
	BaseLayer::onExit();
// 	NOTIFICATION->postNotification(SHOW_MAIN_SCENE);
	FaceBookSDK::sharedInstance()->setCallbackForUserInfo(nullptr,nullptr);
	CSceneManager::sharedSceneManager()->PostMessageA(SHOW_HEAD,0,nullptr,nullptr);
	HttpLoadImage::getInstance()->bindUiTarget(nullptr);
}

void CFriendLayer::onDeleteFriend(CCObject* pSender)
{
	CButton *btn = (CButton*)pSender;
	m_selectFriend = (CFriend*)btn->getUserData();
	ShowConfirmTextTip(GETLANGSTR(24),this,ccw_click_selector(CFriendLayer::onComfirmDeleteFriend));
}

void CFriendLayer::onComfirmDeleteFriend(CCObject* pSender)
{
	CButton *btn = (CButton*)pSender;
	if (btn->getTag()==PopTipConfirm)
	{
		CPlayerControl::getInstance().sendDeleteFriend(m_selectFriend->friendId);
	}
	((CPopTip*)(btn->getParent()->getParent()))->onClose(nullptr);
}

void CFriendLayer::onAddFriend(CCObject* pSender)
{
	PlayEffectSound(SFX_Button);

	CButton *btn = (CButton*)pSender;
	m_selectFriend = (CFriend*)btn->getUserData();
	CPlayerControl::getInstance().sendAddFriend(m_selectFriend->friendId);
}

void CFriendLayer::onAcceptFriend(CCObject* pSender)
{
	CButton *btn = (CButton*)pSender;
	m_selectFriend = (CFriend*)btn->getUserData();
	CPlayerControl::getInstance().sendAcceptFriend(m_selectFriend->friendId,btn->getTag()==10);
}

void CFriendLayer::processNetMsg(int type, google::protobuf::Message *msg)
{
	//(47)请求添加好友的操作结果(1 请求已发送，2 好友上限，3 好友已存在，4 好友不在线，5 数据错误)
	//(48)申请加为好友的请求数(服务器推送 retval = 请求数量)
	//(49)同意、拒绝添加请求(1 操作成功，2 请求已失效，3 好友上限，4 数据错误)
	//(63)删除好友结果(1 删除成功，2 数据错误)
	this->setVisible(true);

	FriendRes *res = (FriendRes *)msg;
	switch (type)
	{
	case AddFriendMsg:
		{
			if (m_isSearch)
			{
				m_ui->removeChildByTag(100);
				if (m_selectType==1)
				{
					m_friendView->setVisible(true);
				}
				else if (m_selectType==2)
				{
					m_recommendView->setVisible(true);
				}			
			}
			switch (res->retval())
			{
				
			case 1:
				{
					if (m_selectType==1&&m_selectFriend)
					{
						m_isSearch = false;
						FriendMap::iterator iter = m_friendListMap.find(m_selectType);
						if (iter!=m_friendListMap.end())
						{
							iter->second.insert(iter->second.begin(),*m_selectFriend);
							m_friendTable->setCountOfCell(m_friendTable->getCountOfCell()+1);
							m_friendTable->reloadData();
						}		
					}
					else if (m_selectType==2&&m_selectFriend)
					{
						removeCell(m_selectType);
					}

					showFriendTip(m_recommendTable->getCountOfCell()<=0, GETLANGSTR(1237));

				}
				break;
			case 2:
				ShowPopTextTip(GETLANGSTR(168));
				break;
			case 3:
				ShowPopTextTip(GETLANGSTR(169));
				break;
			case 4:
				ShowPopTextTip(GETLANGSTR(170));
				break;
			default:
				break;
			}
			m_isSearch = false;
		}
		break;
	case DeleteFriendMsg:
		{
			switch (res->retval())
			{
			case 1:
				if (m_selectType==1&&m_selectFriend)
				{
					removeCell(m_selectType);
					showFriendTip(m_friendTable->getCountOfCell()<=0, GETLANGSTR(1236));
				}
				break;
			case 2:
				ShowPopTextTip(GETLANGSTR(171));
				break;
			default:
				break;
			}
		}
		break;
	case AcceptFriendMsg:
		{
			//(1 操作成功，2 请求已失效，3 好友上限，4 数据错误)
			switch (res->retval())
			{
			case 1:
				ShowPopTextTip(GETLANGSTR(171));
				if (m_selectType==3&&m_selectFriend)
				{
					removeCell(m_selectType);
					showFriendTip(m_acceptTable->getCountOfCell()<=0, GETLANGSTR(1236));
				}
				break;
			case 2:
				ShowPopTextTip(GETLANGSTR(172));
				break;
			case 3:
				ShowPopTextTip(GETLANGSTR(173));
				break;
			case 4:
				ShowPopTextTip(GETLANGSTR(170));
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}
}

void CFriendLayer::addTableCell(unsigned int uIdx, CTableViewCell * pCell)
{
	if (m_isSearch)
	{
		addFriendCell(m_cell1,uIdx,pCell);
	}
	else if (m_selectType==1)
	{
		addFriendCell(m_cell,uIdx,pCell);
	}
	else if (m_selectType==2)
	{
		addFriendCell(m_cell1,uIdx,pCell);
	}
	else 
	{
		addAcceptCell(m_cell2,uIdx,pCell);
	}
}

void CFriendLayer::addFriendCell(CLayout* layCell, unsigned int uIdx, CTableViewCell * pCell)
{
	CLayout *lay = UICloneMgr::cloneLayout(layCell);
	FriendMap::iterator iter ;
	
	if (m_isSearch)
	{
		iter= m_friendListMap.find(4);
	}
	else
	{
		iter= m_friendListMap.find(m_selectType);
	}
	CFriend *frd = &iter->second.at(uIdx);
	for (int i=1;i<=11;++i)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);
		switch (i)
		{
		case 1:
			break;
		case 2:
			{
				if (frd->thumb>0)
				{
					CCSprite *spr = CCSprite::create(CCString::createWithFormat("headImg/%d.png", frd->thumb)->getCString());
					if (!spr)
					{
						spr = CCSprite::create("headImg/101.png");
						CCLOG("[ ERROR ] CFriendLayer::addFriendCell Lost Image = %d",frd->thumb);
					}
					child->addChild(spr);
					NodeFillParent(spr);
				}
				else
				{
					string fbName = frd->fbId+".jpg";
					string fullName = CCFileUtils::sharedFileUtils()->fullPathForFilename(fbName.c_str());
					bool isFileExist = CCFileUtils::sharedFileUtils()->isFileExist(fullName);
					if(isFileExist)
					{
						CCSprite* spr =CCSprite::create(fullName.c_str());
						if (spr)
						{
							child->addChild(spr);
							NodeFillParent(spr);
						}
					}
					else
					{
						CCString *imgUrl = CCString::createWithFormat(FACEBOOKIMG,frd->fbId.c_str());
						HttpLoadImage::getInstance()->requestUrlImage(imgUrl->getCString(),frd->fbId.c_str());
					}
				}
			}
			break;
		case 3:
			{
				((CLabel*)child)->setString(frd->friendName.c_str());
			}
			break;
		case 4:
			{
				((CLabel*)child)->setString(CCString::createWithFormat("Lv%d",frd->level)->getCString());
			}
			break;
			//战斗力
		case 5:
			{
				int batlev = getCombatLevel(frd->combat);
				CImageView *img = (CImageView*)child;
				if (batlev<=3)
				{
					//img->setTexture(CCTextureCache::sharedTextureCache()->addImage("common/combat_1.png"));
				}
				else
				{
					//img->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("warPreview/combat_%d.png",batlev)->getCString()));
				}
			}
			break;
		case 6:
			{
				((CLabelAtlas*)child)->setString(ToString(frd->combat));
				child->setAnchorPoint(ccp(0,0.5));
			}
			break;
		case 7:
			{
				time_t t  = frd->loginTime/1000;		
				int  hour=0, min = 0; int day = 0;
				if (t>0)
				{
					hour = floor(t / 3600);
					min = floor((t - hour * 3600) / 60);
					int sec = floor(t - hour * 3600 - min * 60);
					day =  floor(hour/24); 
				}
				char buf[64];
				if (day>0)
				{
					hour -= 24*day;
					sprintf(buf,GETLANGSTR(23),day,hour);
				}
				else if (frd->online)
				{
				    sprintf(buf,"%s",GETLANGSTR(244));
				}
				else
				{
					sprintf(buf,GETLANGSTR(161),hour,min);
				}
				
				((CLabel*)child)->setString(buf);
			}
			break;
		case 8:
			{
				CButton *btn = (CButton*)child;
				btn->setUserData(&iter->second.at(uIdx));
				if (m_isSearch)
				{
					btn->setOnClickListener(this,ccw_click_selector(CFriendLayer::onAddFriend));
				}
				else if (m_selectType==1)
				{
					btn->setOnClickListener(this,ccw_click_selector(CFriendLayer::onDeleteFriend));
				}
				else if (m_selectType==2)
				{
					btn->setOnClickListener(this,ccw_click_selector(CFriendLayer::onAddFriend));
				}
			}
			break;
		default:
			break;
		}
	}
}

void CFriendLayer::addAcceptCell(CLayout* layCell, unsigned int uIdx, CTableViewCell * pCell)
{
	CLayout *lay = UICloneMgr::cloneLayout(layCell);
	FriendMap::iterator iter = m_friendListMap.find(m_selectType);
	CFriend *frd = &iter->second.at(uIdx);
	for (int i=1;i<=11;++i)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);
		switch (i)
		{
		case 1:
			break;
		case 2:
			{
				CCSprite *spr = CCSprite::create(CCString::createWithFormat("headImg/%d.png", frd->thumb)->getCString());
				if (!spr)
				{
					spr = CCSprite::create("headImg/101.png");
					CCLOG("[ ERROR ] CFriendLayer::addFriendCell Lost Image = %d",frd->thumb);
				}
				child->addChild(spr);
				spr->setScale(0.8f);
				spr->setPosition(ccp(child->getContentSize().width/2,child->getContentSize().height/2));
			}
			break;
		case 3:
			{
				((CLabel*)child)->setString(frd->friendName.c_str());
			}
			break;
		case 4:
			{
				((CLabel*)child)->setString(CCString::createWithFormat("Lv.%d",frd->level)->getCString());
			}
			break;
		case 6:
			{
				((CLabelAtlas*)child)->setString(ToString(frd->vip));
			}
			break;
		case 7:
			{
#ifdef _WIN32
				time_t t  = frd->loginTime/1000;
#else
				time_t t = frd->loginTime;
#endif
				time_t now = time(nullptr);
				t = now - t;
				int  hour=0, min = 0; int day = 0;
				if (t>0)
				{
					hour = floor(t / 3600);
					min = floor((t - hour * 3600) / 60);
					int sec = floor(t - hour * 3600 - min * 60);
					day =  floor(hour/24);
				}
				char buf[64];
				if (day>0)
				{
					hour -= 24*day;
					sprintf(buf,"%d天%d小时前上线",day,hour);
				}
				else if (hour==0&&min==0)
				{
					sprintf(buf,"%s","在线");
				}
				else
				{
					sprintf(buf,"%d小时%d分前上线",hour,min);
				}
				((CLabel*)child)->setString(buf);
			}
			break;
		case 8:
		case 10:
			{
				CButton *btn = (CButton*)child;
				btn->setUserData(&iter->second.at(uIdx));	
				btn->setOnClickListener(this,ccw_click_selector(CFriendLayer::onAcceptFriend));
			}
			break;
		default:
			break;
		}
	}
}

void CFriendLayer::friendListRes(int type, google::protobuf::Message *msg)
{
	FriendListResponse *res = (FriendListResponse*)msg;
	if(!isVisible())
	{
		this->setVisible(true);
		//NOTIFICATION->postNotification(HIDE_MAIN_SCENE);
	}

	if (m_isSearch)
	{
		if (m_friendListMap.find(4)!=m_friendListMap.end())
		{
			m_friendListMap.find(4)->second.clear();
		}
		for (int i = 0; i < res->friends_size(); i++)
		{
			CFriend frd;
			frd.read(res->friends(i));
			FriendMap::iterator iter = m_friendListMap.find(4);
			if (iter!= m_friendListMap.end())
			{
				iter->second.push_back(frd);
			}
			else
			{
				vector<CFriend> frdlist;
				frdlist.push_back(frd);
				m_friendListMap[4] = frdlist; 
			}
		}
		if (res->friends_size()>0)
		{	
			m_friendView->setVisible(false);
			m_recommendView->setVisible(false);

			CTableView *searchTable = CTableView::create(m_friendTable->getContentSize());
			searchTable->setPosition(m_friendTable->getPosition());
			searchTable->setDirection(eScrollViewDirectionVertical);
			searchTable->setSizeOfCell(m_cell->getContentSize());
			searchTable->setCountOfCell(res->friends_size());
			searchTable->setBounceable(false);
			searchTable->setAnchorPoint(ccp(0,0));
			searchTable->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CFriendLayer::friendDataSource));
			searchTable->reloadData();
			m_ui->addChild(searchTable,0,100);
		}
		else
		{
			ShowPopTextTip(GETLANGSTR(175));
		}
	
	}
	else
	{
		m_ui->removeChildByTag(100);
		if (m_friendListMap.find(m_selectType)!=m_friendListMap.end())
		{
			m_friendListMap.find(m_selectType)->second.clear();
		}
		for (int i = 0; i < res->friends_size(); i++)
		{
			CFriend frd;
			frd.read(res->friends(i));
			FriendMap::iterator iter = m_friendListMap.find(m_selectType);
			if (iter!= m_friendListMap.end())
			{
				iter->second.push_back(frd);
			}
			else
			{
				vector<CFriend> frdlist;
				frdlist.push_back(frd);
				m_friendListMap[m_selectType] = frdlist; 
			}
		}
	
		if (m_friendListMap.find(m_selectType)!=m_friendListMap.end())
		{
			CTableView *pTable = nullptr;
			if (m_selectType==1)
			{
				m_friendTable->setCountOfCell(m_friendListMap.find(m_selectType)->second.size());
				m_friendTable->reloadData();
				pTable =  m_friendTable;
			}
			else if (m_selectType==2)
			{
				m_recommendTable->setCountOfCell(m_friendListMap.find(m_selectType)->second.size());
				m_recommendTable->reloadData();
				pTable =  m_recommendTable;
			}
			else
			{
				m_acceptTable->setCountOfCell(m_friendListMap.find(m_selectType)->second.size());
				m_acceptTable->reloadData();
				pTable =  m_acceptTable;
			}
			int size = m_friendListMap.find(m_selectType)->second.size();
			for (int i = 0; i <size&&i<4; i++)
			{
				CTableViewCell *pCell = pTable->cellAtIndex(i);
				pCell->setPositionY(pCell->getPositionY()-400);
				pCell->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.1*i),CCEaseElasticInOut::create(CCMoveBy::create(0.3f,ccp(0,400)))));
			}
			CCSprite *frontBg = (CCSprite*)(m_ui->findWidgetById("frontBg"));
		    if (!frontBg->getUserObject())
			{
				frontBg->setUserObject(CCBool::create(true));
				int times = size>4?4:size;
				frontBg->runAction(CCRepeat::create(CCSequence::create(CCMoveBy::create(0.05f,ccp(0,10)),CCMoveBy::create(0.05f,ccp(0,-10)),CCDelayTime::create(0.1f),nullptr),times));
		    }
		
		}
	
		if (res->has_friendlimit())
		{
			CLabel *online = (CLabel*)m_ui->findWidgetById("online");
			online->setString(CCString::createWithFormat("%d/%d",res->friends_size(),res->friendlimit())->getCString());
		}

		//如果好友数量为0, 
		if(res->friends_size()<=0)
		{
			showFriendTip(true, m_selectType==1?GETLANGSTR(1236):GETLANGSTR(1237));
		}

		if (m_selectType==1)
		{
			CLayout *find = (CLayout*)(m_ui->findWidgetById("find"));
			find->setVisible(true);
			m_textFieldName->setVisible(true);
		}
	}
}

void CFriendLayer::removeCell(int type)
{
	FriendMap::iterator iter = m_friendListMap.find(type);
	vector<CFriend>::iterator it = iter->second.begin();
	for (; it!= iter->second.end(); it++)
	{
		if (m_selectFriend->friendId == it->friendId)
		{
			iter->second.erase(it);
			break;
		}
	}
	m_selectFriend = nullptr;
	if (type==1)
	{
		m_friendTable->setCountOfCell(m_friendListMap.find(type)->second.size());
		m_friendTable->reloadData();
	}
	else if (type==2)
	{
		m_recommendTable->setCountOfCell(m_friendListMap.find(type)->second.size());
		m_recommendTable->reloadData();
	}
	else
	{
		m_acceptTable->setCountOfCell(m_friendListMap.find(type)->second.size());
		m_acceptTable->reloadData();
	}
	
}

void CFriendLayer::accountBinRes(int type, google::protobuf::Message *msg)
{
	AcountBindRes *res = (AcountBindRes*)msg;
	if (res->result())
	{
		ShowPopTextTip(GETLANGSTR(1121));
	}
}

void CFriendLayer::imageLoadSuccessCallBack(string sTag, vector<char>* pBuffer)
{
	CCImage* img = new CCImage;
	img->initWithImageData((unsigned char*)pBuffer->data(), pBuffer->size());
	CCTexture2D* texture = new CCTexture2D();
	texture->initWithImage(img);

	string path = HttpLoadImage::getInstance()->getStoragePath("download/fbImg",sTag.c_str())+".jpg";
	string buff(pBuffer->begin(), pBuffer->end());
	CCLOG("path: %s", path.c_str());
	FILE *fp = fopen(path.c_str(), "wb+");
	fwrite(buff.c_str(), 1, pBuffer->size(),  fp);
	fclose(fp);

	FriendMap::iterator iter = m_friendListMap.find(m_selectType);	
	CTableViewCell *pCell = nullptr;
	int nIdx =-1;
	if (iter!=m_friendListMap.end())
	{
		for (int i = 0; i < iter->second.size(); i++)
		{
			if (iter->second.at(i).fbId==sTag)
			{
				nIdx = i;
				break;
			}
		}
	}
	if (nIdx!=-1)
	{
		if (m_selectType==1)
		{
			pCell = m_friendTable->cellAtIndex(nIdx);
		}
		else if (m_selectType==2)
		{
			pCell = m_recommendTable->cellAtIndex(nIdx);
		}
	}
	CCSprite *headImg = CCSprite::createWithTexture(texture);
	if (pCell) pCell->getChildByTag(2)->addChild(headImg);
	NodeFillParent(headImg);
	img->release();
}

void CFriendLayer::showFriendTip( bool bShow, const char* sInfo )
{
	if(bShow)
	{
		if(m_pPanelTips==nullptr)
		{
			m_pPanelTips = CPanelTips::create();
			this->addChild(m_pPanelTips);
		}
		m_pPanelTips->setVisible(true);
		m_pPanelTips->setString(sInfo);
	}
	else
	{
		if(m_pPanelTips!=nullptr)
		{
			m_pPanelTips->setVisible(false);
		}
	}
}
