#include "scene/LoadScene.h"
#include "SimpleAudioEngine.h"
#include "Global.h"
#include "Battle/AnimationManager.h"
#include "tools/ToolDefine.h"
#include "model/DataCenter.h"
#include "common/CommonFunction.h"
#include "Battle/EffectObject.h"
#include "model/DataCenter.h"
#include "model/WarManager.h"

using namespace CocosDenshion;

LoadScene::LoadScene()
:m_tip(nullptr)
,m_currNum(0)
, m_totalNum(0)
, m_currTip(0)
, m_currTipNum(0)
, m_time(0)
,m_asynLoadNum(0)
,m_fPercent(0)
,m_iPlistIndex(0)
{}
using namespace BattleSpace;
void LoadScene::onCreate()
{
	CWidgetWindow* layout = CWidgetWindow::create();
	layout->setTag(1);
	addChild(layout);

	CColorView *clorView = CColorView::create(ccc4(0,0,0,255));
	clorView->setContentSize(CCDirector::sharedDirector()->getWinSize());
	clorView->setPosition(VCENTER);
	this->addChild(clorView);

	m_ui = LoadComponent("loading.xaml");
	m_ui->setPosition(VCENTER);
	this->addChild(m_ui);

	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("skill/9049.plist");
	AnimationManager::sharedAction()->ParseAnimation("9049");

	schedule(schedule_selector(LoadScene::updatePercent));

}
void LoadScene::onEnter()
{
	CScene::onEnter();
	CSVFile* file = (CSVFile*)FileUtils::sharedFileUtils()->loadCSVFile(CSV_ROOT("preloadRes.csv"));
	m_totalNum = file->getRowNum();
	--m_totalNum;
	m_currNum = 0;
	file = (CSVFile*)FileUtils::sharedFileUtils()->loadCSVFile(CSV_ROOT("loadTips.csv"));
	m_currTip = 0;
	m_time = 0;
	this->schedule(schedule_selector(LoadScene::load));
	this->schedule(schedule_selector(LoadScene::load2), 0.025f);

	//进度条
	m_progress = (CProgressBar*)m_ui->findWidgetById("progress");
	m_progress->setMaxValue(100);

	//僵尸跳
	CCAnimation *pZombieEffect = AnimationManager::sharedAction()->getAnimation("9049");
	pZombieEffect->setDelayPerUnit(0.05f);
	CCAnimate* pAnimate = CCAnimate::create(pZombieEffect);
	CCSprite* pZombieSprite = CCSprite::create();
	pZombieSprite->setScale(0.9f);
	pZombieSprite->setAnchorPoint(ccp(0.5f, 0.0f));
	pZombieSprite->setPositionY(m_progress->getPositionY()+5);
	m_ui->addChild(pZombieSprite);
	pZombieSprite->runAction(CCRepeatForever::create(pAnimate));
	m_pZombieSprite = pZombieSprite;

	updatePercent(0);

	CCSprite* backgroundImage = (CCSprite*)m_ui->findWidgetById("bg");
	int newID = 0;			
	int stage = DataCenter::sharedData()->getWar()->getStageID();
	if (!stage||stage==101)								//（关卡0和关卡101特殊处理）
	{
		newID = stage;
	}else{
		int oldID = DataCenter::sharedData()->getWar()->getLoadImage();
		do{
			newID = CCRANDOM_0_1()*9;					//图片总数
			CCLOG("----------%d",newID);
			if (newID != oldID)
				break;
		} while (true);
	}

	DataCenter::sharedData()->getWar()->setLoadImage(newID);
	char path [60] = {0};
	sprintf(path,"warScene/LoadImage/%d.png",newID);
	if (!backgroundImage->initWithFile(path))
	{
		CCLOG("[ *ERROR ] LoadWar::BackImage BMG initWithFile Fail");
		backgroundImage->initWithFile("warScene/LoadImage/0.jpg");			//容错性处理
	}
}

void LoadScene::onExit()
{
	CScene::onExit();
	FileUtils::sharedFileUtils()->releaseFile(CSV_ROOT("loadTips.csv"));
	FileUtils::sharedFileUtils()->releaseFile(CSV_ROOT("preloadRes.csv"));
}

void LoadScene::load(float fdetal)
{
	CSVFile* file = (CSVFile*)FileUtils::sharedFileUtils()->getFile(CSV_ROOT("preloadRes.csv"));
	int resType = atoi(file->get(m_currNum+1,0));
	const char* url = file->get(m_currNum+1,1);
	switch(resType)
	{
	case LOAD_RES_MODEL:
		{
			//先判断是角色还是特效然后判断是否需要加载plist文件进缓存
			const char* model = file->get(m_currNum+1,2);
			int type = atoi(file->get(m_currNum+1,3));
			string str = url;
			int pos = str.find("plist");
			if (pos!=string::npos)
			{
				string pngStr = str.substr(0,pos);
				pngStr = pngStr.append("png");
				CCTextureCache::sharedTextureCache()->addImageAsync(pngStr.c_str(),this,callfuncO_selector(LoadScene::loadAsyncImg));
				LoadResourceInfo res;
				res.FileName = model;
				res.FilePath = url;
				res.Loadtype = LoadType::Load_Effect;
				m_resVec.push_back(res);
			}
			++m_currNum;
		}break;
	case LOAD_BACKGROUND_SND:
		{
			char str[30]={0};
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
			sprintf(str,"BGM/Test/%d.mp3",atoi(file->get(m_currNum+1,1)));
#else
			sprintf(str,"BGM/%d.ogg",atoi(file->get(m_currNum+1,1)));
#endif
			CCLOG("Load BGM %s",file->get(m_currNum+1,1));
			SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic(url);
			++m_currNum;
		}break;
	case LOAD_EFFECT_SND:
		{
			char str[30]={0};
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
			sprintf(str,"SFX/Test/%d.mp3",atoi(file->get(m_currNum+1,1)));
#else
			sprintf(str,"SFX/%d.ogg",atoi(file->get(m_currNum+1,1)));
#endif
			CCLOG("Load SFX %s",file->get(m_currNum+1,1));
			SimpleAudioEngine::sharedEngine()->preloadEffect(url);
			++m_currNum;
		}break;
	case LOAD_FONT:{   ++m_currNum;    }
		break;
	default:
		break;
	}

	m_fPercent = (m_currNum+m_iPlistIndex+1)*100.0f/(m_totalNum+m_resVec.size());
}


void LoadScene::load2( float fdetal )
{
	//加载plist
	if(m_iPlistIndex < m_resVec.size())
	{
		LoadResourceInfo &res = m_resVec.at(m_iPlistIndex);

		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(res.FilePath.c_str());//将plist文件加载进入缓存
		CCLOG("load mode %s ,\n",res.FileName.c_str());
		if(res.Loadtype == LoadType::Load_FrameAnimation)
		{
			AnimationManager::sharedAction()->ParseAnimation(res.FileName.c_str(),eFrameRole);
		}
		else if(res.Loadtype == LoadType::Load_Effect)
		{
			AnimationManager::sharedAction()->ParseAnimation(res.FileName.c_str());
		}
		m_iPlistIndex++;
	}
}


void LoadScene::loadAsyncImg(CCObject* pSender)
{
	CCTexture2D *texture = (CCTexture2D*)pSender;
	m_asynLoadNum++;
}

void LoadScene::updatePercent(float dt)
{
	//进度条
	m_progress->setValue(m_fPercent);
	//僵尸位置
	m_pZombieSprite->setPositionX(m_progress->getPositionX()+m_progress->getContentSize().width*m_fPercent/100-5);
	if(m_fPercent>=100)
	{
		this->unschedule(schedule_selector(LoadScene::updatePercent));
		this->unschedule(schedule_selector(LoadScene::load));
		this->unschedule(schedule_selector(LoadScene::load2));
		CSceneManager::sharedSceneManager()->replaceScene(GETSCENE(CMainScene), 1.0f);
	}
}
