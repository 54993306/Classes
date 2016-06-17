#include "MapEffect.h"
#include "Global.h"
#include "tools/FileUtils.h"
#include "Battle/AnimationManager.h"
#include "Battle/BattleLayer/BattleMapLayer.h"
#include "Battle/EffectObject.h"
namespace BattleSpace{
	MapEffectData::MapEffectData()
		:m_type(0),m_x(0),m_y(0),m_EffectID(0),m_AnchorPointx(0),m_AnchorPointy(0),m_scale(1){}

	MapEffectData::~MapEffectData()
	{
		if (m_type != FadeIn_FadeOut)	//释放动画资源处理
		{
			char path[60] = {0};
			sprintf(path,"map/mapEffect/AnimationEffect/%d.plist",m_EffectID);
			AnimationManager::sharedAction()->ReleaseAnimation(ToString(m_EffectID));
			CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile(path);
			CCLOG("[ *TIPS ] MapEffectData::~MapEffectData removeSpriteFramesFromFile %d",m_EffectID);
		}
		m_EffectID = 0;
		m_type = 0;
	}
	void MapEffectData::settype(int var)
	{
		if (var != FadeIn_FadeOut)//加载动画资源处理
		{
			char path[60] = {0};
			sprintf(path,"map/mapEffect/AnimationEffect/%d.plist",m_EffectID);
			CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(path);					//导入plist文件
			if (var == JPG_Effect)
				AnimationManager::sharedAction()->ParseAnimation(ToString(m_EffectID),eEffectJPG);
			else
				AnimationManager::sharedAction()->ParseAnimation(ToString(m_EffectID),eEffectPNG);
			CCLOG("[ *TIPS ] MapEffectData:: MapEffectData addSpriteFramesWithFile %d",m_EffectID);
		}
		m_type = var;
	}
	int MapEffectData::gettype() {return m_type;}
	MapBackgroundManage::MapBackgroundManage():m_Map(nullptr),m_fogLay(nullptr),m_fogLay1(nullptr)
		,m_cloudPos(0),m_cloudAPos(0)
	{}

	MapBackgroundManage::~MapBackgroundManage()
	{
		for (Map_MapEffect::iterator iter = m_mapeffect.begin();iter != m_mapeffect.end();++iter)
		{
			vector< MapEffectData* > Vec = iter->second;
			for (vector< MapEffectData* >::iterator _iter = Vec.begin();_iter!= Vec.end();_iter++)
			{
				CC_SAFE_RELEASE(*_iter);
			}
			Vec.clear();
		}
		m_mapeffect.clear();
		for (vector<MapBackgroundData*>::iterator iter = m_VecBackground.begin();iter != m_VecBackground.end();iter++)
		{
			CC_SAFE_RELEASE(*iter);
		}
		m_VecBackground.clear();
	}

	void MapBackgroundManage::setMap(BattleMapLayer* map) 
	{ 
		if (map)
		{
			m_Map = map; 
			CCFadeIn* fin = CCFadeIn::create(0.5f);
			CCFadeOut* fout = CCFadeOut::create(0.5f);
			CCSequence* seq = CCSequence::create(fout,fin,NULL);
			CCRepeatForever* rep = CCRepeatForever::create(seq);
		}else{
			CCLOG("[ *ERROR ] MapBackgroundManage::setMap");
			return;
		}
	}

	bool MapBackgroundManage::init()
	{
		//if ( !EffectInfoinit() || !BackgroundInfoinit())
		//{
		//	CCLOG("[ *ERROR ] MapBackgroundManage::init");
		//	return false;
		//}
		return true;
	}

	bool MapBackgroundManage::EffectInfoinit()
	{
		unsigned long size = 0;
		unsigned char *buff = CCFileUtils::sharedFileUtils()->getFileData("map/mapEffect.json","r",&size);
		string data ((const char*)buff,size);
		rapidjson::Document doc;
		doc.Parse<0>(data.c_str());
		if ( doc.HasParseError())
		{
			CCLOG("[ *ERROR ] MapEffect::EffectInfoinit %s",doc.GetParseError());
			return false;
		}
		if ( !doc.IsObject() || !doc.HasMember("mapEffect") )
		{
			if (!doc.IsObject())
			{
				CCLOG("[ *ERROR ] MapEffect::EffectInfoinit Create Json Project Fail");
				return false;
			}
			CCLOG("[ *ERROR ] MapEffect::EffectInfoinit Can't Find Member map_effect");
			return false;
		}
		rapidjson::Value& Vec = doc["mapEffect"];		//得到键对应的数组信息
		if (!Vec.IsArray())								//判断是否为数组
		{
			CCLOG("[ *ERROR ]  MapEffect::EffectInfoinit Can Not't Find map_effect Vec");
			return false;
		}
		for (rapidjson::SizeType i = 0;i<Vec.Size();i++)
		{
			rapidjson::Value& json_step = Vec[i];							//得到数组里单步引导数据
			if (json_step.HasMember("mapID"))
			{
				rapidjson::Value& Vec_Effect = json_step["effect"];
				vector< MapEffectData* > Vec_EffectObje;
				if (Vec_Effect.IsArray())									//判断是否为数组
				{
					for (rapidjson::SizeType i = 0;i<Vec_Effect.Size();i++)
					{
						rapidjson::Value& Effect_ones = Vec_Effect[i];		//得到一个特效信息
						MapEffectData* efInfo = MapEffectData::create();
						if (Effect_ones.HasMember("EffectID"))
						{
							rapidjson::Value & var = Effect_ones["EffectID"];
							efInfo->setEffectID(var.GetInt());
						}else{
							CCLOG("[ *ERROR ] MapEffect::EffectInfoinit Lost Effect EffectID");
							continue;
						}
						if (Effect_ones.HasMember("type"))
						{
							rapidjson::Value & var = Effect_ones["type"];
							efInfo->settype(var.GetInt());
						}else{
							CCLOG("[ *ERROR ] MapEffect::EffectInfoinit Lost Effect type");
							continue;
						}
						if (Effect_ones.HasMember("scale"))
						{
							rapidjson::Value & var = Effect_ones["scale"];
							efInfo->setScale(var.GetInt());
						}
						if (Effect_ones.HasMember("point_x"))
						{
							rapidjson::Value & var = Effect_ones["point_x"];
							efInfo->setPointX(var.GetInt());
						}
						if (Effect_ones.HasMember("point_y"))
						{
							rapidjson::Value & var = Effect_ones["point_y"];
							efInfo->setPointY(var.GetInt());
						}
						if (Effect_ones.HasMember("AnchorPoint_x"))
						{
							rapidjson::Value & var = Effect_ones["AnchorPoint_x"];
							efInfo->setAnchorPointX(var.GetDouble());
						}
						if (Effect_ones.HasMember("AnchorPoint_y"))
						{
							rapidjson::Value & var = Effect_ones["AnchorPoint_y"];
							efInfo->setAnchorPointY(var.GetDouble());
						}
						efInfo->retain();
						AddEffectByVec(Vec_EffectObje,efInfo);
					}
					rapidjson::Value & MapFile = json_step["mapID"];
					AddEffectVecByMap(Vec_EffectObje,MapFile.GetInt());
				}else{
					CCLOG("[ *ERROR ] MapEffect::EffectInfoinit Can Not't Find effect Vec");
					return false;
				}
			}else{
				CCLOG("[ *ERROR ] MapEffect::EffectInfoinit Fail Index = %d",i);
				continue;
			}
		}
		return true;
	}

	vector<MapEffectData*>* MapBackgroundManage::getMapEffect(int mapID)
	{
		Map_MapEffect::iterator iter = m_mapeffect.find(mapID);
		if ( iter != m_mapeffect.end() )
			return &iter->second;
		return nullptr;
	}

	void MapBackgroundManage::AddEffectByVec(vector< MapEffectData* >& Vec,MapEffectData* data)
	{
		for (vector< MapEffectData* >::iterator iter = Vec.begin();iter != Vec.end();iter++)
		{
			if (*iter == data)	
			{
				CCLOG("[ *ERROR ] MapBackgroundManage::AddEffectByVec EffectID=%d",data->getEffectID());
				return;
			}
		}
		Vec.push_back(data);
	}

	void MapBackgroundManage::AddEffectVecByMap(vector< MapEffectData* >& Vec,int mapID)
	{
		if (!Vec.size())
		{
			CCLOG("[ *ERROR ] MapBackgroundManage::AddEffectVec MapID Repead MapID = %d",mapID);
			return;
		}
		Map_MapEffect::iterator iter = m_mapeffect.find(mapID);
		if ( iter != m_mapeffect.end() )
		{
			CCLOG("[ *ERROR ] MapBackgroundManage::AddEffectVec MapID Repead MapID = %d",mapID);
			return;
		}
		m_mapeffect[mapID] = Vec;
	}

	bool MapBackgroundManage::BackgroundInfoinit()
	{
		unsigned long size = 0;
		unsigned char *buff = CCFileUtils::sharedFileUtils()->getFileData("map/mapBackground.json","r",&size);
		string data ((const char*)buff,size);
		rapidjson::Document doc;
		doc.Parse<0>(data.c_str());
		if ( doc.HasParseError())
		{
			CCLOG("[ *ERROR ] MapEffect::initBackgroundInfo %s",doc.GetParseError());
			return false;
		}
		if ( !doc.IsObject() || !doc.HasMember("background") )
		{
			CCLOG("[ *ERROR ] MapEffect::initBackgroundInfo Create Json Project Fail");
			return false;
		}
		rapidjson::Value& Vec = doc["background"];		//得到键对应的数组信息
		if (!Vec.IsArray())								//判断是否为数组
		{
			CCLOG("[ *ERROR ]  MapEffect::initBackgroundInfo Can Not't Find map_effect Vec");
			return false;
		}
		for (rapidjson::SizeType i = 0;i<Vec.Size();i++)
		{
			rapidjson::Value& json_step = Vec[i];
			MapBackgroundData* data = MapBackgroundData::create();
			if (json_step.HasMember("stage"))
			{
				rapidjson::Value & var = json_step["stage"];
				data->setStageID(var.GetInt());
			}else{
				CCLOG("[ *ERROR ] MapEffect::BackgroundInfoinit Lost stage");
				continue;
			}
			if (json_step.HasMember("batch"))
			{
				rapidjson::Value & var = json_step["batch"];
				data->setBatch(var.GetInt());
			}else{
				CCLOG("[ *ERROR ] MapEffect::BackgroundInfoinit Lost batch");
				continue;
			}
			if (json_step.HasMember("imageID"))
			{
				rapidjson::Value & var = json_step["imageID"];
				data->setMapID(var.GetInt());
			}else{
				CCLOG("[ *ERROR ] MapEffect::BackgroundInfoinit Lost imageID");
				continue;
			}
			if (json_step.HasMember("turn"))
			{
				rapidjson::Value & var = json_step["turn"];
				data->setTurn(var.GetBool());
			}
			data->retain();
			AddBackgroundData(data);
		}
		return true;
	}

	void MapBackgroundManage::AddBackgroundData(MapBackgroundData*data)
	{
		for (vector<MapBackgroundData*>::iterator iter = m_VecBackground.begin(); iter != m_VecBackground.end();iter++)
		{
			if (*iter == data)
			{
				CCLOG("[ *ERROR ]  MapBackgroundManage::AddBackgroundData MapID=%d",data->getMapID());
				return;
			}
		}
		m_VecBackground.push_back(data);
	}
	//现在一个关卡只有一个背景图
	void MapBackgroundManage::initWithStageBatch(int stage,int batch)
	{
		bool CreateFail = true;
		for (vector<MapBackgroundData*>::iterator iter = m_VecBackground.begin();iter != m_VecBackground.end();iter++)
		{
			MapBackgroundData* data = *iter;
			if (stage == data->getStageID() && batch == data->getBatch())
			{
				CCNode* mapLayer = createMapByMapID(data->getMapID());
				if (data->getTurn())
					mapLayer->setScaleX(-1);
				mapLayer->setTag(map_Layer);
				m_Map->removeChildByTag(map_Layer);
				m_Map->addChild(mapLayer,-2);
				CreateFail = false;
				break;									//可以存在多个背景图的情况分段显示
			}
		}
		if (CreateFail&&!m_Map->getChildByTag(map_Layer))						//创建失败且当前没有地图背景的情况
		{
			CCNode* mapLayer = createMapByMapID(999);							//容错性处理
			mapLayer->setTag(map_Layer);
			m_Map->addChild(mapLayer,-2);
			CCLOG("[ *ERROR ] MapBackgroundManage::initWithStageBatch Stage = %d,Batch=%d",stage,batch);
		}
	}

	CCNode* MapBackgroundManage::createMapByMapID(int mapID)
	{
		CCNode* mapLayer = CCNode::create();
		char bmg_path [60] = {0};
		sprintf(bmg_path,"map/%d.jpg",mapID);
		CCSprite* bmg_sp = CCSprite::create(bmg_path);
		bmg_sp->setAnchorPoint(CCPointZero);//Sprite加在node上，要想原点重合就需要设置锚点为原点,精灵默认锚点是0.5，0.5
		if (!bmg_sp)
		{
			CCLOG("[ *ERROR ]MapBackgroundManage::createMapByMapID Bmg Is NULL !");
			return mapLayer;
		}
		mapLayer->addChild(bmg_sp);
		return mapLayer;
		vector<MapEffectData*>* Vec_EF = getMapEffect(mapID);
		if (!Vec_EF)
		{
			CCLOG("[ *TIPS ] MapBackgroundManage::createMapByMapID Can't Finde MapID=%d MapEffect",mapID);
			return mapLayer;
		}	
		for (int i=0 ;i < Vec_EF->size() ; i++)
		{
			MapEffectData* ef_data = Vec_EF->at(i);
			char pathp[60] = {0};
			switch (ef_data->gettype())
			{
			case FadeIn_FadeOut:
				{
					CCFadeIn* fin = CCFadeIn::create(0.5f);
					CCFadeOut* fout = CCFadeOut::create(0.5f);
					CCSequence* seq = CCSequence::create(fout,fin,NULL);
					CCRepeatForever* rep = CCRepeatForever::create(seq);
					sprintf(pathp,"map/mapEffect/%d.jpg",ef_data->getEffectID());
					CCSprite* sp_ef = CCSprite::create(pathp);
					if (sp_ef)
					{
						sp_ef->runAction((CCRepeatForever*)rep->copy());
						sp_ef->setAnchorPoint(ccp(ef_data->getAnchorPointX(),ef_data->getAnchorPointY()));
						sp_ef->setPosition(ccp(ef_data->getPointX(),ef_data->getPointY()));
						mapLayer->addChild(sp_ef);
						break;
					}
					CCLOG("[ *ERROR ] WarMapLayer::initMapBg Sprite init fail %d",ef_data->getEffectID());
				}
			case JPG_Effect:
			case PNG_Effect:
				{
					EffectObject* Animation_effect = EffectObject::create(ToString(ef_data->getEffectID()),PLAYERTYPE::Repeat);
					Animation_effect->setEffAnchorPoint(ef_data->getAnchorPointX(),ef_data->getAnchorPointY());
					Animation_effect->setPosition(ccp(ef_data->getPointX(),ef_data->getPointY()));
					Animation_effect->setScale(ef_data->getScale());
					mapLayer->addChild(Animation_effect);
				}
				break;
			default:
				break;
			}
		}
		return mapLayer;
	}

	void MapBackgroundManage::initWithStage( int stage )
	{
		char str1[60] = {0};
		sprintf(str1,"mapImage/%d.xaml",stage);
		string str = CCFileUtils::sharedFileUtils()->fullPathForFilename(str1);
		if (!CCFileUtils::sharedFileUtils()->isFileExist(str))
		{
			CCLOG("[ *ERROR ] MapBackgroundManage::initWithStage stage=%d",stage);
			str = CCFileUtils::sharedFileUtils()->fullPathForFilename("mapImage/0.xaml");
		}		
		CLayout* mapLayer = LoadComponent(str.c_str());
		if (!mapLayer)return;
		mapLayer->setAnchorPoint(CCPointZero);
		m_fogLay  = (CLayout *)mapLayer->findWidgetById("fog2");
		m_fogLay1 = (CLayout *)mapLayer->findWidgetById("fog3");
		m_fogLay2 = (CLayout *)mapLayer->findWidgetById("fog2_Copy");
		m_fogLay3 = (CLayout *)mapLayer->findWidgetById("fog3_Copy");
		runCloudAction();
		m_Map->setMaplayer(mapLayer);
	}
	//云运动
	void MapBackgroundManage::runCloudAction()
	{
		float width = 1308*2;
		CCMoveBy* move = CCMoveBy::create(60,ccp(-width,0));
		CCMoveBy* moveb = CCMoveBy::create(60,ccp(-width,0));
		CCSequence *sequence = CCSequence::create(move,CCCallFuncN::create(this,callfuncN_selector(MapBackgroundManage::relocation)),moveb,nullptr);
		m_fogLay->runAction(CCRepeatForever::create((CCSequence*)sequence->copy()));
		m_fogLay2->runAction(CCRepeatForever::create((CCSequence*)sequence->copy()));
		if (!m_cloudPos)
			m_cloudPos = m_fogLay->getPosition().x;
		if (!m_cloudAPos)
			m_cloudAPos = m_fogLay1->getPosition().x;
		CCMoveBy* move1 = CCMoveBy::create(120.0f,ccp(-width*2,0));
		CCSequence *sequence1 = CCSequence::createWithTwoActions(move1,CCCallFuncN::create(this,callfuncN_selector(MapBackgroundManage::relocation)));
		m_fogLay1->runAction(CCRepeatForever::create((CCSequence*)sequence1->copy()));
		m_fogLay3->runAction(CCRepeatForever::create((CCSequence*)sequence1->copy()));
	}

	void MapBackgroundManage::relocation(CCNode* pSender)
	{
		CLayout *cloud = (CLayout*)pSender;
		float width = 1308*2;
		float pos; 
		if (strcmp(cloud->getId(),"fog2")==0 || strcmp(cloud->getId(),"fog2_Copy")==0)
		{
			pos = m_cloudPos + width;
			cloud->setPositionX(pos);
		}else{
			cloud->setPositionX(m_cloudAPos);
		}
	}
}