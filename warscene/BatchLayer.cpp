#include "BatchLayer.h"
#include "model/DataCenter.h"
#include "model/DataDefine.h"
#include "common/CommonFunction.h"
#include "model/WarManager.h"

BatchLayer::BatchLayer()
	:m_batchLayer(nullptr)
	,m_BatchPro(nullptr)
	,m_HpPro(nullptr)
{

}

BatchLayer::~BatchLayer()
{
	REMOVE_TARGET(m_batchLayer);
	CC_SAFE_RELEASE(m_batchLayer);
	m_batchLayer = nullptr;
}

bool BatchLayer::init()
{
	bool res = CCNode::init();
	do 
	{
		CC_BREAK_IF(!res);
		m_batchLayer  = LoadComponent("warbatch.xaml");
		m_batchLayer->retain();
		m_batchLayer->setPosition(CCSizeCenter(m_batchLayer->getContentSize()));
		//批次条
		CCSprite* batch_bar = (CCSprite*)m_batchLayer->getChildByTag(barchBar_tag);
		CCPoint pRightDown = batch_bar->getPosition();
		m_BatchPro = CProgressBar::create("public/bar_yellow2.png");
		m_BatchPro->setDirection(eProgressBarDirectionRightToLeft);
		m_BatchPro->setPosition(pRightDown);
		m_BatchPro->retain();
		m_batchLayer->removeChildByTag(barchBar_tag);
		m_batchLayer->addChild(m_BatchPro);
		//血量条
		CCSprite* hpbar = (CCSprite*)m_batchLayer->getChildByTag(hpBar_tag);
		CCPoint hp_point = hpbar->getPosition();
		m_HpPro = CProgressBar::create("public/bar_red.png");
		m_HpPro->setDirection(eProgressBarDirectionLeftToRight);
		m_HpPro->setPosition(hp_point);
		m_HpPro->setValue(100);
		m_HpPro->retain();
		m_batchLayer->removeChildByTag(hpBar_tag);
		m_batchLayer->addChild(m_HpPro);
		//CCProgressFromTo* proto = CCProgressFromTo::create(0.5f,10,80);

		CCNode* xueliang = (CCNode*)m_batchLayer->getChildByTag(hpNum_tag);
		CCPoint _point = xueliang->getPosition();
		CCLabelAtlas* hpPe = CCLabelAtlas::create(ToString(0),"label/number_03.png",27.0f,35,47);
		hpPe->setAnchorPoint(ccp(1,0));
		hpPe->setPosition(ccpAdd(_point,ccp(-xueliang->getContentSize().width/2,-xueliang->getContentSize().height/2)));
		hpPe->setTag(hpPe_tag);
		hpPe->setString(ToString( setHpPe() ));
		m_batchLayer->addChild(hpPe);

		CCNode* weizhi = (CCNode*)m_batchLayer->getChildByTag(ziti_tag);
		CCPoint point = weizhi->getPosition();
		CCLabelAtlas* LabAllbatch = CCLabelAtlas::create(ToString(0),"label/number_03.png",27.0f,35,47);
		LabAllbatch->setPosition(ccpAdd(point,ccp(weizhi->getContentSize().width/2,-weizhi->getContentSize().height/2)));
		LabAllbatch->setTag(allBatch_tag);
		m_batchLayer->addChild(LabAllbatch);

		CCLabelAtlas* LabCurbatch = CCLabelAtlas::create(ToString(0),"label/number_03.png",27.0f,35,47);
		LabCurbatch->setPosition(ccpAdd(point,ccp(-weizhi->getContentSize().width/2,-weizhi->getContentSize().height/2)));
		LabCurbatch->setTag(currBatch_tag);
		m_batchLayer->addChild(LabCurbatch);

		CCLabelAtlas* Labxiegang = CCLabelAtlas::create("/","label/number_03.png",27.0f,35,47);
		Labxiegang->setPosition(ccpAdd(point,ccp(0,-weizhi->getContentSize().height/2)));
		Labxiegang->setTag(xiegan_tag);
		m_batchLayer->addChild(Labxiegang);
		m_batchLayer->removeChildByTag(ziti_tag);
		this->addChild(m_batchLayer);
	} while (0);
	return res;
}

void BatchLayer::setBatch(int totalBatch,int currBatch,bool reset)
{
	float percent = (currBatch*1.0f)/totalBatch *100;
	//CCLOG("percent=%f",percent);
	float oldPe = m_BatchPro->getPercentage() * 100;
	CCSprite* gost = (CCSprite*)m_batchLayer->getChildByTag(sword_tag);
	float x = m_BatchPro->getContentSize().width * (percent-oldPe) * 0.01;
	if (gost)
	{
		CCPoint point = ccpAdd(gost->getPosition(),ccp(-x,0));
		CCMoveBy* mb = CCMoveBy::create(0.5f,ccp(-x,0));
		gost->runAction(mb);
	}
	m_BatchPro->startProgressFromTo(oldPe,percent,0.5f);
	//if(percent == 100)CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic(BOSS_BG,true);   //最后一波播放boss音效

	CCLabelAtlas* hpPe = (CCLabelAtlas*)m_batchLayer->getChildByTag(hpPe_tag);
	if (reset)hpPe->setString(ToString( setHpPe() ));
	CCLabelAtlas* allbatch = (CCLabelAtlas*)m_batchLayer->getChildByTag(allBatch_tag);
	if (!outPutERRORMsg("WarBackLayer::setBatch",allbatch))allbatch->setString(ToString(totalBatch+1));
	
	CCLabelAtlas* currbatch = (CCLabelAtlas*)m_batchLayer->getChildByTag(currBatch_tag);
	if (!outPutERRORMsg("WarBackLayer::setBatch",currbatch))currbatch->setString(ToString(currBatch+1));
}

int BatchLayer::setHpPe()
{
	//float hpnum = DataCenter::sharedData()->getWar()->getCurrHp();
	//float totalHp = DataCenter::sharedData()->getWar()->getHerosHp();
	//float pe = hpnum/totalHp * 100;
	//float oldPe = m_HpPro->getPercentage()*100;
	//m_HpPro->startProgressFromTo(oldPe,pe,0.5f);
	//CCLOG("pe=%f",pe);
	return 1/*pe*/;
}