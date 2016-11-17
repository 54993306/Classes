#include "Battle/BattleScene/LoadBattleResource.h"
#include "SimpleAudioEngine.h"
#include "Battle/AnimationManager.h"
#include "tools/ToolDefine.h"
#include "Battle/BattleCenter.h"
#include "Battle/BattleScene/BattleScene.h"
#include "common/CommonFunction.h"
#include "Battle/EffectData.h"
#include "Battle/CombatGuideManage.h"
#include "Battle/BaseRoleData.h"
#include "Battle/Landform/TrapManage.h"
#include "Battle/WarManager.h"
#include "Battle/CoordsManage.h"
#include "scene/layer/LayerManager.h"
#include "common/CGameSound.h"
#include "Battle/SpineDataManage.h"
#include "Battle/BattleTools.h"
/******************************************/
#include "Battle/BaseRoleData.h"
#include "Battle/RoleSkill.h"
#include "Battle/skEffectData.h"
#include "Battle/BattleDataCenter.h"
#include "Battle/MonsterData.h"
#include "Battle/HeroData.h"
#include "Battle/Config/ConfigManage.h"
#include "Battle/RoleConfigData.h"
using namespace CocosDenshion;
//using namespace Battle;
namespace BattleSpace{
	LoadBattleResource::LoadBattleResource():m_tip(nullptr),m_CurrIndex(0),m_publicNum(0), m_totalNum(0)
		,m_progress(nullptr),m_Release(false),m_SceneType(-1),m_loadResNum(0),m_Layer(nullptr)
	{}

	//背景图片显示处理
	void LoadBattleResource::BackImage()
	{
		CCSprite* backgroundImage = (CCSprite*)m_Layer->findWidgetById("bg");
		int newID = 0;			
		int stage = BattleManage->getStageIndex();
		if (!stage||stage==101)								//（关卡0和关卡101特殊处理）
		{
			newID = stage;
		}else{
			int oldID = BattleManage->getLoadImage();
			do{
				newID = CCRANDOM_0_1()*9;					//图片总数
				CCLOG("----------%d",newID);
				if (newID != oldID)
					break;
			} while (true);
		}

		BattleManage->setLoadImage(newID);
		char path [60] = {0};
		sprintf(path,"warScene/LoadImage/%d.png",newID);
		if (!backgroundImage->initWithFile(path))
		{
			CCLOG("[ *ERROR ] LoadBattleResource::BackImage BMG initWithFile Fail");
			backgroundImage->initWithFile("warScene/LoadImage/0.jpg");			//容错性处理
		}
		if (CCDirector::sharedDirector()->getScheduler()->getTimeScale() != 1)
			CCDirector::sharedDirector()->getScheduler()->setTimeScale(1);
	}

	void LoadBattleResource::onEnterTransitionDidFinish()
	{
		PlayBackgroundMusic(BGM_LoadWar,true);
	}

	void LoadBattleResource::onCreate()
	{
		m_Layer = LoadComponent("loading.xaml");//接收来自父节点的消息
		m_Layer->setPosition(VCENTER);
		this->addChild(m_Layer);

		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("skill/9049.plist");
		AnimationManager::sharedAction()->ParseAnimation("9049");

		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("skill/104.plist");
		AnimationManager::sharedAction()->ParseAnimation("104");										//效果模型容错性处理
		//进度条
		m_progress = (CProgressBar*)m_Layer->findWidgetById("progress");
		m_progress->setMaxValue(100);

		//僵尸跳
		CCAnimation *pZombieEffect = AnimationManager::sharedAction()->getAnimation("9049");
		pZombieEffect->setDelayPerUnit(0.05f);					//设置帧率
		CCAnimate* pAnimate = CCAnimate::create(pZombieEffect);
		CCSprite* pZombieSprite = CCSprite::create();
		pZombieSprite->setScale(0.9f);
		pZombieSprite->setAnchorPoint(ccp(0.5f, 0.0f));
		pZombieSprite->setPositionY(m_progress->getPositionY()+5);
		m_Layer->addChild(pZombieSprite);
		pZombieSprite->runAction(CCRepeatForever::create(pAnimate));
		m_pZombieSprite = pZombieSprite;

		m_tip = CCLabelTTF::create(GETLANGSTR(1003),"font/arial.ttf",20);
		m_tip->setColor(ccc3(255,255,255));
		CCSize winSize = VirtualScreen::Instance()->VirtualSize();
		m_tip->setPositionY(70);
		m_tip->setPositionX(winSize.width/2 + OFFSETX - 200);
		m_tip->setHorizontalAlignment(kCCTextAlignmentLeft);				//设置对齐样式
		m_tip->setAnchorPoint(ccp(0,0.5));
		m_Layer->addChild(m_tip);
		srandNum();
		BackImage();
	}

	void LoadBattleResource::setRelease(bool isrelease,int sceneType)
	{
		m_Release = isrelease;
		m_SceneType = sceneType;
	}

	void LoadBattleResource::onEnter()
	{
		CScene::onEnter();
		CSVFile* file = (CSVFile*)FileUtils::sharedFileUtils()->loadCSVFile("csv/loadWar.csv");
		m_publicNum = file->getRowNum() - 1;					//一个资源加载完成继续加载另外一个文件的资源，一个类加载多个文件资源
		if (m_publicNum<public_time)m_publicNum = public_time;
		m_totalNum = m_publicNum+total_time+50;
		DataParse();											//得到战斗需显示的资源数据
		TrapParse();
		updateTips(0);
		this->schedule(schedule_selector(LoadBattleResource::ResourceDispose));
		this->schedule(schedule_selector(LoadBattleResource::updateTips),2);
	}
	void LoadBattleResource::onExit()
	{
		CScene::onExit();
		FileUtils::sharedFileUtils()->releaseFile("csv/warTips.csv");
		FileUtils::sharedFileUtils()->releaseFile("csv/loadWar.csv");
		FileUtils::sharedFileUtils()->releaseFile("csv/preloadRes.csv");
	}
	void LoadBattleResource::updateTips(float fdetal)
	{
		CSVFile* file = (CSVFile*)FileUtils::sharedFileUtils()->getFile("csv/warTips.csv");//从缓存中获取配置文件
		if (!file)
			file = (CSVFile*)FileUtils::sharedFileUtils()->loadCSVFile("csv/warTips.csv");//将文件加载到缓存中
		string tips = string(GETLANGSTR(1003)) + file->get(CCRANDOM_0_1()*(file->getRowNum()-1),0);
		m_tip->setString(tips.c_str());
		m_tip->setVisible(false);
	}
	//技能解析,通过技能得到加载数据
	void LoadBattleResource::SkillParse( const BaseRoleData* pRole)
	{
		vector< const RoleSkill*> VSkill;
		VSkill.push_back(pRole->getNormalSkill());
		VSkill.push_back(pRole->getSpecialSkill());
		VSkill.push_back(pRole->getActiveSkill());
		VSkill.push_back(pRole->getCaptainSkill());
		for ( const RoleSkill* tSkill:VSkill)
			for (vector<skEffectData*> tVecEffect:tSkill->getEffectVector())
				for (skEffectData* tEffectData:tVecEffect)
				{
					mVecEffect.push_back(tEffectData->getEffectID());		//for (auto l:k.buffList){}		需要引入头文件
					const BuffConfig* Buffdata = BattleConfig->getBuffData();
					for (auto p:tEffectData->getBuffVector())
					{
						const BuffEffect* effect = Buffdata->getBuffEffect(p->getBuffType(),p->getIsDBuff());
						mVecBuff.push_back(effect->getEffect_up());
						mVecBuff.push_back(effect->getEffect_down());
					}
				}
	}
	//数据解析
	void LoadBattleResource::DataParse()
	{
		const vector<BaseRoleData*> tRoleDatas = BattleData->getRoleDatas();
		for (auto tRoleData: tRoleDatas)
		{
			if (SpineManage->isSpineModel(tRoleData->getRoleModel()))
			{
				SpineManage->AddRoleSpineID(tRoleData->getRoleModel());
			}else{
				mVecRole.push_back(tRoleData->getRoleModel());
				const RoleConfigData* tData = BattleConfig->getConfigData(tRoleData->getRoleModel());				//加载变身模型
				if (tData->getVariant())
					SpineManage->AddRoleSpineID(tData->getVariantModel());
			}
			SkillParse(tRoleData);	
		}
		mVecRole.push_back(516);
		SpineManage->AddRoleSpineID(146);
		SpineManage->AddSpineEffectID(9999);
		SpineManage->AddSpineEffectID(20001);
		SpineManage->AddSpineEffectID(20002);
#if BATTLE_TEST
		//m_LoadSpine->AddRoleSpineID(2317);
#endif
	}

	void LoadBattleResource::TrapParse()
	{
		const vector<TrapData*>tTrapVec = BattleData->getTraps();
		if ( tTrapVec.empty() )
			return;
		SpineManage->AddTrapID(10000);
		vector<int> tLoads;
		for (auto tTrap : tTrapVec)
		{
			if (SpineManage->isSpineModel(tTrap->getTrapModel()))
				SpineManage->AddTrapID(tTrap->getTrapModel());
			const BuffConfig* Buffdata = BattleConfig->getBuffData();
			const BuffEffect* effect = Buffdata->getBuffEffect(tTrap->getTrapBuff()->getBuffType(),tTrap->getTrapBuff()->getIsDBuff());
			mVecBuff.push_back(effect->getEffect_up());
			mVecBuff.push_back(effect->getEffect_down());
		}
	}
	//公共资源、地图特效技能特效一起加载、骨骼动画和骨骼动画效果一起加载
	void LoadBattleResource::ResourceDispose(float delta)
	{
		switch(m_CurrIndex)
		{
		case LOAD_PUBLIC_EFFECT:
			{
				LoadPublic();
				LoadEffect();
			}break;
		case LOAD_ROLE:		
			{
				LoadCocosEffect();
				for (auto id: mVecRole)
					CocosBoneThread(id);
				if (!m_Release)
					SpineManage->LoadSpineAnimation();
			}break;
		case total_time:
			{
				if (m_Release||BattleManage->getStageIndex())
					break;		
				LoadBeingAnimation();
			}break;
		default:break;
		}
		++m_CurrIndex;
		updateProgress();
	}

	void LoadBattleResource::updateProgress()
	{
		float fPercent = float(m_CurrIndex)/float(m_totalNum) * 100;
		fPercent = fPercent>100?100:fPercent;
		m_progress->setValue(fPercent);
		m_pZombieSprite->setPositionX(m_progress->getPositionX()+m_progress->getContentSize().width*fPercent/100-5);
		if(m_CurrIndex < m_totalNum)
			return ;
		if (m_Release)
		{
			releaseResource();
		}else{
			ProgressEnd();
		}
	}
	//应该是抽象出一个类来对这个进行处理
	void LoadBattleResource::LoadCocosEffect()
	{
		if (m_Release)
		{
			CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo("warScene/attack.ExportJson");
		}else{
			CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfoAsync("warScene/attack.ExportJson",this,schedule_selector(LoadBattleResource::CocosBoneCallBack));//添加之前判断是否存在
			LoadResourceInfo res;
			res.Loadtype = sLoadType::eCocosBone;
			m_resVec.push_back(res);
		}
	}
	//加载角色,根据角色和是否存在技能加载释放技能所需图片
	void LoadBattleResource::CocosBoneThread(int ModeID)
	{
		char ExportJson_str[60] = {0};//"BoneAnimation/101.ExportJson"
		sprintf(ExportJson_str,"BoneAnimation/%d.ExportJson",ModeID);
		CCAnimationData *animationData = CCArmatureDataManager::sharedArmatureDataManager()->getAnimationData(ToString(ModeID));
		if (m_Release)
		{
			CCLOG("Release WarResourse SKELETON = %d",ModeID);
			if (outPutERRORMsg("LoadBattleResource::CocosBoneThread can not find animationData",animationData))
				return;
			CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo(ExportJson_str);
		}else{
			if (animationData)
			{
				CCLOG("[ *TIPS ] LoadBattleResource::CocosBoneThread The Role Have Again %d",ModeID);
				return;
			}
			CCLOG("CocosBoneThread Load SKELETON = %d",ModeID);
			CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfoAsync(ExportJson_str,this,schedule_selector(LoadBattleResource::CocosBoneCallBack));//添加之前判断是否存在
			LoadResourceInfo res;
			res.FileName = ToString(ModeID);
			res.FilePath = std::string(ExportJson_str);
			res.Loadtype = sLoadType::eCocosBone;
			m_resVec.push_back(res);
		}
	}
	void LoadBattleResource::CocosBoneCallBack( float dt )
	{ 
		m_loadResNum++; 
		//CCLOG("LoadBattleResource::CocosBoneCallBack___%d", m_loadResNum);
	}

	void LoadBattleResource::TextureThread(const char* url, const char* model, sLoadType type/*= LoadType::Load_Effect*/)
	{
		for (auto i:m_resVec)
		{
			if (strcmp(i.FilePath.c_str(),url)==0)
				return;
		}
		string str = url;
		int pos = str.find("plist");
		if (pos==string::npos)
			return;
		string pngStr = str.substr(0,pos);
		pngStr = pngStr.append("png");				//将plist的后缀换成png
		CCTextureCache::sharedTextureCache()->addImageAsync(pngStr.c_str(),this,callfuncO_selector(LoadBattleResource::TextureThreadCallBack));
		LoadResourceInfo res;
		res.FileName = model;
		res.FilePath = url;
		res.Loadtype = type;
		m_resVec.push_back(res);
	}
	void LoadBattleResource::TextureThreadCallBack(CCObject* pSender) 
	{
		m_loadResNum++; 
		//CCLOG("LoadBattleResource::TextureThreadCallBack___%d", m_loadResNum);
	}

	//加载公共资源
	void LoadBattleResource::LoadPublic()
	{
		CSVFile* file = (CSVFile*)FileUtils::sharedFileUtils()->getFile("csv/loadWar.csv");			//加载战斗公共资源
		char ExportJson_str[60] = {0};
		for (int i=1;i < file->getRowNum(); i++)
		{
			int resType = atoi(file->get(i,0));
			const char* url = file->get(i,1);
			switch(resType)
			{
			case LOAD_FrameAnimation:
				{
					const char* model = file->get(i,2);
					int type = atoi(file->get(i,3));
					if (m_Release)
					{
						CCLOG("Release  public mode %s,\n",model);
						AnimationManager::sharedAction()->ReleaseAnimation(model);
						CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile(url);		//根据路径下的文件释放资源
					}else{
						TextureThread(url, model, sLoadType::eEffect);
						CCLOG("Load Public Effect %s",model);
					}
				}break;
			case LOAD_BackMusic:
				{ 
					SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic(url); 
				}break;
			case LOAD_EffectMusic:
				{
					CCLOG("load effect %s,\n",url);
					SimpleAudioEngine::sharedEngine()->preloadEffect(url);
				}break;
			case LOAD_Font:{}break;
			case LOAD_CocosBone:
				{
					int skeletonID = atoi(file->get(i,1));
					sprintf(ExportJson_str,"BoneAnimation/%d.ExportJson",skeletonID);
					if (m_Release)
					{
						CCLOG("Release Public SKELETON = %d",skeletonID);
						CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo(ExportJson_str);
					}else{
						CCLOG("Load Public SKELETON = %d",skeletonID);
						CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(ExportJson_str);
					}
				}break;
			default:break;
			}
		}
	}
	//加载技能特效
	void LoadBattleResource::LoadEffect()
	{
		char plist_str[60] = {0};
		const EffectData* EfData = BattleConfig->getEffData();
		vector<int> VecEffectID;
		VectorUnique(mVecEffect);
		for(auto tID : mVecEffect)
			EfData->getEffectInfo(tID)->initEffectList(VecEffectID);
		VectorUnique(VecEffectID);
		for(auto tFileID:VecEffectID)
		{
			if (!tFileID) continue;
			if (!m_Release && SpineManage->isSpineModel(tFileID))
			{
				SpineManage->AddSpineEffectID(tFileID);
			}else{
				sprintf(plist_str,"skill/%d.plist",tFileID);
				if (m_Release)
				{
					CCLOG("Release WarResourse EFFectID = %d",tFileID);
					AnimationManager::sharedAction()->ReleaseAnimation(ToString(tFileID));
					CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile(plist_str);
				}else{
					TextureThread(plist_str,ToString(tFileID));
				}
			}
		}
		VectorUnique(mVecBuff);
		for (auto ptr:mVecBuff)
		{
			if ( !ptr )continue;
			sprintf(plist_str,"skill/%d.plist",ptr);
			if (m_Release)
			{
				CCLOG("Release WarResourse EFFectID = %d",ptr);
				AnimationManager::sharedAction()->ReleaseAnimation(ToString(ptr));
				CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile(plist_str);
			}else{
				TextureThread(plist_str,ToString(ptr));
			}
		}
	}

	void LoadBattleResource::LoadBeingAnimation()
	{
		unsigned long size = 0;
		string strPath = CCFileUtils::sharedFileUtils()->fullPathForFilename("csv/Animation/taiwen.json");
		unsigned char *buff = CCFileUtils::sharedFileUtils()->getFileData(strPath.c_str(),"r",&size);
		string data ((const char*)buff,size);
		rapidjson::Document doc;
		doc.Parse<0>(data.c_str());
		if ( doc.HasParseError()||!doc.IsObject() || !doc.HasMember("OPEN"))
		{
			CCLOG("[ *ERROR ] LoginScene::LoadAnimationCallBack %s",doc.GetParseError());
			return;
		}
		rapidjson::Value& VecPath = doc["OPEN"];		//得到键对应的数组信息
		if (!VecPath.IsArray())							//判断是否为数组
		{
			CCLOG("[ *ERROR ] BufExp::init LoadJsonFile Can Not't Find Vec");
			return;
		}
		for (rapidjson::SizeType i = 0;i<VecPath.Size();i++)
		{
			rapidjson::Value& json_Info = VecPath[i];					//得到数组里单步引导数据
			if (json_Info.HasMember("path"))
			{
				rapidjson::Value & path = json_Info["path"];
				CCTexture2D* tex = CCTextureCache::sharedTextureCache()->addImage(path.GetString());
				CCSpriteFrame* frem = CCSpriteFrame::createWithTexture(tex,CCRect(0,0,tex->getContentSize().width,tex->getContentSize().height));
				if (json_Info.HasMember("name"))
				{
					rapidjson::Value & name = json_Info["name"];
					CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(frem,name.GetString());
				}else{
					CCLOG("[ *ERROR ] LoginScene::onEnterTransitionDidFinish");
					continue;
				}
			}
			if (json_Info.HasMember("plist"))
			{
				rapidjson::Value & plist = json_Info["plist"];
				CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(plist.GetString());
			}
		}
		CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("warScene/open/taiwen.ExportJson");
		//在新手引导关卡才加载公共信息，否则在进入主城时加载
		CSVFile* file = (CSVFile*)FileUtils::sharedFileUtils()->loadCSVFile("csv/preloadRes.csv");
		int row = file->getRowNum();
		for(int i = 1; i < row;++i)
		{
			int resType = atoi(file->get(i,0));
			const char* url = file->get(i,1);
			switch(resType)
			{
			case LOAD_BACKGROUND_SND:
				{
					SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic(url);
				}break;
			case LOAD_EFFECT_SND:
				{
					SimpleAudioEngine::sharedEngine()->preloadEffect(url);
				}break;
			}
		}
	}

	void LoadBattleResource::releaseResource()
	{
		if ( !BattleManage->getStageIndex())	//统一释放了spine动画,是否可以统一的去释放骨骼动画资源
			CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo("warpublic/open.ExportJson");
		this->unscheduleAllSelectors();
		ManageCenter->clearBattleData();
		//ManageCenter->rleaseData();
		CCTextureCache::sharedTextureCache()->removeUnusedTextures();				//清理掉所有的不使用的图片
		FileUtils::sharedFileUtils()->releaseFile("csv/loadWar.csv");
		LayerManager::instance()->closeAll();
		switch (m_SceneType)
		{
		case SkipcityScene:
			{
				CSceneManager::sharedSceneManager()->replaceScene(GETSCENE(CMainScene), 1.0f);
			}break;
		case SkipLoginScene:
			{
				CSceneManager::sharedSceneManager()->replaceScene(GETSCENE(LoginScene), 0.5f);		
			}break;
		case skipSelectHero:
			{
				CSceneManager::sharedSceneManager()->replaceScene(GETSCENE(SelectHeroScene), 0.5f);	
			}break;
		default:	
			{
				CCLOG("[ ERROR ]: LoadBattleResource::loadWarResourse()  Skip Scene Fail ");	
			}break;
		}
	}

	void LoadBattleResource::ProgressEnd()
	{
		if ((m_loadResNum>=m_resVec.size() && SpineManage->getLoadSucceed()) || 
			(m_CurrIndex >= m_totalNum + 1000) )
		{
			if (m_CurrIndex >= m_totalNum + 1000)
				CCLOG("[ *ERROR ] LoadBattleResource::ProgressEnd");
			for (int i=0; i<m_resVec.size();++i)
			{
				LoadResourceInfo &res = m_resVec.at(i);
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
			CSceneManager::sharedSceneManager()->replaceScene(GETSCENE(BattleScene), 0.5f);
		}
	}

}
