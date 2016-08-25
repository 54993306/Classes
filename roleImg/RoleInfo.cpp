#include "RoleInfo.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
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
#include "bag/bagData.h"
#include "Global.h"
#include "common/CommonFunction.h"

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
// 	HttpLoadImage::getInstance()->bindUiTarget(this);

}


void CRoleInfo::onExit()
{
	BaseLayer::onExit();
// 	HttpLoadImage::getInstance()->bindUiTarget(nullptr);

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

	CCSprite *headbg = (CCSprite*)(m_ui->findWidgetById("headbg"));
	CImageView *headImg = (CImageView*)(m_ui->findWidgetById("headImg"));
	if (data->getThumb()>0)
	{
		CCTexture2D *texture = CCTextureCache::sharedTextureCache()
			->addImage(CCString::createWithFormat("headIcon/%d.png", data->getThumb())->getCString());
		if (texture)
		{
			headImg->setTexture(texture);
		}
		headImg->setFlipY(false);

	}
	else
	{
		string fbName = /*CCFileUtils::sharedFileUtils()->getWritablePath()+*//*"download/fbImg/"+*/data->getFbId()+".jpg";
		string fullName = CCFileUtils::sharedFileUtils()->fullPathForFilename(fbName.c_str());
		bool isFileExist = CCFileUtils::sharedFileUtils()->isFileExist(fullName);

		if(isFileExist)
		{
			CImageView *sp = CImageView::create(fbName.c_str());
			if (sp)
			{	
				CImageView *spr = MakeFaceBookHeadToCircle(sp);	
				spr->setPosition(headbg->getPosition());
				m_ui->removeChild(headImg);
				spr->setId("headImg");
				m_ui->addChild(spr);
			}
			else
			{
				CCLOGERROR("%s error", fullName.c_str());
			}
		
		}
// 		else
// 		{
// 			HttpLoadImage::getInstance()->bindUiTarget(this);
// 			CCString *imgUrl = CCString::createWithFormat(FACEBOOKIMG_106,data->getFbId().c_str());
// 			HttpLoadImage::getInstance()->requestUrlImage(imgUrl->getCString(),data->getFbId().c_str());
// 		}
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

void CRoleInfo::imageLoadSuccessCallBack(string sTag, vector<char>* pBuffer)
{
	CCImage* img = new CCImage;
	img->initWithImageData((unsigned char*)pBuffer->data(), pBuffer->size());
	CCTexture2D* texture = new CCTexture2D();
	texture->initWithImage(img);
	CLayout* headPart = (CLayout*)m_ui->findWidgetById("head_part");

	CCSprite* head = (CCSprite*)headPart->getChildByTag(15);
	//	head->removeFromParent();
	//head->setTexture(texture);

	string path = HttpLoadImage::getInstance()->getStoragePath("download/fbImg",sTag.c_str())+".png";
	string buff(pBuffer->begin(), pBuffer->end());
	CCLOG("path: %s", path.c_str());
	FILE *fp = fopen(path.c_str(), "wb+");
	fwrite(buff.c_str(), 1, pBuffer->size(),  fp);
	fclose(fp);

	CImageView *headImg = (CImageView*)(m_ui->findWidgetById("headImg"));
	CCSprite *headSpr = MakeFaceBookHeadToCircle(CCSprite::createWithTexture(texture));
	headImg->setTexture(headSpr->getTexture());
	headImg->setTextureRect(CCRectMake(0,0,105,105));
	img->release();
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
	CCSprite *headbg = (CCSprite*)(m_ui->findWidgetById("headbg"));
	CImageView *headImg = (CImageView*)(m_ui->findWidgetById("headImg"));

	if (data->getThumb()>0)
	{
		CImageView *spr = (CImageView*)CImageView::create(CCString::createWithFormat("headIcon/%d.png", data->getThumb())->getCString());
		spr->setPosition(ccpAdd(headbg->getPosition(),ccp(0,10)));
		spr->setScale(0.75f);
		m_ui->removeChild(headImg);
		spr->setId("headImg");
		m_ui->addChild(spr);
	}
	else
	{
		string fbName = data->getFbId()+".jpg";
		string fullName = CCFileUtils::sharedFileUtils()->fullPathForFilename(fbName.c_str());
		bool isFileExist = CCFileUtils::sharedFileUtils()->isFileExist(fullName);
		if(isFileExist)
		{
			CImageView *spr = MakeFaceBookHeadToCircle(CCSprite::create(fullName.c_str()));
			spr->setPosition(headbg->getPosition());
			m_ui->removeChild(headImg);
			spr->setId("headImg");
			m_ui->addChild(spr);
		}
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

