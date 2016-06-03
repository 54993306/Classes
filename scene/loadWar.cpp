#include "scene/loadWar.h"
#include "SimpleAudioEngine.h"
#include "Global.h"
#include "scene/AnimationManager.h"
#include "tools/ToolDefine.h"
#include "model/DataCenter.h"
#include "scene/WarScene.h"
#include "common/CommonFunction.h"
#include "warscene/EffectData.h"
#include "warscene/CombatGuideManage.h"
//#include "model/BattleData.h"
#include "Battle/RoleBaseData.h"
#include "model/TerrainManager.h"
#include "model/WarManager.h"
#include "model/MapManager.h"
#include "scene/layer/LayerManager.h"
#include "common/CGameSound.h"
#include "Battle/LoadSpineData.h"
#include "warscene/BattleTools.h"
using namespace CocosDenshion;

LoadWar::LoadWar():m_tip(nullptr),m_currNum(0),m_publicNum(0), m_totalNum(0)
,m_progress(nullptr),m_Release(false),m_SceneType(-1),m_loadResNum(0),m_Layer(nullptr)
{}

LoadWar::~LoadWar()
{
	CC_SAFE_RELEASE(m_LoadSpine);
	m_LoadSpine = nullptr;
}

//背景图片显示处理
void LoadWar::BackImage()
{
	CCSprite* backgroundImage = (CCSprite*)m_Layer->findWidgetById("bg");
	int newID = 0;			
	int stage = m_Manage->getStageID();
	if (!stage||stage==101)								//（关卡0和关卡101特殊处理）
	{
		newID = stage;
	}else{
		int oldID = m_Manage->getLoadImage();
		do{
			newID = CCRANDOM_0_1()*9;					//图片总数
			CCLOG("----------%d",newID);
			if (newID != oldID)
				break;
		} while (true);
	}

	m_Manage->setLoadImage(newID);
	char path [60] = {0};
	sprintf(path,"warScene/LoadImage/%d.png",newID);
	if (!backgroundImage->initWithFile(path))
	{
		CCLOG("[ *ERROR ] LoadWar::BackImage BMG initWithFile Fail");
		backgroundImage->initWithFile("warScene/LoadImage/0.jpg");			//容错性处理
	}
	if (CCDirector::sharedDirector()->getScheduler()->getTimeScale() != 1)
		CCDirector::sharedDirector()->getScheduler()->setTimeScale(1);
}

void LoadWar::onEnterTransitionDidFinish()
{
	PlayBackgroundMusic(BGM_LoadWar,true);
}

void LoadWar::onCreate()
{
	m_Layer = LoadComponent("loading.xaml");//接收来自父节点的消息
	m_Layer->setPosition(VCENTER);
	this->addChild(m_Layer);

	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("skill/9049.plist");
	AnimationManager::sharedAction()->ParseAnimation("9049");

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
	m_Manage = DataCenter::sharedData()->getWar();
	m_LoadSpine = LoadSpineData::create();
	m_LoadSpine->retain();
	srandNum();
	BackImage();
}

void LoadWar::setRelease(bool isrelease,int sceneType)
{
	m_Release = isrelease;
	m_SceneType = sceneType;
}

void LoadWar::onEnter()
{
	CScene::onEnter();
	CSVFile* file = (CSVFile*)FileUtils::sharedFileUtils()->loadCSVFile(CSV_ROOT("loadWar.csv"));
	m_publicNum = file->getRowNum() - 1;					//一个资源加载完成继续加载另外一个文件的资源，一个类加载多个文件资源
	if (m_publicNum<public_time)m_publicNum = public_time;
	m_totalNum = m_publicNum+total_time+50;
	DataParse();											//得到战斗需显示的资源数据
	updateTips(0);
	this->schedule(schedule_selector(LoadWar::ResourceDispose));
	this->schedule(schedule_selector(LoadWar::updateTips),2);
}
void LoadWar::onExit()
{
	CScene::onExit();
	FileUtils::sharedFileUtils()->releaseFile(CSV_ROOT("warTips.csv"));
	FileUtils::sharedFileUtils()->releaseFile(CSV_ROOT("loadWar.csv"));
	FileUtils::sharedFileUtils()->releaseFile(CSV_ROOT("preloadRes.csv"));
}
void LoadWar::updateTips(float fdetal)
{
	CSVFile* file = (CSVFile*)FileUtils::sharedFileUtils()->getFile(CSV_ROOT("warTips.csv"));//从缓存中获取配置文件
	if (!file)
		file = (CSVFile*)FileUtils::sharedFileUtils()->loadCSVFile(CSV_ROOT("warTips.csv"));//将文件加载到缓存中
	string tips = string(GETLANGSTR(1003)) + file->get(CCRANDOM_0_1()*(file->getRowNum()-1),0);
	m_tip->setString(tips.c_str());
	m_tip->setVisible(false);
}
//技能解析,通过技能得到加载数据
void LoadWar::SkillParse( RoleBaseData& role,vector<int>&VecEffect,vector<int>&VecBuff )
{
	vector<RoleSkill*> VSkill;
	VSkill.push_back(&role.skNormal);
	VSkill.push_back(&role.skSpecial);
	VSkill.push_back(&role.skActive);
	VSkill.push_back(&role.skCaptain);
	for (auto i:VSkill)
		for (auto j:i->EffectList)
			for (auto k:j)
			{
				VecEffect.push_back(k.effectId);		//for (auto l:k.buffList){}		
				BuffData* Buffdata = m_Manage->getBuffData();
				for (auto p:k.buffList)
				{
					BuffEffect* effect = Buffdata->getBuffEffect(p.getBuffType(),p.getIsDBuff());
					if (!effect)
						continue;
					VecBuff.push_back(effect->getEffect_up());
					VecBuff.push_back(effect->getEffect_down());
				}
			}
}
//数据解析
void LoadWar::DataParse()
{
	BattleServerData* data = m_Manage->getBattleData();
	vector<int> VecRole;
	vector<int> VecEffect;
	vector<int> VecBuff;
	vector<int> VecTerrain;
	for (auto i: data->HeroList)
	{
		if (m_Manage->isSpine(i.thumb))
		{
			m_LoadSpine->AddRoleSpineID(i.thumb);
		}else{
			VecRole.push_back(i.thumb);
		}
		SkillParse(i,VecEffect,VecBuff);	
	}
	m_LoadSpine->AddRoleSpineID(146);
	m_LoadSpine->AddRoleSpineID(9999);
	VecRole.push_back(516);
	for (auto i: data->MonsterList)
	{
		if (m_Manage->isSpine(i.thumb))
		{
			m_LoadSpine->AddRoleSpineID(i.thumb);
		}else{
			VecRole.push_back(i.thumb);
		}
		SkillParse(i,VecEffect,VecBuff);	
	}
	for (auto i:data->TrapList)
		VecTerrain.push_back(i.terrainId);
	VectorRemoveRepeat(VecRole);
	VectorRemoveRepeat(VecEffect);
	VectorRemoveRepeat(VecBuff);
	VectorRemoveRepeat(VecTerrain);
	m_WarResouse[ResourceType::Load_Role]		= VecRole;
	m_WarResouse[ResourceType::Load_Effect]		= VecEffect;
	m_WarResouse[ResourceType::Load_Buff]		= VecBuff;
	m_WarResouse[ResourceType::Load_Terrain]	= VecTerrain;
}
//公共资源、地图特效技能特效一起加载、骨骼动画和骨骼动画效果一起加载
void LoadWar::ResourceDispose(float delta)
{
	++m_currNum;
	CCObject* resouseID = nullptr;
	switch(m_currNum)
	{
	case LOAD_PUBLIC_EFFECT:
		{
			LoadPublic();
			LoadTerrain();
			LoadEffect();
		}break;
	case LOAD_ROLE:		
		{
			LoadCocosEffect();
			for (auto id: m_WarResouse.find(ResourceType::Load_Role)->second)
				CocosBoneThread(id);
			if (!m_Release)
				m_LoadSpine->LoadSpineAnimation();
		}break;
	case total_time:
		{
			if (m_Release||m_Manage->getStageID())
				break;		
			LoadBeingAnimation();
		}break;
	default:break;
	}
	float fPercent = float(m_currNum)/float(m_totalNum) * 100;
	fPercent = fPercent>100?100:fPercent;
	m_progress->setValue(fPercent);
	m_pZombieSprite->setPositionX(m_progress->getPositionX()+m_progress->getContentSize().width*fPercent/100-5);
	if(m_currNum>=m_totalNum)
		ProgressEnd();
}
//应该是抽象出一个类来对这个进行处理
void LoadWar::LoadCocosEffect()
{
	if (m_Release)
	{
		CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo("warScene/attack.ExportJson");
	}else{
		CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfoAsync("warScene/attack.ExportJson",this,schedule_selector(LoadWar::CocosBoneCallBack));//添加之前判断是否存在
		LoadResourceInfo res;
		res.Loadtype = LoadType::Load_CocosBone;
		m_resVec.push_back(res);
	}
}

//加载角色,根据角色和是否存在技能加载释放技能所需图片
void LoadWar::CocosBoneThread(int ModeID)
{
	char ExportJson_str[60] = {0};//"BoneAnimation/101.ExportJson"
	sprintf(ExportJson_str,"BoneAnimation/%d.ExportJson",ModeID);
	CCAnimationData *animationData = CCArmatureDataManager::sharedArmatureDataManager()->getAnimationData(ToString(ModeID));
	if (m_Release)
	{
		CCLOG("Release WarResourse SKELETON = %d",ModeID);
		if (outPutERRORMsg("LoadWar::LoadRole can not find animationData",animationData))
			return;
		CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo(ExportJson_str);
	}else{
		if (animationData)
		{
			CCLOG("[ *TIPS ] LoadWar::LoadRole The Role Have Again");
			return;
		}
		CCLOG("LoadWarResourse Load SKELETON = %d",ModeID);
		CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfoAsync(ExportJson_str,this,schedule_selector(LoadWar::CocosBoneCallBack));//添加之前判断是否存在
		LoadResourceInfo res;
		res.FileName = ToString(ModeID);
		res.FilePath = std::string(ExportJson_str);
		res.Loadtype = LoadType::Load_CocosBone;
		m_resVec.push_back(res);
	}
}
void LoadWar::CocosBoneCallBack( float dt )
{ 
	m_loadResNum++; 
	//CCLOG("LoadWar::CocosBoneCallBack___%d", m_loadResNum);
}

void LoadWar::TextureThread(const char* url, const char* model, LoadType type/*= LoadType::Load_Effect*/)
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
	CCTextureCache::sharedTextureCache()->addImageAsync(pngStr.c_str(),this,callfuncO_selector(LoadWar::TextureThreadCallBack));
	LoadResourceInfo res;
	res.FileName = model;
	res.FilePath = url;
	res.Loadtype = type;
	m_resVec.push_back(res);
}
void LoadWar::TextureThreadCallBack(CCObject* pSender) 
{
	m_loadResNum++; 
	//CCLOG("LoadWar::TextureThreadCallBack___%d", m_loadResNum);
}

//加载公共资源
void LoadWar::LoadPublic()
{
	CSVFile* file = (CSVFile*)FileUtils::sharedFileUtils()->getFile(CSV_ROOT("loadWar.csv"));			//加载战斗公共资源
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
					TextureThread(url, model, LoadType::Load_Effect);
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
void LoadWar::LoadEffect()
{
	char plist_str[60] = {0};
	EffectData* EfData = m_Manage->getEffData();
	vector<int> VecEffectID;
	for (auto id:m_WarResouse.find(ResourceType::Load_Effect)->second)
	{
		if (EfData->getEffectInfo(id))
		{
			CCArray*arr = EfData->getEffectInfo(id)->getEffIdList();//特效id数组
			CCObject*obj = nullptr; 
			CCARRAY_FOREACH(arr,obj)
			{
				CCInteger* efid = (CCInteger*)obj;
				VecEffectID.push_back(efid->getValue());
			}
		}else{ CCLOG("[ ERROE ]: LoadWar::loadWarResourse in load skil Effect id =%d",id); }
	}
	VectorRemoveRepeat(VecEffectID);
	for(auto i:VecEffectID)
	{
		sprintf(plist_str,"skill/%d.plist",i);
		if (m_Release)
		{
			CCLOG("Release WarResourse EFFectID = %d",i);
			AnimationManager::sharedAction()->ReleaseAnimation(ToString(i));
			CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile(plist_str);
		}else{
			TextureThread(plist_str,ToString(i));
		}
	}
	BuffData* Buffdata = m_Manage->getBuffData();
	for (auto ptr:m_WarResouse.find(ResourceType::Load_Buff)->second)
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
//地形和地形在人物身上特效
void LoadWar::LoadTerrain()
{
	return;
	terData* data = m_Manage->getTerData();
	char plist_str[60] = {0};
	for (auto id: m_WarResouse.find(ResourceType::Load_Terrain)->second)
	{
		sprintf(plist_str,"terrain/%d.plist",id);
		if (m_Release)
		{
			CCLOG("Release WarResourse Terrain = %d",id);
			AnimationManager::sharedAction()->ReleaseAnimation(ToString(id));
			CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile(plist_str);
		}else{
			TextureThread(plist_str,ToString(id));
		}
		int effectID = data->getEffect(id);												//加载地形的人物身上效果
		if (effectID)
		{
			sprintf(plist_str,"skill/%d.plist",effectID);
			if (m_Release)
			{
				AnimationManager::sharedAction()->ReleaseAnimation(ToString(effectID));
				CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile(plist_str);
				CCLOG("Release WarResourse TerrainEffect = %d",effectID);
			}else{
				TextureThread(plist_str,ToString(effectID));
			}
		}
	}
}

void LoadWar::LoadBeingAnimation()
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
	CSVFile* file = (CSVFile*)FileUtils::sharedFileUtils()->loadCSVFile(CSV_ROOT("preloadRes.csv"));
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
		default:
			break;
		}
	}
}

void LoadWar::ProgressEnd()
{
	if (m_Release)
	{
		CCLOG("m_Release true");
		if (!m_Manage->getStageID())	//统一释放了spine动画,是否可以统一的去释放骨骼动画资源
			CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo("warpublic/open.ExportJson");
		this->unscheduleAllSelectors();
		DataCenter::sharedData()->getCombatGuideMg()->clearGuideData(true);
		m_Manage->BattleDataClear();
		DataCenter::sharedData()->getWar()->setWorldBoss(false);
		DataCenter::sharedData()->getWar()->setWorldBossRank(0);
		DataCenter::sharedData()->getTer()->clear();
		DataCenter::sharedData()->getMap()->clearMap();
		CCTextureCache::sharedTextureCache()->removeUnusedTextures();				//清理掉所有的不使用的图片
		FileUtils::sharedFileUtils()->releaseFile(CSV_ROOT("loadWar.csv"));
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
				CCLOG("[ ERROR ]: LoadWar::loadWarResourse()  Skip Scene Fail ");	
			}break;
		}
	}else if ( m_loadResNum>=m_resVec.size())
	{
		if ( !m_LoadSpine->getLoadSucceed())
			return;
		for (int i=0; i<m_resVec.size();++i)
		{
			LoadResourceInfo &res = m_resVec.at(i);
			if (res.Loadtype>LoadType::Load_Effect)
				continue;
			CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(res.FilePath.c_str());//将plist文件加载进入缓存
			CCLOG("ProgressEnd addSpriteFramesWithFile mode1 %s ",res.FileName.c_str());
			if(res.Loadtype == LoadType::Load_FrameAnimation)
				AnimationManager::sharedAction()->ParseAnimation(res.FileName.c_str(),RoleType);
			else if(res.Loadtype == LoadType::Load_Effect)
				AnimationManager::sharedAction()->ParseAnimation(res.FileName.c_str());
		}
		this->unscheduleAllSelectors();
		CSceneManager::sharedSceneManager()->replaceScene(GETSCENE(WarScene), 0.5f);
	}
}

