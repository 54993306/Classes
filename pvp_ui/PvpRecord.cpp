#include "PvpRecord.h"
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
#include "scene/CPopTip.h"

#include "common/CGameSound.h"
#include "Resources.h"
#include "common/CommonFunction.h"
#include "Global.h"
#include "SelectPvpArmy.h"
#include "SelectChallengeLayer.h"

CPvpRecord::CPvpRecord():m_cell(nullptr),m_sRoleName("")
{

}


bool CPvpRecord::init()
{
	if (BaseLayer::init())
	{
		m_ui = LoadComponent("PvpRecord.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);	

		setIsShowBlack(false);

		return true;
	}
	return false;

}

void CPvpRecord::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CPvpRecord::onToggle(CCObject* pSender)
{
	PlayEffectSound(SFX_Button);

}

void CPvpRecord::onEnter()
{
	BaseLayer::onEnter();
	m_cell = (CLayout*)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
    m_ui->removeChild(m_cell);

	
	m_heroLay = (CLayout*)(m_ui->findWidgetById("hero"));
	m_heroLay->retain();
	m_ui->removeChild(m_heroLay);

	m_tableView = (CTableView*)m_ui->findWidgetById("scroll");
	m_tableView->setDirection(eScrollViewDirectionVertical);	
	m_tableView->setCountOfCell(0);
	m_tableView->setSizeOfCell(m_cell->getContentSize());
	m_tableView->setAnchorPoint(ccp(0,0));
	m_tableView->setAutoRelocate(true);
	m_tableView->setDeaccelerateable(false);
	m_tableView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CPvpRecord::tableviewDataSource));
	m_tableSize = m_tableView->getContentSize();


	GetTcpNet->registerMsgHandler(PvpRecordResMsg, this, CMsgHandler_selector(CPvpRecord::ProcessMsg));
}

CCObject* CPvpRecord::tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
{ 
	CTableViewCell* pCell = (CTableViewCell*)pConvertCell;
	if (!pCell)
	{
		pCell = new CTableViewCell();
		pCell->autorelease();
		pCell->setTag(uIdx);
		addTableCell(uIdx,pCell);
	}
	else
	{
		pCell->removeAllChildren();
		addTableCell(uIdx,pCell);
	}
	return pCell;
}



void CPvpRecord::onExit()
{
	BaseLayer::onExit();

	GetTcpNet->unRegisterAllMsgHandler(this);
} 

void CPvpRecord::addTableCell(unsigned int uIdx, CTableViewCell* pCell)
{
	const Record &pRecord = m_recordRes.record_list().Get(uIdx);
	pCell->setTag(uIdx);

	CLayout *lay = UICloneMgr::cloneLayout(m_cell);

	//时间
	int iMinuteAll = pRecord.battle_time();
	int iDay = iMinuteAll/1440;
	int iHour = (iMinuteAll%1440)/60;
	int iMinute = iMinuteAll%60;
	CLabel *pTime = (CLabel *)lay->findWidgetById("time");
	if(iDay>0)
	{
		pTime->setString(CCString::createWithFormat("%d%s%d%s%d%s", iDay, GETLANGSTR(1130), iHour, GETLANGSTR(1131), iMinute, GETLANGSTR(1132))->getCString());
	}
	else if(iHour>0)
	{
		pTime->setString(CCString::createWithFormat("%d%s%d%s", iHour, GETLANGSTR(1131), iMinute, GETLANGSTR(1132))->getCString());
	}
	else
	{
		pTime->setString(CCString::createWithFormat("%d%s", iMinute, GETLANGSTR(1132))->getCString());
	}
	

	//防守方-蓝-play2
	const RecordData& recordDataLeft = pRecord.player2();

	//进攻方-红-play1
	const RecordData& recordDataRight = pRecord.player1();

	//数据更新-头像（fb），名称，等级，排名变化，战斗队伍
	updateTeamInfo(recordDataLeft, 1, lay);
	updateTeamInfo(recordDataRight, 2, lay);


	//复仇
	CButton *pReven = (CButton *)lay->findWidgetById("play");
	if(pRecord.record_id() > 0)
	{
		pReven->setTag(uIdx);
		pReven->setOnClickListener(this, ccw_click_selector(CPvpRecord::onReven));
	}
	else
	{
		pReven->setVisible(false);
	}

	while (lay->getChildrenCount()>0)
	{
		CCNode *child = (CCNode*)lay->getChildren()->objectAtIndex(0);
		child->removeFromParent();
		pCell->addChild(child);
	}

}



CPvpRecord::~CPvpRecord()
{
	CC_SAFE_RELEASE(m_cell);
	CC_SAFE_RELEASE(m_heroLay);
}


void CPvpRecord::ProcessMsg(int type, google::protobuf::Message *msg)
{
	if(type != PvpRecordResMsg)
	{
		onClose(nullptr);
		return;
	}

	setVisible(true);

	PvpRecordRes *pRes = (PvpRecordRes *)msg;

	m_recordRes = *pRes;

	updateUI();

}


void CPvpRecord::setOptionType(int type)
{

}

void CPvpRecord::updateUI()
{
	//列表更新
	int iRecordNum = m_recordRes.record_list().size();
	m_tableView->setCountOfCell(iRecordNum);
	m_tableView->reloadData();


}

void CPvpRecord::updateTeamInfo( const RecordData& recordData, int iTag, CLayout *pParent )
{
	//头像
	CImageView *pHead = (CImageView *)pParent->findWidgetById(CCString::createWithFormat("headbg%d", iTag)->getCString());
	if (recordData.role_thumb()>0)
	{
		CCString* pHeadStr = CCString::createWithFormat("headImg/%d.png", recordData.role_thumb());
		std::string strFullPathHead = CCFileUtils::sharedFileUtils()->fullPathForFilename(pHeadStr->getCString());
		if(CCFileUtils::sharedFileUtils()->isFileExist(strFullPathHead))
		{
			pHead->setTexture(CCTextureCache::sharedTextureCache()->addImage(pHeadStr->getCString()));
		} 
	}
	else
	{
		//加载facebook头像
		string fbName = recordData.fb_id()+".jpg";
		string fullName = CCFileUtils::sharedFileUtils()->fullPathForFilename(fbName.c_str());
		bool isFileExist = CCFileUtils::sharedFileUtils()->isFileExist(fullName);		
		if(CCFileUtils::sharedFileUtils()->isFileExist(fullName))
		{
			pHead->setTexture(CCTextureCache::sharedTextureCache()->addImage(fullName.c_str()));
		} 
		else
		{
			CCString *imgUrl = CCString::createWithFormat(FACEBOOKIMG, recordData.fb_id().c_str());
			HttpLoadImage::getInstance()->requestUrlImage(imgUrl->getCString(), recordData.fb_id().c_str());
		}
	}

	//名称
	CLabel *pName = (CLabel *)pParent->findWidgetById(CCString::createWithFormat("name%d", iTag)->getCString());
	pName->setString(recordData.role_name().c_str());

	//等级
	CLabel *pLevel = (CLabel *)pParent->findWidgetById(CCString::createWithFormat("level%d", iTag)->getCString());
	pLevel->setString(ToString(recordData.role_level()));


	//排名上升还是下降
	//排名变化情况
	CLabelAtlas *pRankChange = (CLabelAtlas *)pParent->findWidgetById(CCString::createWithFormat("rank%d", iTag)->getCString());
	CImageView *pBattleResult = (CImageView *)pParent->findWidgetById(CCString::createWithFormat("result%d", iTag)->getCString());
	CImageView *pRankTip = (CImageView *)pParent->findWidgetById(CCString::createWithFormat("arrow%d", iTag)->getCString());
	int iRankChange =  recordData.rank();
	if(iRankChange>0)
	{
		pRankTip->setTexture(CCTextureCache::sharedTextureCache()->addImage("pvp/arrow_green.png"));
		pBattleResult->setTexture(CCTextureCache::sharedTextureCache()->addImage("pvp/pvp_win.png"));
		pRankChange->setTexture(CCTextureCache::sharedTextureCache()->addImage("pvp/number_up.png"));
	}
	else
	{
		pRankTip->setTexture(CCTextureCache::sharedTextureCache()->addImage("pvp/arrow_red.png"));
		pBattleResult->setTexture(CCTextureCache::sharedTextureCache()->addImage("pvp/pvp_lost.png"));
		pRankChange->setTexture(CCTextureCache::sharedTextureCache()->addImage("pvp/number_down.png"));
	}
	
	pRankChange->setString(ToString(abs(iRankChange)));
	pRankChange->setScale(iRankChange>=1000?0.8f:1.0f);

	//队伍
	int iPosXGap = (iTag-1)*445;
	int iSize = recordData.teams().size();
	for (int i = 0; i < iSize; i++)
	{
		CLayout *heroLay = UICloneMgr::cloneLayout(m_heroLay);
		const Hero& hero = recordData.teams().Get(iSize-1-i);
		const HeroInfoData *data = DataCenter::sharedData()->getHeroInfo()->getCfg(hero.thumb());
		while (heroLay->getChildrenCount()>0)
		{
			CCNode *node = (CCNode *) heroLay->getChildren()->objectAtIndex(0);
			node->removeFromParent();
			pParent->addChild(node);
			node->setPositionX(node->getPositionX()+50*i+iPosXGap);

			if(node->getTag() == 2)
			{
				CImageView *pMask = (CImageView *)node;
				//品质框
				if(data)
				{
					pMask->setTexture(SetRectColor(data->iType1));
				}

				//添加星星
				CLayout* pStarLayout = getStarLayout(hero.color());
				pMask->addChild(pStarLayout, 10);
			}
			else if(node->getTag() == 1)
			{
				//获取头像底板
				CImageView *pBoardHead = (CImageView *)node;

				//生成头像
				CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png", hero.thumb())->getCString());
				if (!head)
				{
					head = CCSprite::create("headImg/101.png");
					CCLOG("[ ERROR ]CPvpRecord::init Lost headImg/%d.png",hero.thumb());
				}
				pBoardHead->addChild(head);
				NodeFillParent(head);
			}
		}
	}
}

void CPvpRecord::imageLoadSuccessCallBack( string sTag, vector<char>* pBuffer )
{
	//
	CCImage* img = new CCImage;
	img->initWithImageData((unsigned char*)pBuffer->data(), pBuffer->size());
	CCTexture2D* texture = new CCTexture2D();
	texture->initWithImage(img);
	//保存facebook 头像
	string path = HttpLoadImage::getInstance()->getStoragePath("download/fbImg",sTag.c_str())+".jpg";
	string buff(pBuffer->begin(), pBuffer->end());
	CCLOG("path: %s", path.c_str());
	FILE *fp = fopen(path.c_str(), "wb+");
	fwrite(buff.c_str(), 1, pBuffer->size(),  fp);
	fclose(fp);

	//从当前cell中找到某一条信息里是否有和当前fb头像同名的
	CCArray *pChildren = m_tableView->getContainer()->getChildren();
	CCObject *pObj;
	CCARRAY_FOREACH(pChildren, pObj)
	{
		CCNode *pCell = (CCNode *)pObj;
		int iTag = pCell->getTag();
		const Record &pRecord = m_recordRes.record_list().Get(iTag);

		CImageView *pHead = nullptr;

		//防守方-蓝-play2
		const RecordData& recordDataLeft = pRecord.player2();
		if( recordDataLeft.fb_id() == sTag)
		{
			pHead = (CImageView *)m_ui->findWidgetById("headbg1");
		}

		//进攻方-红-play1
		const RecordData& recordDataRight = pRecord.player1();
		if( recordDataRight.fb_id() == sTag)
		{
			pHead = (CImageView *)m_ui->findWidgetById("headbg2");
		}
		
		if(pHead)
		{
			pHead->setTexture(CCTextureCache::sharedTextureCache()->addImage(path.c_str()));
		}
	};
}

void CPvpRecord::onReven( CCObject *pSender )
{
	//复仇
	CCNode *pNode = (CCNode *)pSender;
	int uIdx = pNode->getTag();
	const Record &pRecord = m_recordRes.record_list().Get(uIdx);

	//防守方-蓝-play2
	const RecordData& recordDataLeft = pRecord.player2();

	//进入选人界面
	CSelectPvpArmy *pSelArmy = CSelectPvpArmy::create();
	pSelArmy->setRoleId(pRecord.record_id());
	pSelArmy->setIsRobot(false);
	pSelArmy->setEnemyRoleName(recordDataLeft.role_name());
	pSelArmy->setEnemyRoleHead(recordDataLeft.role_thumb());
	pSelArmy->setEnemyRoleFacebook(recordDataLeft.fb_id());
	pSelArmy->setReven(true);
	LayerManager::instance()->push(pSelArmy);

	AskForGoToSelectPvpArmy(pRecord.record_id(), false);
	CPlayerControl::getInstance().sendUnion(0, 0);
}



