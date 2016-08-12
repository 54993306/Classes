#include "SharpTollgate.h"
#include "Global.h"
#include "common/CGameSound.h"
#include "UserDefaultData.h"
#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "jni/CJniHelper.h"
#include "netcontrol/CPlayerControl.h"
#include "common/ShaderDataHelper.h"
#include "GMessage.h"
#include "model/DataDefine.h"

#include "tools/UICloneMgr.h"
#include "StageData.h"
#include "model/DataCenter.h"
#include "Battle/WarManager.h"
#include "Sweep.h"
#include "TollgatePreview.h"
#include "Battle/BattleCenter.h"
#include "Battle/AnimationManager.h"

using namespace BattleSpace;

CSharpTollgate::CSharpTollgate():m_ui(nullptr),m_chapter(0),m_openChapter(0)
{

}

CSharpTollgate::~CSharpTollgate()
{

}

bool CSharpTollgate::init()
{
	if (BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		//黑底
		MaskLayer* pMaskLayer = MaskLayer::create("tollgMask");
		pMaskLayer->setContentSize(winSize);
		LayerManager::instance()->push(pMaskLayer, true);

		m_ui = LoadComponent("SharpTollgate.xaml");
		m_ui->setTag(1);
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		this->setVisible(true);

		return true;
	}
	return false;
}

void CSharpTollgate::onEnter()
{
	BaseLayer::onEnter();

	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CSharpTollgate::onClose));
	this->addChild(pClose, 999);


	m_cell = (CLayout*)(m_ui->findWidgetById("Cell"));
// 	m_cell->retain();
// 	m_ui->removeChild(m_cell);

	
	CCAnimation *bgAnim = AnimationManager::sharedAction()->getAnimation("8055");
	bgAnim->setDelayPerUnit(0.05f);
	CCSprite *bg = createAnimationSprite("skill/8055.png",VCENTER,bgAnim,true);
	bg->setScale(3.0f);
	m_ui->addChild(bg);

	m_tableView = (CTableView *)(m_ui->findWidgetById("scroll"));
	m_tableView->setDirection(eScrollViewDirectionHorizontal);
	m_tableView->setSizeOfCell(m_cell->getContentSize());
	//m_tableView->setSizeOfCell(CCSizeMake(790,115));
	m_tableView->setCountOfCell(0);
	m_tableView->setBounceable(false);
	m_tableView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CSharpTollgate::tableviewDataSource));	
	m_tableView->reloadData();

}

void CSharpTollgate::onExit()
{
	BaseLayer::onExit();	
	if (LayerManager::instance()->getLayer("CActivityLayer"))
	{
		NOTIFICATION->postNotification(SHOW_TOP_LAYER);
		NOTIFICATION->postNotification(SHOW_TOP_LAYER);
	}
}

CCObject* CSharpTollgate::tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
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

void CSharpTollgate::onClose( CCObject * pSender )
{
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CSharpTollgate::addTableCell(unsigned int uIdx, CTableViewCell * pCell)
{
	const vector<StageWidget> *data = DataCenter::sharedData()->getStageData()
		->getStageWidgets(m_chapter);
	CStage &stage = m_stageList.at(uIdx);

	for (int i = 0; i < 2; i++)
	{
		CCNode * node = (CCNode*)m_cell->getChildren()->objectAtIndex(i);

		if (node->getTag()==1)
		{
			const StageWidget *widget = nullptr;			
			CCString *strId = CCString::createWithFormat("hero%d",uIdx+1);
			for (int j=0; j<data->size();++j)
			{
				widget = &data->at(j);
				if (widget->widgetId!=""&&widget->widgetId.compare(strId->getCString())==0)
				{				
					CButton *btn =  CButton::create(widget->normalImage.c_str());
					btn->setScaleX(widget->scaleX);
					btn->setScaleY(widget->scaleY);
					btn->setPosition(ccp(btn->boundingBox().size.width/2, 80/*btn->boundingBox().size.height*/));  
					btn->setAnchorPoint(ccp(0.5, 0.0));
					btn->setUserData(&m_stageList.at(uIdx));
					btn->setOnClickListener(this,ccw_click_selector(CSharpTollgate::onBattle));
					btn->setTag(1);
					pCell->addChild(btn);
					if (!stage.isOpen)
					{
						btn->getNormalImage()->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
// 						btn->getSelectedImage()->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
					}
					break;
				}
			}
		}
		else if (node->getTag()==2)
		{
			CImageView *image = UICloneMgr::cloneImageView((CImageView*)node);
			if (stage.star>0)
			{		
				image->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("tollgate/star_%d.png",stage.star)->getCString()));
			}
			else
			{
				image->setTexture(CCTextureCache::sharedTextureCache()->addImage("tollgate/star_3.png"));
				image->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
			}
			image->setAnchorPoint(ccp(0.5f,0));
			pCell->addChild(image);
		}
	}
	pCell->setVisible(false);
	pCell->setScale(1.15f);
	pCell->runAction(CCSequence::create(CCDelayTime::create(0.1f+0.15f*uIdx),CCShow::create(),CCScaleTo::create(0.05f,1.0f),CCCallFuncN::create(this,callfuncN_selector(CSharpTollgate::heroCall)),nullptr));

	if (uIdx==3)
	{
		pCell->setPositionX(pCell->getPositionX()+30);
	}
}

void CSharpTollgate::heroCall(CCNode* pNode)
{
	CTableViewCell *node = (CTableViewCell*)pNode;
	CButton *btn = (CButton*)node->getChildByTag(1);
	//人物会动
	CCScaleTo *tobig = CCScaleTo::create(0.55f,1.0*btn->getScaleX(),1.0*btn->getScaleY());
	CCScaleTo *toSmall = CCScaleTo::create(0.55f,0.96*btn->getScaleX(), 0.96*btn->getScaleY());
	btn->runAction(CCRepeatForever::create(CCSequence::create(toSmall,tobig,nullptr)));
}

void CSharpTollgate::setStageList(const vector<CStage>& stageList, int chapter, int openChapter)
{
	m_stageList = stageList;
	m_chapter = chapter;	
	m_openChapter = openChapter;
	m_tableView->setCountOfCell(stageList.size());
	m_tableView->reloadData();

	CTableViewCell *cell = m_tableView->cellAtIndex(3);
	cell->setPositionX(cell->getPositionX()+65);

	CImageView *name = (CImageView*)(m_ui->findWidgetById("name"));
	CCTexture2D *texture = CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("tollgate/chapter_%d.png",chapter)->getCString());
	if(!texture)
	{
		texture = CCTextureCache::sharedTextureCache()->addImage("tollgate/chapter_1.png");
	}
	name->setTexture(texture);
	name->setTextureRect(CCRectMake(0,0,texture->getContentSize().width,texture->getContentSize().height));
}

void CSharpTollgate::onBattle(CCObject* pSender)
{
	PlayEffectSound(SFX_Button);

	CButton *btn = (CButton*)pSender;
	CStage *stage = (CStage*)btn->getUserData();
	int zorder = btn->getZOrder();
	if (stage&&stage->isOpen)
	{
		BattleManage->setLastStageId(stage->id); 
		if (stage->star>=1)
			BattleManage->setFirstStage(false);
		//DataCenter::sharedData()->getWar()->setTollgate(*stage); 
		BattleManage->setStageType(zorder);
		BattleManage->setNormal(false);
		BattleManage->setChapterIndex(m_chapter);
		BattleManage->setChapterCount(m_openChapter);

		CTollgatePreview *preview = CTollgatePreview::create();
		preview->setIsLastStageInChapter(stage->id==m_stageList.at(m_stageList.size()-1).id&&stage->star<=0);
		LayerManager::instance()->push(preview);
		preview->setStage(stage->id, stage->name.c_str());
		CStageInfoRes *ir = DATAPOOL->getStageInfoById(stage->id);		
		preview->setNormal(false);
		if (ir)
		{
			preview->showStageInfo(*ir);
		}
		else
		{
			CPlayerControl::getInstance().sendStageInfo(stage->id);
		}
	}
}
