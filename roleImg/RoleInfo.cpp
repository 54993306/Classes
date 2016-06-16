#include "RoleInfo.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "GMessage.h"

#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "tools/UICloneMgr.h"

#include "mainCity/MainCityControl.h"

#include "ChangeName.h"
#include "ChangeHeadImg.h"

#include "set/SetLayer.h"
#include "common/CShowToBuyResource.h"

#include "common/CGameSound.h"
#include "Resources.h"

CRoleInfo::CRoleInfo()
{

}

bool CRoleInfo::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("RoleInfo");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("RoleInfo.xaml");  //  SelectSkill
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		this->setVisible(true);

		return true;
	}
	return false;

}

void CRoleInfo::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CRoleInfo::onChangeHeadImg(CCObject* pSender)
{
	CChangeHeadImg *changeHead = CChangeHeadImg::create();
	LayerManager::instance()->push(changeHead);
	GetTcpNet->sendDataType(HeadImgListMsg,true);
}

void CRoleInfo::onSet(CCObject* pSender)
{
	CSetLayer *setLayer = CSetLayer::create();
	LayerManager::instance()->push(setLayer);
}

void CRoleInfo::onChangName(CCObject* pSender)
{
	CChangeName *changeName = CChangeName::create();
	LayerManager::instance()->push(changeName);
}

void CRoleInfo::onEnter()
{
	BaseLayer::onEnter();

	////退出
	//CButton* pClose = CButton::create("common/back.png", "common/back.png");
	//pClose->getSelectedImage()->setScale(1.1f);
	//pClose->setPosition(VLEFT+50, VTOP-50);
	//pClose->setOnClickListener(this,ccw_click_selector(CRoleInfo::onClose));
	//this->addChild(pClose, 999);

	CButton *changeName = (CButton*)(m_ui->findWidgetById("changeName"));
	changeName->setOnClickListener(this,ccw_click_selector(CRoleInfo::onChangName));

	CButton *changeimg = (CButton*)(m_ui->findWidgetById("changeimg"));
	changeimg->setOnClickListener(this,ccw_click_selector(CRoleInfo::onChangeHeadImg));

	CImageView *headbg = (CImageView*)(m_ui->findWidgetById("headbg"));
	headbg->setTouchEnabled(true);
	headbg->setOnClickListener(this,ccw_click_selector(CRoleInfo::onChangeHeadImg)); //
	
	CButton *onset = (CButton*)(m_ui->findWidgetById("onset"));
	onset->setOnClickListener(this,ccw_click_selector(CRoleInfo::onSet));

	GetTcpNet->registerMsgHandler(ChangeNameMsg,this, CMsgHandler_selector(CRoleInfo::ProcessMsg));
	GetTcpNet->registerMsgHandler(ChangeHeadImgMsg,this, CMsgHandler_selector(CRoleInfo::ProcessMsg));

	NOTIFICATION->addObserver(this, callfuncO_selector(CRoleInfo::show), SHOW_ROLE_INFO, nullptr);
	NOTIFICATION->addObserver(this, callfuncO_selector(CRoleInfo::hide), HIDE_ROLE_INFO, nullptr);
}


void CRoleInfo::onExit()
{
	BaseLayer::onExit();

	GetTcpNet->unRegisterAllMsgHandler(this);
	NOTIFICATION->removeAllObservers(this);
}

CRoleInfo::~CRoleInfo()
{
}

void CRoleInfo::setRoleInfoVal(UserData *data)
{
	m_data = data;

	CLabel *name = (CLabel*)(m_ui->findWidgetById("name"));
	name->setString(data->getRoleName().c_str());

	CLabel *level = (CLabel*)(m_ui->findWidgetById("level"));
	level->setString(ToString(data->getLevel()));

	CLabel *exp = (CLabel*)(m_ui->findWidgetById("exp"));
	exp->setString(CCString::createWithFormat("%d/%d",data->getExp(),data->getNextExp())->getCString());

	CImageView *headImg = (CImageView*)(m_ui->findWidgetById("headImg"));
	if (data->getThumb()>0)
	{
		CCTexture2D *texture = CCTextureCache::sharedTextureCache()
			->addImage(CCString::createWithFormat("headIcon/%d.png", data->getThumb())->getCString());
		if (texture)
		{
			headImg->setTexture(texture);
		}
	}
	else
	{
		string fbName = "fbImg/"+data->getFbId()+".png";
		string fullName = CCFileUtils::sharedFileUtils()->fullPathForFilename(fbName.c_str());
		bool isFileExist = CCFileUtils::sharedFileUtils()->isFileExist(fullName);
		if(isFileExist)
		{
			CCSprite *spr = maskedSprite(CCSprite::create(fullName.c_str()));
			headImg->setTexture(spr->getTexture());
		}
	}

	CLabel *acttime = (CLabel*)(m_ui->findWidgetById("time"));

	if (data->getActionTime()==0)
	{
		acttime->setString(GETLANGSTR(1122));
	}
	else
	{
		recoverTime(data, acttime);
		this->schedule(schedule_selector(CRoleInfo::updateActionTime),60);
	}
	CLabel *roleId = (CLabel*)(m_ui->findWidgetById("roleId"));
	roleId->setString(ToString(data->getRoleID()));
}

CCSprite* CRoleInfo::maskedSprite(CCSprite *textureSprite)  
{  
	CCSprite * maskSprite = CCSprite::create("mainCity/touxiang_kuang.png");  
	CCRenderTexture * renderTexture = CCRenderTexture::create(maskSprite->getContentSize().width, maskSprite->getContentSize().height);  

	maskSprite->setPosition(ccp(maskSprite->getContentSize().width / 2, maskSprite->getContentSize().height / 2));  
	textureSprite->setPosition(ccp(textureSprite->getContentSize().width / 2, textureSprite->getContentSize().height / 2));  

	ccBlendFunc bfun1;
	bfun1.src = GL_ONE;
	bfun1.dst = GL_ZERO;
	maskSprite->setBlendFunc(bfun1);  
	ccBlendFunc bfun2;
	bfun2.src = GL_DST_ALPHA;
	bfun2.dst = GL_ZERO;
	textureSprite->setBlendFunc(bfun2);  

	renderTexture->begin();  
	maskSprite->visit();  
	textureSprite->visit();  
	renderTexture->end();  

	CCSprite * retval = CCSprite::createWithTexture(renderTexture->getSprite()->getTexture());  
	retval->setFlipY(true);  
	return retval;  
}  

void CRoleInfo::updateActionTime(float dt)
{
	CLabel *acttime = (CLabel*)(m_ui->findWidgetById("time"));
	UserData *data = DataCenter::sharedData()->getUser()->getUserData();
    data->setActionTime(data->getActionTime()-60000);
	recoverTime(data,acttime);
}

void CRoleInfo::ProcessMsg(int type, google::protobuf::Message *msg)
{

	switch (type)
	{
	case ChangeNameMsg:
	case ChangeHeadImgMsg:
		{
			RoleCtrlRes *res = (RoleCtrlRes*)msg;
			if (res->result()==1&&res->has_role())
			{
				UserData *data = DataCenter::sharedData()->getUser()->getUserData();
				data->read(res->role());
				CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,10,nullptr,nullptr);
				updateRoleProperty();
			}
			else
			{
				processError(res->result());
			}
		}
		break;
	default:
		break;
	}
}

void CRoleInfo::updateRoleProperty()
{
	UserData *data = DataCenter::sharedData()->getUser()->getUserData();

	CLabel *name = (CLabel*)(m_ui->findWidgetById("name"));
	name->setString(data->getRoleName().c_str());

	CCSprite* head = (CCSprite*)m_ui->getChildByTag(15);
	CCTexture2D *texture = CCTextureCache::sharedTextureCache()
		->addImage(CCString::createWithFormat("headIcon/%d.png", data->getThumb())->getCString());
	if (texture)
	{
		head->setTexture(texture);
	}
}

void CRoleInfo::processError(int errCode)
{
	//改名结果(1成功，2名称已存在，3元宝不足，4数据错误)
	switch (errCode)
	{
	case 2:
		ShowPopTextTip(GETLANGSTR(193));
		break;
	case 3:
		{
			//ShowPopTextTip(GETLANGSTR(194));
			CShowToBuyResource* pShow = CShowToBuyResource::create();
			pShow->showToBuyResourceByType(ShowBuyResourceGold);
		}
		break;
	case 4:
		ShowPopTextTip(GETLANGSTR(170));
		break;
	default:
		break;
	}
}

bool CRoleInfo::ccTouchBegan( CCTouch *pTouch, CCEvent *pEvent )
{
	bool res = CWidgetWindow::ccTouchBegan(pTouch,pEvent);
	if( !res )
	{
		CCPoint pTouchPos = m_ui->convertToNodeSpace(pTouch->getLocation());

		CImageView *bgView1 = (CImageView*)(m_ui->findWidgetById("bg"));

		if( bgView1->boundingBox().containsPoint(pTouchPos))
		{
			res = true;
		}
		else
		{
			onClose(nullptr);
		}
	}
	return true;
}

void CRoleInfo::recoverTime(UserData * data, CLabel * acttime)
{
	time_t t = data->getActionTime()/1000;
	int  hour=0, min = 0;
	if (t>0)
	{
		hour = floor(t / 3600);
		min = floor((t - hour * 3600) / 60);
		int sec = floor(t - hour * 3600 - min * 60);
		int day =  floor(hour/24);
	}
	char buf[64];
	sprintf(buf,"%d:%d",hour,min);
	acttime->setString(buf);
}

void CRoleInfo::hide(CCObject* pObj)
{
	setVisible(false);
}

void CRoleInfo::show(CCObject* pObj)
{
	setVisible(true);
}

