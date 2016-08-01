#include "HeroEvolveDataPanel.h"
#include "tools/UICloneMgr.h"
#include "model/DataDefine.h"
#include "common/CommonFunction.h"
#include "Battle/AnimationManager.h"
#include "model/DataCenter.h"

using namespace BattleSpace;

CHeroEvolveDataPanel::CHeroEvolveDataPanel()
	:m_pUi(nullptr), m_pHero(nullptr),m_pCellLay(nullptr), m_pBaseInfoLay(nullptr), m_fBaseInfoPanelHeight(0.0f)
	,m_pBgImageScale9(nullptr), m_pBaseHero(nullptr)
{

}


CHeroEvolveDataPanel::~CHeroEvolveDataPanel()
{
	CC_SAFE_RELEASE(m_pCellLay);
}

bool CHeroEvolveDataPanel::init()
{
	if (BaseLayer::init())
	{
		m_pUi = LoadComponent("HeroEvolveDataPanel.xaml");  //
		m_pUi->setPosition(VCENTER);
		this->addChild(m_pUi);

		this->setIsShowBlack(false);
		this->setVisible(false);

		initDataUI();

		return true;
	}
	return false;
}


void CHeroEvolveDataPanel::initDataUI()
{
	//TODO
	//8个增长属性-默认都显示
	m_pBaseInfoLay = (CLayout*)m_pUi->findWidgetById("data");

	//1阶-三个技能-（找开放参数为0的 ）
	//2阶-1个技能-（找开放参数不为0的）
	//3阶-装备孔（数据配在技能表，所有英雄都一样）
	//4阶-特殊技能1
	//5阶-特殊技能2
	//备注: 默认开放的技能，开放参数是0


	//技能cell
	m_pCellLay = (CLayout*)m_pUi->findWidgetById("Cell");
	CC_SAFE_RETAIN(m_pCellLay);
	m_pCellLay->removeFromParentAndCleanup(false);

	//默认克隆一个cell
	insertOneSkillPanel();


	//标记位置-line做标记
	CCNode *pLine = (CCNode*)m_pUi->findWidgetById("line");
	m_pStartFlagPos = pLine->getPosition() - ccp(pLine->getContentSize().width/2, 0);

	//基础信息面板高度
	CCNode *pBaseDataPanelBg = (CCNode*)m_pBaseInfoLay->findWidgetById("bg");
	m_fBaseInfoPanelHeight = pBaseDataPanelBg->getContentSize().height*pBaseDataPanelBg->getScaleY();

	//可拉伸的背景框
	m_pBgImageScale9 = (CImageViewScale9 *)m_pUi->findWidgetById("bg1");
	m_pBgImageScale9->setAnchorPoint(ccp(0.5f, 1.0f));
	m_pBgImageScale9->setPositionY(m_pBgImageScale9->getPositionY()+m_pBgImageScale9->getContentSize().height/2);
}


void CHeroEvolveDataPanel::insertOneSkillPanel()
{
	CLayout *pSkillPanel = UICloneMgr::cloneLayout(m_pCellLay);
	pSkillPanel->setAnchorPoint(CCPointZero);
	pSkillPanel->setVisible(false);
	m_pUi->addChild(pSkillPanel, 90);

	m_pSkillLayVec.push_back(pSkillPanel);
}


void CHeroEvolveDataPanel::fitSkillPanelVec( int iSize )
{
	int iGapSize = iSize - m_pSkillLayVec.size();
	if(iGapSize<0)
	{
		return;
	}
	for(int i=0; i<iGapSize; i++)
	{
		insertOneSkillPanel();
	}
}

void CHeroEvolveDataPanel::updateUI()
{
	//更新标题
	CLabel *pTitle = (CLabel *)m_pUi->findWidgetById("title");
	pTitle->setString(GETLANGSTR(2004+m_pHero->quality));

	//Warning特殊处理
	if(m_pHero->quality == 6)
	{
		pTitle->setString(GETLANGSTR(2018));
	}

	//更新技能信息
	updateSkillInfo();

	//根据不同阶级执行不同策略
	switch (m_pHero->quality)
	{
	case 1:
		{
			showBaseInfo();
			//hideBaseInfo();
		}break;
	case 2:
		{
			showBaseInfo();
		}break;
	case 3:
		{
			showBaseInfo();
		}break;
	case 4:
		{
			showBaseInfo();
		}break;
	case 5:
		{
			showBaseInfo();
		}break;
	case 6://特殊
		{
			hideBaseInfo();
		}break;
	default:
		break;
	}

	//自动适配位置
	autoFixPos();
}


void CHeroEvolveDataPanel::onEnter()
{
	BaseLayer::onEnter();
}

void CHeroEvolveDataPanel::onExit()
{
	BaseLayer::onExit();
}


void CHeroEvolveDataPanel::show( CHero *pBaseHero,  CHero *pHero /*= nullptr*/, bool bForceChange/*=false*/ )
{
	this->setVisible(true);

	if(pHero != nullptr)
	{
		if( m_pBaseHero != pBaseHero || m_pHero != pHero || bForceChange)
		{
			m_pHero = pHero;
			m_pBaseHero = pBaseHero;
			updateUI();
		}
	}
}

void CHeroEvolveDataPanel::hide()
{
	this->setVisible(false);
}

vector<CSkill*> CHeroEvolveDataPanel::getRankOpenSkill( int iRank )
{
	vector<CSkill*> vec;

	//特殊处理,3,4,5阶-用skill1, skill6, skill7
	switch (iRank)
	{
	case 1:
		{
			//遍历英雄的技能-其实只关注三个 3,4,5
			CSkill *pSkill[3] = {&m_pBaseHero->skill3, &m_pBaseHero->skill4, &m_pBaseHero->skill5};
			for(int i=0; i<3; i++)
			{
				CSkill *pSkillOne = pSkill[i];
				if( pSkillOne->star==0 &&iRank==1 ) //后面这个条件是1阶的特殊情况
				{
					vec.push_back(pSkillOne);
				}
			}
		}break;
	case 2:
		{
			if( m_pBaseHero->skill2.skillId>10 && m_pBaseHero->skill2.star==iRank ) //1有配技能，10以上吧
			{
				vec.push_back(&m_pBaseHero->skill2);
			}
		}break;
	case 3:
		{
			if(m_pBaseHero->iColor >= 3)
			{
				m_pBaseHero->skill1.skillId = 70000;
				vec.push_back(&m_pBaseHero->skill1);
			}
		}break;
	case 4:
		{
			if(m_pBaseHero->iColor >= 4)
			{
				m_pBaseHero->skill6.skillId = 80000;
				vec.push_back(&m_pBaseHero->skill6);
			}
		}break;
	case 5:
		{
			if(m_pBaseHero->iColor >= 5)
			{
				m_pBaseHero->skill7.skillId = 90000;
				vec.push_back(&m_pBaseHero->skill7);
			}
		}break;
	case 6:
		{
			m_pBaseHero->skill7.skillId = 90000;
			vec.push_back(&m_pBaseHero->skill7);
		}break;
	default:
		break;
	}

	return vec;
}

void CHeroEvolveDataPanel::showBaseInfo()
{
	m_pBaseInfoLay->setVisible(true);
	updateBaseInfo();
}

void CHeroEvolveDataPanel::hideBaseInfo()
{
	m_pBaseInfoLay->setVisible(false);
}

void CHeroEvolveDataPanel::updateBaseInfo()
{
	//TODO
	//int	addHp;						//血量成长系数
	//int	addAtk;						//攻击成长系数
	//int	addDef;					//防御成长系数
	//int	addDex;					//敏捷加成
	//int addHit;						//命中加成
	//int addCrit;						//暴击加成
	//int addDodge;				//闪避加成
	//int	addRenew;				//回复加成
	int iAddValue[8] = 
	{
		m_pHero->hp/*-m_pBaseHero->hp*/, m_pHero->atk/*-m_pBaseHero->atk*/, 
		m_pHero->def/*-m_pBaseHero->def*/, m_pHero->dex/*-m_pBaseHero->dex*/,
		 m_pHero->crit/*-m_pBaseHero->crit*/, m_pHero->dodge/*-m_pBaseHero->dodge*/,
		 m_pHero->renew/*-m_pBaseHero->renew*/,m_pHero->hit/*-m_pBaseHero->hit*/,
	};
	for(int i=0; i<8; i++)
	{
		CLabel *pLabel = (CLabel*)m_pBaseInfoLay->findWidgetById(CCString::createWithFormat("value_%d", i+1)->getCString());
		pLabel->setString(CCString::createWithFormat("%d", iAddValue[i])->getCString());
	}
}

void CHeroEvolveDataPanel::updateSkillInfo()
{
	//获取要显示的技能列表
	vector<CSkill*> skillVec = getRankOpenSkill(m_pHero->quality);
	int iSkillSize = skillVec.size();

	//补充技能面板
	fitSkillPanelVec(iSkillSize);

	//隐藏所有
	hideAllSkillPanel();

	//更新需要的数量
	for(int i=0; i<iSkillSize; i++)
	{
		CSkill *pSkill = skillVec[i];
		CLayout *pLay = m_pSkillLayVec[i];
		fillOneSkillInfo(pLay, pSkill, isRealSkill(pSkill));
	}
}

void CHeroEvolveDataPanel::fillOneSkillInfo( CLayout *pLay, CSkill *pSkill, bool showExtra )
{
	const SkillCfg *pCFg = DataCenter::sharedData()->getSkill()->getCfg(pSkill->skillId);

	if (pCFg)
	{
		pLay->setVisible(true);

		//ICON
		CImageView *pBg = (CImageView*)pLay->findWidgetById("itembg");
		CImageView *pIcon = CImageView::create(CCString::createWithFormat("skillIcon/%d.png", pCFg->icon)->getCString());
		if(!pIcon)
		{
			CCLOG("ERROR  CHeroEvolveDataPanel::fillOneSkillInfo");
			pIcon  = CImageView::create("skillIcon/100000.png");
		}	
		pBg->removeAllChildrenWithCleanup(true);
		pBg->addChild(pIcon);
		NodeFillParent(pIcon);
		
		//宝箱特殊处理
		{
			if( m_pHero->quality == 3 )
			{
				//MASK隐藏
				CImageView *pMask = (CImageView*)pLay->findWidgetById("mask1");
				pMask->setVisible(false);
			}
			else
			{
				//MASK隐藏
				CImageView *pMask = (CImageView*)pLay->findWidgetById("mask1");
				pMask->setVisible(true);
			}
		}

		//名称
		CLabel *pName = (CLabel*)pLay->findWidgetById("name");
		pName->setString(pCFg->name.c_str());

		//描述
		CLabel *pDesc = (CLabel*)pLay->findWidgetById("desc1");
		pDesc->setString(pCFg->desc.c_str());


		//主/被-cost
		CCNode *pTypeBg = (CCNode*)pLay->findWidgetById("type_bg");
		CLabel *pType = (CLabel*)pLay->findWidgetById("type");
		CCNode *pCostBg = (CCNode*)pLay->findWidgetById("cost");
		pTypeBg->setVisible(showExtra);
		pType->setVisible(showExtra);
		pCostBg->setVisible(showExtra);
		
		if(showExtra)
		{
			pType->setString(GETLANGSTR(getSkillTypeDesId(pSkill)));

			//cost
			//添加火和数字
			if(pSkill->cost>0)
			{
				if(pCostBg->getChildByTag(1)==nullptr)
				{
					CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("skill/9051.plist");//将plist文件加载进入缓存
					AnimationManager::sharedAction()->ParseAnimation("9051");
					CCAnimation *culAnim = AnimationManager::sharedAction()->getAnimation("9051");
					culAnim->setDelayPerUnit(0.1f);
					CCAnimate* pAnimate = CCAnimate::create(culAnim);
					CCSprite* pFire  = CCSprite::create();
					pFire->setPosition(ccp(pCostBg->getContentSize().width/2, pCostBg->getContentSize().height/2+5));
					pCostBg->addChild(pFire, 1, 1);
					pFire->runAction(CCRepeatForever::create(pAnimate));
				}
				pCostBg->removeChildByTag(2);
				CCLabelAtlas* pCostBgAtlas = CCLabelAtlas::create(ToString(pSkill->cost), "label/number2.png", 26, 32, 46);
				pCostBgAtlas->setAnchorPoint(ccp(0.5f, 0.5f));
				pCostBgAtlas->setScale(0.9f);
				pCostBgAtlas->setPosition(ccp(pCostBg->getContentSize().width/2, pCostBg->getContentSize().height/2));
				pCostBg->addChild(pCostBgAtlas, 2, 2);
			}
			else
			{
				pCostBg->setVisible(false);
			}
		}
	}
}

bool CHeroEvolveDataPanel::isInitiativeSkill( CSkill *pSkill )
{
	if(pSkill == &m_pBaseHero->skill3 || pSkill == &m_pBaseHero->skill4)
	{
		return true;
	}
	return false;
}


int CHeroEvolveDataPanel::getSkillTypeDesId( CSkill *pSkill )
{
	if(pSkill == &m_pBaseHero->skill2)
	{
		return 2022;
	}
	else if(pSkill == &m_pBaseHero->skill3)
	{
		return 2019;
	}
	else if(pSkill == &m_pBaseHero->skill4)
	{
		return 2020;
	}
	else if(pSkill == &m_pBaseHero->skill5)
	{
		return 2021;
	}
	else if(pSkill == &m_pBaseHero->skill6)
	{
		return 2023;
	}

	return 2023;
}


void CHeroEvolveDataPanel::autoFixPos()
{
	//TODO
	CCPoint pPos = m_pStartFlagPos;

	//是否有基础信息显示
	pPos.y -= m_pBaseInfoLay->isVisible()?m_fBaseInfoPanelHeight:0;

	//技能列表位置
	bool bNoSkill = true;
	for(int i=0; i<m_pSkillLayVec.size(); i++)
	{
		if(m_pSkillLayVec[i]->isVisible())
		{
			bNoSkill = false;
			pPos.y -= m_pCellLay->getContentSize().height;
			m_pSkillLayVec[i]->setPosition(pPos);
		}
	}

	//没有技能条，补充5像素
	if( bNoSkill )
	{
		pPos.y -= 5;
	}

	//设置背景长度
	m_pBgImageScale9->setContentSize(
		CCSize(
		m_pBgImageScale9->getContentSize().width, 
		m_pStartFlagPos.y-pPos.y+45)
		);

	//整体居中
	float fOffY = VIRTUAL_FIXED_HEIGHT/2+m_pBgImageScale9->getContentSize().height/2-m_pBgImageScale9->getPositionY();
	m_pUi->setPositionY(VIRTUAL_FIXED_HEIGHT/2+fOffY);

}

void CHeroEvolveDataPanel::hideAllSkillPanel()
{
	for(int i=0; i<m_pSkillLayVec.size(); i++)
	{
		m_pSkillLayVec[i]->setVisible(false);
	}
}

bool CHeroEvolveDataPanel::isRealSkill( CSkill *pSkill )
{
	if(pSkill==&m_pBaseHero->skill1/* || pSkill==&m_pBaseHero->skill6*/ || pSkill==&m_pBaseHero->skill7)
	{
		return false;
	}
	return true;
}


CLayout* CHeroEvolveDataPanel::createSkillPanel( CHero *pBaseHero, CHero *pHero /*= nullptr*/, CLayout *pLay/*=nullptr*/ )
{
	CHero *pTempBaseHero = m_pBaseHero;
	CHero *pTempHero = m_pHero;

	m_pBaseHero = pBaseHero;
	m_pHero = pHero;

	//只返回第一个
	//获取要显示的技能列表-只取第一个
	vector<CSkill*> skillVec = getRankOpenSkill(m_pHero->quality);
	if(skillVec.size()>0)
	{
		if(pLay == nullptr)
		{
			pLay = UICloneMgr::cloneLayout(m_pCellLay);
		}
		pLay->setAnchorPoint(CCPointCenter);
		fillOneSkillInfo(pLay, skillVec.at(0), isRealSkill(skillVec.at(0)));
	}

	m_pBaseHero = pTempBaseHero;
	m_pHero = pTempHero;

	if(skillVec.size()<=0)
	{
		return nullptr;
	}
	
	return pLay;
}

HeroEvolveEffectuUnlockType CHeroEvolveDataPanel::getUnLockTypeWithRank( int iRank )
{
	switch (iRank)
	{
	case 1:
	case 2:
		{
			return HeroEvolveEffectuUnlockType::UnlockSkill;
		}break;
	case 3:
		{
			return HeroEvolveEffectuUnlockType::UnlockBox;
		}break;
	case 4:
	case 5:
		{
			return HeroEvolveEffectuUnlockType::UnlockExtra;
		}break;
	default:
		break;
	}

	return HeroEvolveEffectuUnlockType::UnlockSkill;
}

void CHeroEvolveDataPanel::resetAllHeroData()
{
	m_pHero = nullptr;
	m_pBaseHero = nullptr;
}

