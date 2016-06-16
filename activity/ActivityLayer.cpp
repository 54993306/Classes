#include "ActivityLayer.h"
#include "tools/UICloneMgr.h"
#include "ActivityControl.h"
#include "tools/StringUtil.h"
#include "tools/ShowTexttip.h"
#include "sign/PopItem.h"
#include "model/DataCenter.h"
#include "vip/VipLayer.h"
#include "GMessage.h"
#include "reward/RewardLayer.h"
#include "mainCity/Recruit.h"
#include "scene/AnimationManager.h"
#include "Battle/EffectObject.h"
#include "common/CommonFunction.h"
#include "common/CheckMoney.h"
#include "common/CShowToBuyResource.h"
#include "scene/CPopTip.h"
#include "mainCity/MainCityControl.h"
#include "update/CUpdateLayer.h"
#include "common/CGameSound.h"
#include "Resources.h"
//#include <thread>
//#include <future>
using namespace BattleSpace;
bool SortActivity(const CActivity& activity1, const CActivity& activity2)
{
	return activity1.iStatus<activity2.iStatus;
}

CActivityLayer::CActivityLayer():
m_ui(nullptr),m_pLoading(nullptr)
{
	for(unsigned int i=0; i<ActivityTabMax; i++)
	{
		m_cell[i] = nullptr;
		m_tableView[i] = nullptr;
		m_index_selected[i] = 0;
		m_base_url[i] = "";
	}	

	m_show_info_sprite = nullptr;

	m_iRequestLimit = 1;

	m_activity_type = ActivityTabSale;

	m_join_activity = nullptr;

	m_join_activity_label = nullptr;
}

CActivityLayer::~CActivityLayer()
{
	for(unsigned int i=0; i<ActivityTabMax; i++)
	{
		CC_SAFE_RELEASE(m_cell[i]);
	}
}

bool CActivityLayer::init()
{
	if(BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();
		
		//黑底
		MaskLayer* pMaskLayer = MaskLayer::create("ActivityMaskLayer");
		pMaskLayer->setContentSize(winSize);
		LayerManager::instance()->push(pMaskLayer);

		//内容
		m_ui = LoadComponent("activity.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		return true;
	}

	return false;
}

void CActivityLayer::onEnter()
{
	BaseLayer::onEnter();

	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("skill/loading.plist");
	AnimationManager::sharedAction()->ParseAnimation("loading");

	//loading贴图
	EffectObject* pLoadEffect = EffectObject::create("loading",PLAYERTYPE::Repeat);
	pLoadEffect->setPosition(dynamic_cast<CCNode*>(m_ui->findWidgetById("loading"))->getPosition());
	pLoadEffect->setEffAnchorPoint(0.5f,0.5f);
	pLoadEffect->setScale(0.6f);
	m_ui->addChild(pLoadEffect, 99);
	m_pLoading = pLoadEffect;
	m_pLoading->setVisible(false);

	//选项卡
	CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_ui->getChildByTag(1);
	for (unsigned int i=0; i<ActivityTabMax; i++)
	{
		CRadioButton *radioBtn= (CRadioButton*)(radioGroup->getChildByTag(i));
		radioBtn->setOnCheckListener(this,ccw_check_selector(CActivityLayer::onSwitchBtn));
		m_pRadioButton[i] = radioBtn;
	}

	//初始化战斗活动和销售活动版块
	initTabPart();

	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CActivityLayer::onClose));
	this->addChild(pClose, 999);

	//参与活动按钮
	m_join_activity= (CButton *)m_ui->findWidgetById("join_activity");
	m_join_activity->setOnClickListener(this,ccw_click_selector(CActivityLayer::onJoinActivity));

	//参与活动标签
	m_join_activity_label = (CLabel*)m_ui->findWidgetById("join_label");

	//展示区图片
	m_show_info_sprite = CCSprite::create(default_activity_show_sprite);
	m_show_info_scroll = (CScrollView*)m_ui->findWidgetById("scroll_info");
	m_show_info_scroll->setDirection(eScrollViewDirectionVertical);
	m_show_info_scroll->setBounceable(false);
	m_show_info_scroll->getContainer()->addChild(m_show_info_sprite);
	updateShowInfoScroll();

	////执行弹出框动作
	//m_ui->setScale(0.0f);
	//CCScaleTo *big = CCScaleTo::create(0.2f,1.0f);
	//m_ui->runAction(big);

	//绑定活动列表回调
	GetTcpNet->registerMsgHandler(ActListMsg, this, CMsgHandler_selector(CActivityLayer::processNetMsg));
	//绑定购买礼包回调
	GetTcpNet->registerMsgHandler(BuyGiftMsg, this, CMsgHandler_selector(CActivityLayer::processNetMsg));

// 	//注册场景事件监听
 	//CSceneManager::sharedSceneManager()->addMsgObserver(DELETE_TASK, this, GameMsghandler_selector(CTaskLayer::deleteTaskMsg));

	//绑定图片
	HttpLoadImage::getInstance()->bindUiTarget(this);

	//默认选择第一个
	selectTabPanel(ActivityTabSale, true);
}

void CActivityLayer::onExit()
{
	BaseLayer::onExit();

	//解绑网络回调处理
	GetTcpNet->unRegisterAllMsgHandler(this);
	//解绑场景事件监听
	CSceneManager::sharedSceneManager()->removeMsgObserver("NULL", this);

	if(this == HttpLoadImage::getInstance()->getTarget())
	{
		HttpLoadImage::getInstance()->bindUiTarget(nullptr);
	}

	//移除无用的资源
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();

	NOTIFICATION->postNotification(SHOW_MAIN_SCENE);
	CSceneManager::sharedSceneManager()->PostMessageA(SHOW_HEAD,0,nullptr,nullptr);

}

void CActivityLayer::initTabPart()
{
	//获取编辑器中设定好的Cell, 暂时用同一个
	CLayout* pLayout = (CLayout*)m_ui->findWidgetById(activity_cell[0]);
	pLayout->retain();
	pLayout->retain();
	m_ui->removeChild(pLayout);
	m_cell[ActivityTabBattle] = pLayout;
	m_cell[ActivityTabSale] = pLayout;

	for(unsigned int i=0; i<ActivityTabMax; i++)
	{
		//获取空的滑动列表Scroll
		m_tableView[i] = (CTableView*)m_ui->findWidgetById(activity_scroll[i]);
		m_tableView[i]->setDirection(eScrollViewDirectionVertical);
		m_tableView[i]->setSizeOfCell(m_cell[i]->getContentSize());
		m_tableView[i]->setCountOfCell(0);
		m_tableView[i]->setBounceable(true);
		m_tableView[i]->setDataSourceAdapter(this, ccw_datasource_adapter_selector(CActivityLayer::tableviewDataSource));
	}
}

void CActivityLayer::onSwitchBtn( CCObject *pSender, bool bChecked )
{
	CRadioButton *btn = (CRadioButton*)pSender;
	int selIndex = btn->getTag();

	if (bChecked)
	{
		PlayEffectSound(SFX_Button);

		selectTabPanel((ActivityTabType)selIndex);

		//if(m_activity_type==ActivityTabBattle)
		//{
		//	showBookAnimate(m_ui, false);
		//}
		//else
		//{
		//	showBookAnimate(m_ui);
		//}
		

		if(m_iRequestLimit>0)
		{
			//发送网络请求
			CActivityControl::getInstance()->askForActList(m_activity_type+1);
			m_iRequestLimit--;
		}
		
	}
}

void CActivityLayer::selectTabPanel( ActivityTabType iType, bool bForce )
{
	if(m_activity_type==iType && !bForce)
	{
		return;
	}

	m_activity_type = iType;

	for(unsigned int i=0; i<ActivityTabMax; i++)
	{
		bool isSelected = i==iType;
		m_tableView[i]->setVisible(isSelected);
		m_pRadioButton[i]->setChecked(isSelected);
	}

	//更新展示区域
	updateShowArea();
}

void CActivityLayer::onClose( CCObject* pSender )
{
	PlayEffectSound(SFX_Ensure);
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CActivityLayer::processNetMsg( int type, google::protobuf::Message *msg )
{
	
	if(!isVisible())
	{
		this->setVisible(true);
		NOTIFICATION->postNotification(HIDE_MAIN_SCENE);

		showBookAnimate(m_ui);
	}
	
	switch (type)
	{
	case ActListMsg:
		{
			ActListRes *res = (ActListRes*)msg;

			int iRequestActivityType = res->tab()-1;

			m_activity_type = (ActivityTabType)iRequestActivityType;

			m_base_url[iRequestActivityType] = res->baseurl();

			m_activity_list[iRequestActivityType].clear();
			for (unsigned int i = 0; i < res->actlist_size(); i++)
			{
				CActivity activity;
				activity.read(res->actlist(i));
				m_activity_list[iRequestActivityType].push_back(activity);
			}

			//排序
			std::sort(m_activity_list[iRequestActivityType].begin(), m_activity_list[iRequestActivityType].end(), SortActivity);

			m_tableView[iRequestActivityType]->setCountOfCell(m_activity_list[iRequestActivityType].size());
			m_tableView[iRequestActivityType]->reloadData();
			
			//更新展示区
			updateShowArea();
		}
		break;
	case BuyGiftMsg:
		{
			BuyGiftRes* res = (BuyGiftRes*)msg;
			int iResult = res->result();
			switch (iResult)
			{
			case 1://成功
				{
					//解析购买成功后的数据
					CGetPrizeRes prizers;
					prizers.result =1;
					for (int i = 0; i < res->items_size(); i++)
					{  
						CPrize prize;
						prize.read(res->items(i));
						prizers.prizeList.push_back(prize);
					}

					//弹框
					CPopItem *popItem = CPopItem::create();
					LayerManager::instance()->push(popItem);
					popItem->popPrizeRes(&prizers);

					//收集货币
					collectMoneyFromPrize(prizers);

					//ShowPopTextTip(GETLANGSTR(264));
				}break;
			case 2://活动已结束
				{
					ShowPopTextTip(GETLANGSTR(265));
				}break;
			case 3://货币不足
				{
					//ShowPopTextTip(GETLANGSTR(266));
					CShowToBuyResource* pShow = CShowToBuyResource::create();
					pShow->showToBuyResourceByType(ShowBuyResourceGold);
				}break;
			case 4://数据错误
				{
					ShowPopTextTip(GETLANGSTR(207));
				}break;
			default:
				break;
			}
			//暂时不要礼包内容了，做图片上面

		}
		break;
	default:
		break;
	}
}


CCObject* CActivityLayer::tableviewDataSource( CCObject* pConvertCell, unsigned int uIdx )
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

void CActivityLayer::addTableCell( unsigned int uIdx, CTableViewCell * pCell )
{
	CLayout *lay = UICloneMgr::cloneLayout(m_cell[m_activity_type] );

	CActivity& pInfo = m_activity_list[m_activity_type][uIdx];

	for (unsigned int i=1; i<=4; i++)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		child->setTag(i);
		pCell->addChild(child);
		pCell->setTag(uIdx);

		switch (i)
		{
			//活动底板
		case 1:
			{
				CImageView* pBoard = (CImageView*)child;
				if(uIdx==m_index_selected[m_activity_type])
				{
					pBoard->setTexture(CCTextureCache::sharedTextureCache()->addImage(activity_select_panel));
				}
				pBoard->setTouchEnabled(true);
				pBoard->setOnClickListener(this, ccw_click_selector(CActivityLayer::activityCellClick));
			}
			break;
			//活动名称
		case 2:
			{
				CLabel *name = (CLabel*)child;
				name->setString(pInfo.sName.c_str());
			}
			break;
			//活动状态
		case 3:
			{
				//1进行中2即将开始3已结束
				CCSprite *item = (CCSprite*)child;
				item->removeAllChildren();
				item->setOpacity(0);
				CCSprite* pShow = CCSprite::create(activity_status_pic[pInfo.iStatus-1]);
				pShow->setPosition(ccp(item->getContentSize().width/2, item->getContentSize().height/2));
				item->addChild(pShow);
			}
			break;
			//活动图标
		case 4:
			{
				CCSprite *item = (CCSprite*)child;
				
				//没有图片，不读取，用默认
				if(pInfo.sIconFile.size()<=0)
				{
					continue;
				}
				//获取showInfo的本地路径
				string infoFile = getImageReadPath(m_activity_type, pInfo, ActivityLoadImageTagIcon);
				//更新图片
				CCSprite* pAnswer = HttpLoadImage::getInstance()->SetSpriteTextureWithLocalFile(infoFile, item);
				if(!pAnswer)
				{
					//发起网络获取
					HttpLoadImage::getInstance()->requestUrlImage(
						m_base_url[m_activity_type]+pInfo.sIconFile, 
						linkUrlImageTag(m_activity_type, pInfo, ActivityLoadImageTagIcon).c_str());
				}
				else
				{
					item = pAnswer;
				}
			}
			break;
		default:
			break;
		}
	}
}

void CActivityLayer::updateBattleList( const TMessage& tMsg )
{

}

void CActivityLayer::updateNewSaleList( const TMessage& tMsg )
{

}

void CActivityLayer::activityCellClick( CCObject* pObj )
{
	PlayEffectSound(SFX_Button);
	CImageView* pImageView = (CImageView*)pObj;
	int iCurrentSelectedIndex = pImageView->getParent()->getTag();
	//如果不是重复点击同一个
	if(m_index_selected[m_activity_type] != iCurrentSelectedIndex)
	{
		m_index_selected[m_activity_type] = iCurrentSelectedIndex;

		//更新table
		updateTableSelected(m_tableView[m_activity_type], m_index_selected[m_activity_type]);

		//更新右侧展示区域
		updateShowArea();
	}
	
}

void CActivityLayer::updateTableSelected( CTableView* pTable, int iSelected )
{
	CCObject* pObj;
	CCARRAY_FOREACH(pTable->getContainer()->getChildren(), pObj)
	{
		CCNode* pNode = (CCNode*)pObj;
		CImageView* pBoard = (CImageView*)pNode->getChildByTag(1);
		if(pNode->getTag()==iSelected)
		{
			pBoard->setTexture(CCTextureCache::sharedTextureCache()->addImage(activity_select_panel));
		}
		else
		{
			pBoard->setTexture(CCTextureCache::sharedTextureCache()->addImage(activity_unselect_panel));
		}
	}
}

void CActivityLayer::updateShowArea()
{
	//没数据的话，return
	if(!checkDataHere(m_activity_type))
	{
		return;
	}

	CActivity& activity =  m_activity_list[m_activity_type].at(m_index_selected[m_activity_type]);

	//获取showInfo的本地路径
	string infoFile = getImageReadPath(m_activity_type, activity, ActivityLoadImageTagInfo);
	//更新图片
	CCSprite* pAnswer = HttpLoadImage::getInstance()->SetSpriteTextureWithLocalFile(infoFile, m_show_info_sprite);
	if(!pAnswer)
	{
		CCLOG("本地图片加载失败");
		//设置为default图片
		updateShowInfoPicWithTexture(CCTextureCache::sharedTextureCache()->addImage(default_activity_show_sprite));
		//发起网络获取
		HttpLoadImage::getInstance()->requestUrlImage(m_base_url[m_activity_type]+activity.sInfoFile/*"http://192.168.0.9/dass/act/aa.jpg"*/, linkUrlImageTag(m_activity_type, activity, ActivityLoadImageTagInfo).c_str());
		m_pLoading->setVisible(true);
	}
	else
	{
		m_pLoading->setVisible(false);
		CCLOG("本地图片加载成功");
		m_show_info_sprite = pAnswer;
		updateShowInfoScroll();
	}

	m_join_activity_label->setVisible(false);
	m_join_activity->setVisible(false);

	//如果活动正在进行
	if(activity.iStatus == ActivityTabSale_under_way)
	{
		//根据活动类型更新图标
		switch (activity.iType)
		{
			//购买礼包
		case 1:
			{
				m_join_activity_label->setString(GETLANGSTR(267));
				m_join_activity_label->setVisible(true);
				m_join_activity->setVisible(true);
			}
			break;
			//充值
		case 2:
			{
				m_join_activity_label->setString(GETLANGSTR(268));
				m_join_activity_label->setVisible(true);
				m_join_activity->setVisible(true);
			}break;
			//悬赏府
		case 3:
			{
				m_join_activity_label->setString(GETLANGSTR(269));
				m_join_activity_label->setVisible(true);
				m_join_activity->setVisible(true);
			}break;
			//酒馆
		case 4:
			{
				m_join_activity_label->setString(GETLANGSTR(269));
				m_join_activity_label->setVisible(true);
				m_join_activity->setVisible(true);
			}break;
		default:
			break;
		}
	}
}

void CActivityLayer::onJoinActivity( CCObject* pSender )
{
	CCLOG("CActivityLayer::onJoinActivity");
	//1.跳转到对应关卡-世界BOSS-悬赏府(一种type对应一个跳转，不会涉及到详细关卡)
	//（悬赏府 type==3）
	//2.发起购买礼包-限定大礼包(type==1)
	//3.跳转到充值界面-首冲送熊猫-充值送好礼(按比例返还，有点难搞，能否直接给图片，取消天平)(type==2)
	//4.跳转到酒馆-新品武将(type==4)

	CActivity& activity = m_activity_list[m_activity_type].at(m_index_selected[m_activity_type]);
	//获取type
	int iType = activity.iType;

	switch (iType)
	{
		//购买礼包
	case 1:
		{
			ShowConfirmTextTip(CCString::createWithFormat(GETLANGSTR(1001), activity.iExtraParam)->getCString(),this,ccw_click_selector(CActivityLayer::buyGiftPopTipBack));
		}
		break;
		//充值
	case 2:
		{
			CVipLayer *vipLayer= CVipLayer::create();
			if(LayerManager::instance()->push(vipLayer))
			{
				GetTcpNet->sendDataType(VipInfoMsg,true);
				GetTcpNet->sendDataType(VipShopMsg,true);
			}
		}break;
		//悬赏府
	case 3:
		{
			if (CMainCityControl::getInstance()->isCityOpen(15))
			{	
				CRewardLayer *reward = CRewardLayer::create();
				LayerManager::instance()->push(reward);
				GetTcpNet->sendDataType(BountyDataMsg,true);
			}
			else
			{
				ShowPopTextTip(GETLANGSTR(202));
			}
		}break;
		//酒馆
	case 4:
		{
			CRecruit *recr = CRecruit::create();
			LayerManager::instance()->push(recr);
		}break;
	default:
		break;
	}
	

}

void CActivityLayer::imageLoadSuccessCallBack( string sTag,  vector<char>* pBuffer/*=nullptr*/ )
{
	//解析sTag()
	//活动类型_id_imagetype
	CCArray* array = strSplit(sTag.c_str(), "_");

	ActivityTabType activityType = (ActivityTabType)atoi(((CCString*)array->objectAtIndex(0))->getCString());
	
	CActivity& activity = getActivityByActId(activityType, atoi(((CCString*)array->objectAtIndex(1))->getCString()));

	if(&activity == NULL)
	{
		return;
	}

	ActivityLoadImageTag imageTag = (ActivityLoadImageTag)atoi(((CCString*)array->objectAtIndex(2))->getCString());


	// 数据转存本地
	string path = getImageSavePath(activityType, activity, imageTag);
	string buff(pBuffer->begin(), pBuffer->end());
	CCLOG("path: %s", path.c_str());
	FILE *fp = fopen(path.c_str(), "wb+");
	fwrite(buff.c_str(), 1, pBuffer->size(),  fp);
	fclose(fp);

	//创建精灵
	CCImage* img = new CCImage;
	img->initWithImageData((unsigned char*)pBuffer->data(), pBuffer->size());
	//create texture
	CCTexture2D* texture = new CCTexture2D();
	bool isImg = texture->initWithImage(img);
	img->release();

	//更新Texture
	switch (imageTag)
	{
	case ActivityLoadImageTagIcon:
		{
			CTableView* pTable = m_tableView[m_activity_type];
			CCObject* pObj;
			CCARRAY_FOREACH(pTable->getContainer()->getChildren(), pObj)
			{
				CCNode* pNode = (CCNode*)pObj;
				int iIndex = pNode->getTag();
				if(m_activity_list[m_activity_type].at(iIndex).iActId == activity.iActId)
				{
					CCSprite* pIcon = (CCSprite*)pNode->getChildByTag(4);
					pIcon->setTexture(texture);
					break;
				}
			}
		}
		break;
	case ActivityLoadImageTagInfo:
		{
			//判断当前返回的图片是不是当前选择的
			if(activityType==m_activity_type)
			{
				if(m_activity_list[m_activity_type].at(m_index_selected[m_activity_type]).iActId == activity.iActId)
				{
					CCLOG("网络图片加载成功");
					updateShowInfoPicWithTexture(texture);
					m_pLoading->setVisible(false);
				}
			}
		}
		break;
	default:
		break;
	}

	//释放Texture
	texture->release();
}

string CActivityLayer::linkUrlImageTag( ActivityTabType activityType, CActivity& activity, ActivityLoadImageTag  imageTag)
{
	//活动类型_id_imagetype
	string sTag = CCString::createWithFormat("%d_%d_%d", 
		activityType, 
		activity.iActId, 
		imageTag
		)->m_sString;

	return sTag;
}

std::string CActivityLayer::getImageSavePath( ActivityTabType activityType, CActivity& activity, ActivityLoadImageTag imageTag )
{
	string sTag = linkUrlImageTag(activityType, activity, imageTag);

	string picName = imageTag==ActivityLoadImageTagIcon?activity.sIconFile:activity.sInfoFile;

	std::string pathToSave = OTHER_DOWNLOAD_FILE_PATH;
	pathToSave += "/activity/";
	return pathToSave + sTag + picName;
}


std::string CActivityLayer::getImageReadPath( ActivityTabType activityType, CActivity& activity, ActivityLoadImageTag imageTag )
{
	string sTag = linkUrlImageTag(activityType, activity, imageTag);

	string picName = imageTag==ActivityLoadImageTagIcon?activity.sIconFile:activity.sInfoFile;

	return "activity/" + sTag + picName;
}


void CActivityLayer::updateShowInfoPicWithTexture( CCTexture2D* pTexture )
{
	CCSprite* pSprite = CCSprite::createWithTexture(pTexture);
	pSprite->setPosition(m_show_info_sprite->getPosition());
	pSprite->setAnchorPoint(m_show_info_sprite->getAnchorPoint());
	pSprite->setZOrder(m_show_info_sprite->getZOrder());
	m_show_info_sprite->getParent()->addChild(pSprite);
	m_show_info_sprite->removeFromParentAndCleanup(true);
	m_show_info_sprite = pSprite;
	updateShowInfoScroll();
}

bool CActivityLayer::checkDataHere(ActivityTabType iType)
{
	return m_activity_list[iType].size()>0;
}

CActivity& CActivityLayer::getActivityByActId( ActivityTabType iType, int iActId )
{
	for(unsigned int i=0; i<m_activity_list[iType].size(); i++)
	{
		CActivity& pActivity = m_activity_list[iType].at(i);
		if(pActivity.iActId == iActId)
		{
			return pActivity;
		}
	}
	CCLOG("CActivityLayer::getActivityByActId::ERROR");
	return *(CActivity*)NULL;
	//return m_activity_list[iType].at(0);
}

void CActivityLayer::updateShowInfoScroll()
{
	m_show_info_sprite->setContentSize(m_show_info_sprite->getTexture()->getContentSize());
	m_show_info_scroll->setContainerSize(m_show_info_sprite->getContentSize());
	m_show_info_sprite->setPosition(ccp(m_show_info_scroll->getContainerSize().width*0.5f, m_show_info_scroll->getContainerSize().height*0.5f));
	m_show_info_scroll->setContentOffsetToTop();
}

void CActivityLayer::buyGiftPopTipBack( CCObject* pSender )
{
	CButton* pBtn = (CButton*)pSender;

	if(pBtn->getTag()==PopTipConfirm)
	{
		CActivity& activity = m_activity_list[m_activity_type].at(m_index_selected[m_activity_type]);
		
		if(!CheckGold(activity.iExtraParam))
		{
			CShowToBuyResource* pShow = CShowToBuyResource::create();
			pShow->showToBuyResourceByType(ShowBuyResourceGold);
			return;
		}

		int iGiftId = activity.iActId;
		CActivityControl::getInstance()->askForBuyGift(iGiftId);	
	}

	((CPopTip*)(pBtn->getParent()->getParent()))->onClose(nullptr);
}



