#include "scene/LoadScene.h"
#include "SimpleAudioEngine.h"
#include "Global.h"
#include "Battle/AnimationManager.h"
#include "tools/ToolDefine.h"
#include "model/DataCenter.h"
#include "common/CommonFunction.h"
#include "Battle/EffectObject.h"
#include "model/DataCenter.h"
#include "Battle/WarManager.h"

using namespace CocosDenshion;
namespace BattleSpace
{
	LoadScene::LoadScene()
		:mCurrIndex(0)
		,mAsynLoadNum(0)
	{}

	void LoadScene::onCreate()
	{
		CWidgetWindow* layout = CWidgetWindow::create();
		layout->setTag(1);
		addChild(layout);

		m_ui = LoadComponent("loading.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("skill/9049.plist");
		AnimationManager::sharedAction()->ParseAnimation("9049");

	}

	void LoadScene::onEnter()
	{
		CScene::onEnter();
		CSVFile* file = (CSVFile*)FileUtils::sharedFileUtils()->loadCSVFile(CSV_ROOT("preloadRes.csv"));
		schedule(schedule_selector(LoadScene::loadResource));

		//进度条
		mProgress = (CProgressBar*)m_ui->findWidgetById("progress");
		mProgress->setMaxValue(100);

		//僵尸跳
		CCAnimation *pZombieEffect = AnimationManager::sharedAction()->getAnimation("9049");
		pZombieEffect->setDelayPerUnit(0.05f);
		CCAnimate* pAnimate = CCAnimate::create(pZombieEffect);
		CCSprite* pZombieSprite = CCSprite::create();
		pZombieSprite->setScale(0.9f);
		pZombieSprite->setAnchorPoint(ccp(0.5f, 0.0f));
		pZombieSprite->setPositionY(mProgress->getPositionY()+5);
		m_ui->addChild(pZombieSprite);
		pZombieSprite->runAction(CCRepeatForever::create(pAnimate));
		m_pZombieSprite = pZombieSprite;

		CCSprite* tBackround = (CCSprite*)m_ui->findWidgetById("bg");
		tBackround->initWithFile("warScene/LoadImage/1.png");			//容错性处理
	}

	void LoadScene::onExit()
	{
		CScene::onExit();
		FileUtils::sharedFileUtils()->releaseFile(CSV_ROOT("preloadRes.csv"));
	}

	void LoadScene::loadResource(float fdetal)
	{
		CSVFile* tFile = (CSVFile*)FileUtils::sharedFileUtils()->getFile(CSV_ROOT("preloadRes.csv"));
		switch( atoi(tFile->get(mCurrIndex+1,0)) )
		{
		case LOAD_RES_MODEL:
			{
				//先判断是角色还是特效然后判断是否需要加载plist文件进缓存
				string str = tFile->get(mCurrIndex+1,1);
				int pos = str.find("plist");
				if (pos != string::npos)
				{
					string pngStr = str.substr(0,pos);
					pngStr = pngStr.append("png");
					CCTextureCache::sharedTextureCache()->addImageAsync(pngStr.c_str(),this,callfuncO_selector(LoadScene::loadAsyncImg));
					LoadResourceInfo tRes;
					tRes.FilePath = tFile->get(mCurrIndex+1,1);
					tRes.FileName = tFile->get(mCurrIndex+1,2);
					tRes.Loadtype = sLoadType::eEffect;
					mResourceVec.push_back(tRes);
				}
			}break;
		case LOAD_BACKGROUND_SND:
			{
				char str[30]={0};
#if BATTLE_TEST
				sprintf(str,"BGM/Test/%d.mp3",atoi(tFile->get(mCurrIndex+1,1)));
#else
				sprintf(str,"BGM/%d.ogg",atoi(tFile->get(mCurrIndex+1,1)));
#endif
				CCLOG("Load BGM %s",tFile->get(mCurrIndex+1,1));
				SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic(tFile->get(mCurrIndex+1,1));
			}break;
		case LOAD_EFFECT_SND:
			{
				char str[30]={0};
#if BATTLE_TEST
				sprintf(str,"SFX/Test/%d.mp3",atoi(tFile->get(mCurrIndex+1,1)));
#else
				sprintf(str,"SFX/%d.ogg",atoi(tFile->get(mCurrIndex+1,1)));
#endif
				CCLOG("Load SFX %s",tFile->get(mCurrIndex+1,1));
				SimpleAudioEngine::sharedEngine()->preloadEffect(tFile->get(mCurrIndex+1,1));
			}break;
		}
		++mCurrIndex;
		float tPercent = mCurrIndex*100.0f/(tFile->getRowNum() + 20);			//20是跑空帧
		mProgress->setValue(tPercent);		//进度条
		m_pZombieSprite->setPositionX(mProgress->getPositionX()+mProgress->getContentSize().width*tPercent/100-5);		//僵尸位置
		if (tPercent >= 100)
			loadResouceEnd();
	}

	void LoadScene::loadAsyncImg(CCObject* pSender)
	{
		CCTexture2D *texture = (CCTexture2D*)pSender;
		mAsynLoadNum++;
	}

	void LoadScene::loadResouceEnd()
	{
		CSVFile* tFine = (CSVFile*)FileUtils::sharedFileUtils()->getFile(CSV_ROOT("preloadRes.csv"));
		if (mAsynLoadNum < mResourceVec.size() && mCurrIndex < (tFine->getRowNum() + 600))		//当预加载10秒后还没跳转界面则执行强制跳转处理
			return ;
		if (mCurrIndex >= (tFine->getRowNum() + 600))
			CCLOG("[ *ERROR ] LoadScene::loadResouceEnd");
		for (int i=0; i<mResourceVec.size();++i)
		{
			LoadResourceInfo &res = mResourceVec.at(i);
			if (res.Loadtype>sLoadType::eEffect)
				continue;
			CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(res.FilePath.c_str());//将plist文件加载进入缓存
			CCLOG("ProgressEnd addSpriteFramesWithFile mode1 %s ",res.FileName.c_str());
			if(res.Loadtype == sLoadType::eFrameAnimation)
				AnimationManager::sharedAction()->ParseAnimation(res.FileName.c_str(),eFrameRole);
			else if(res.Loadtype == sLoadType::eEffect)
				AnimationManager::sharedAction()->ParseAnimation(res.FileName.c_str());
		}
		this->unscheduleAllSelectors();
		CSceneManager::sharedSceneManager()->replaceScene(GETSCENE(CMainScene), 1.0f);
	}
}