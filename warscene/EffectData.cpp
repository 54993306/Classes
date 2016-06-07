#include "EffectData.h"
#include "tools/ToolDefine.h"
#include "Global.h"
namespace BattleSpace{
	EffectInfo::EffectInfo()
		:m_EFid(0)			//ID
		,m_ActionID(0)		//人物动作ID
		,usEft(0)			//施放特效
		,foeEft(0)			//受击特效
		,FloorEf(0)		    //地板特效id
		,isblack(0)			//是否需要黑屏
		,isShake(0)			//是否震屏
		,usMusicId(0)		//施放音效
		,foeMusicId(0)		//受击音效
	{}

	CCArray* EffectInfo::getEffIdList()
	{
		CCArray*arr= CCArray::create();
		if (usEft)arr->addObject(CCInteger::create(usEft));
		if (foeEft)arr->addObject(CCInteger::create(foeEft));
		if (FloorEf)arr->addObject(CCInteger::create(FloorEf));
		if (arr->count())return arr	;
		return nullptr;
	}

	EffectData::~EffectData()
	{
		for (WarEffect::iterator iter = warEffect.begin();iter != warEffect.end();++iter)
		{
			CC_SAFE_RELEASE(iter->second);
		}
		warEffect.clear();
	}

	bool EffectData::init()
	{
		CSVFile* file = (CSVFile*)FileUtils::sharedFileUtils()->loadCSVFile(CSV_ROOT("Effect.csv"));
		if (! file)
		{
			CCLOG("ERROR : Effect File is NULL Function in EffectData::init()");
			return false;
		}
		int row = file->getRowNum();
		for (int i = 1;i<row;++i)
		{
			int id = atoi(file->get(i,0));
			WarEffect::iterator iter = warEffect.find(id);
			if (iter != warEffect.end())
			{
				CCLOG("[*ERROR]:Effect ID Repeat in [EffectData::init()] id=%d,row=%d",id,row);
				CC_SAFE_RELEASE(iter->second);
				warEffect.erase(iter);
			}
			EffectInfo* efInfo = EffectInfo::create();
			efInfo->setEFid(id);
			efInfo->setActionID(atoi(file->get(i,1)));
			efInfo->setusEft(atoi(file->get(i,2)));
			efInfo->setusMusicId(atoi(file->get(i,3)));
			efInfo->setfoeEft(atoi(file->get(i,4)));
			efInfo->setfoeMusicId(atoi(file->get(i,5)));
			efInfo->setFloorEf(atoi(file->get(i,6)));
			efInfo->setisblack(atoi(file->get(i,7)));
			efInfo->setisShake(atoi(file->get(i,8)));
			efInfo->retain();
			warEffect[efInfo->getEFid()] = efInfo;
		}
		FileUtils::sharedFileUtils()->releaseFile(CSV_ROOT("Effect.csv"));
		return true;
	}

	EffectInfo* EffectData::getEffectInfo(int EffectID)
	{
		WarEffect::iterator iter = warEffect.find(EffectID);
		if (iter != warEffect.end()) return iter->second;
		CCLOG("[ ERROR ]: EffectData::getEffectInfo Lack id : %d",EffectID);
		return nullptr;
	}

	CCArray* EffectData::getEffs()
	{
		CCArray* arr = CCArray::create();
		for (WarEffect::iterator iter = warEffect.begin();iter != warEffect.end();++iter)
			arr->addObject(iter->second);
		return arr;
	}

	SpecialEffectInfo::SpecialEffectInfo():m_EffID(0),p_x(0),p_y(0),Acp_x(0),Acp_y(0),scale(0){}

	SpecialEffectData::~SpecialEffectData()
	{
		for (SpEff::iterator iter = spEff.begin();iter != spEff.end();++iter)
		{
			CC_SAFE_RELEASE(iter->second);
		}
		spEff.clear();
	}

	bool SpecialEffectData::init()
	{
		CSVFile* file = (CSVFile*)FileUtils::sharedFileUtils()->loadCSVFile(CSV_ROOT("specialEffect.csv"));
		if (!file)
		{
			CCLOG("ERROR : Effect File is NULL Function in SpecialEffectData::init()");
			return false;
		}
		int row = file->getRowNum();
		for (int i= 1;i<row;++i)
		{
			int id = atoi(file->get(i,0));
			SpEff::iterator iter = spEff.find(id);
			if (iter!= spEff.end())
			{
				CCLOG("[*ERROR]:Effect ID Repeat in [SpecialEffectData::init()]id=%d,row=%d",id,row);
				CC_SAFE_RELEASE(iter->second);
				spEff.erase(iter);
			}
			SpecialEffectInfo* spEf = SpecialEffectInfo::create();
			spEf->setEffID(id);
			spEf->setp_x(atof(file->get(i,1)));
			spEf->setp_y(atof(file->get(i,2)));
			spEf->setAcp_x(atof(file->get(i,3)));
			spEf->setAcp_y(atof(file->get(i,4)));
			spEf->setscale(atof(file->get(i,5)));
			spEf->setspeed(atof(file->get(i,6)));
			spEf->retain();
			spEff[spEf->getEffID()] = spEf;
		}
		FileUtils::sharedFileUtils()->releaseFile(CSV_ROOT("specialEffect.csv"));
		return true;
	}

	SpecialEffectInfo* SpecialEffectData::getspEff(int spId)
	{
		if (!spId)
			return NULL;
		SpEff::iterator iter = spEff.find(spId);
		if (iter != spEff.end())return iter->second;
		CCLOG("[ LOST ] : SpecialEffectData::getspEff spid =[ %d ]",spId);
		return nullptr;
	}

	CCArray* SpecialEffectData::getspEffs()
	{
		CCArray* arr = CCArray::create();
		for (SpEff::iterator iter = spEff.begin();iter != spEff.end();++iter)
			arr->addObject(iter->second);
		return arr;
	}

	int SpecialEffectData::getSpcialEfectId(int spId)
	{
		SpEff::iterator iter = spEff.find(spId);
		if (iter != spEff.end())
		{
			SpecialEffectInfo*spif = iter->second;
			return spif->getEffID();
		} 
		CCLOG("[ LOST ] : SpecialEffectData::getSpcialEfectId spid = %d",spId);
		return 0;
	}

	bool SpecialEffectData::JudgeSpcialEffect(int id)
	{
		SpEff::iterator iter = spEff.find(id);
		if (iter != spEff.end())
			return true;
		return false;
	}

	BuffEffect::BuffEffect():m_BuffType(0),small_icon(0),big_icon(0),effect_up(0)
		,effect_down(0),up_Loop(false),down_Loop(false),m_Dbuf(false)
	{}

	BuffData::~BuffData() { ClearData(); }

	bool BuffData::init()
	{
		unsigned long size = 0;
		unsigned char *buff = CCFileUtils::sharedFileUtils()->getFileData(CSV_ROOT("buff.json"),"r",&size);
		string data ((const char*)buff,size);
		rapidjson::Document doc;
		doc.Parse<0>(data.c_str());
		if ( doc.HasParseError())
		{
			CCLOG("[ *ERROR ] BufExp::init LoadJsonFile %s",doc.GetParseError());
			return false;
		}
		if ( !doc.IsObject() || !doc.HasMember("BUFF_EFFECT") )
		{
			CCLOG("[ *ERROR ] BufExp::init LoadJsonFile Create Json Project Fail");
			return false;
		}
		rapidjson::Value& VecEffect = doc["BUFF_EFFECT"];		//得到键对应的数组信息
		if (!VecEffect.IsArray())							//判断是否为数组
		{
			CCLOG("[ *ERROR ] BufExp::init LoadJsonFile Can Not't Find Vec");
			return false;
		}
		ClearData();
		BuffEffMap MapFalse;
		BuffEffMap MapTrue;
		m_JsonEffectMap[false] = MapFalse;
		m_JsonEffectMap[true] = MapTrue;
		for (rapidjson::SizeType i = 0;i<VecEffect.Size();i++)
		{
			rapidjson::Value& json_Info = VecEffect[i];					//得到数组里单步引导数据
			BuffEffect* buff_eff = BuffEffect::create();
			if (json_Info.HasMember("bufftype"))
			{
				rapidjson::Value & var = json_Info["bufftype"];
				buff_eff->setBuffType(var.GetInt());
			}else{
				CCLOG("[ *TIPS ] BufExp::init Lost buffid");
				continue;
			}
			if (json_Info.HasMember("Dbuf"))
			{
				rapidjson::Value & var = json_Info["Dbuf"];
				buff_eff->setDbuf(var.GetBool());
			}
			if (json_Info.HasMember("small_icon"))
			{
				rapidjson::Value & var = json_Info["small_icon"];
				buff_eff->setSmall(var.GetInt());
			}
			if (json_Info.HasMember("big_icon"))
			{
				rapidjson::Value & var = json_Info["big_icon"];
				buff_eff->setBig(var.GetInt());
			}
			if (json_Info.HasMember("effect_up"))						//特效的位置和缩放问题统一进行处理
			{
				rapidjson::Value & var = json_Info["effect_up"];
				buff_eff->setEffect_up(var.GetInt());
				if (json_Info.HasMember("up_Loop"))
				{
					rapidjson::Value & var = json_Info["up_Loop"];
					buff_eff->setup_Loop(var.GetBool());
				}
			}
			if (json_Info.HasMember("effect_down"))						//特效的位置和缩放问题统一进行处理
			{
				rapidjson::Value & var = json_Info["effect_down"];
				buff_eff->setEffect_down(var.GetInt());
				if (json_Info.HasMember("down_Loop"))
				{
					rapidjson::Value & var = json_Info["down_Loop"];
					buff_eff->setdown_Loop(var.GetBool());
				}
			}
			AddBuffEffect(buff_eff);
		}
		return true;
	}

	void BuffData::ClearData()
	{
		for (auto j : m_JsonEffectMap)
		{
			for (auto i : j.second)
			{
				CC_SAFE_RELEASE(i.second);
				i.second = nullptr;
			}
			j.second.clear();
		}
		m_JsonEffectMap.clear();
	}

	void BuffData::AddBuffEffect(BuffEffect* info)
	{
		JsonEffectMap::iterator iter = m_JsonEffectMap.find(info->getDbuf());
		if (iter != m_JsonEffectMap.end())
		{
			BuffEffMap::iterator piter = iter->second.find(info->getBuffType());
			if (piter != iter->second.end())
			{
				CCLOG("[ ERROR ] BufExp::AddBuffEffect BuffEffect Repeat type = %d",info->getBuffType());
				CC_SAFE_RELEASE(piter->second);
				iter->second.erase(piter);
			}
			info->retain();
			iter->second[info->getBuffType()] = info;
		}else{
			CCLOG("[ *ERROR ] BuffData::AddBuffEffect");
		}
	}

	BuffEffect* BuffData::getBuffEffect( int bufftype,bool Dbuf )
	{
		JsonEffectMap::iterator iter = m_JsonEffectMap.find(Dbuf);
		if (iter != m_JsonEffectMap.end())
		{
			BuffEffMap::iterator piter = iter->second.find(bufftype);
			if (piter != iter->second.end())
				return piter->second;
			CCLOG("[ TIPS ]BufExp::getBuffEffect Lost BUFF EFFECT type=%d",bufftype);
			return nullptr;
		}else{
			CCLOG("[ *ERROR ] BuffData::getBuffEffect");
			return nullptr;
		}
	}
	//测试数据
	//{
	//	"bufftype":2,
	//		"Dbuf":false,
	//		"effect_up":321,
	//		"up_Loop":false,
	//		"effect_down":325,
	//		"down_Loop":true
	//},
	//{
	//	"bufftype":3,
	//		"Dbuf":false,
	//		"effect_up":303,
	//		"up_Loop":true,
	//		"effect_down":209,
	//		"down_Loop":false
	//}

	StoryStep::StoryStep()
		:m_storyId(0),m_Texture(0),m_BGM(0),m_SFX(0),m_RoleID(0),m_Spine(true)
		,m_RoleShark(false),m_Rolex(0),m_Roley(0),m_right(true),m_Turn(false),m_RoleName("")
		,m_Aside(false),m_time(0),m_Content(""),m_Cx(0),m_Cy(0),m_ShowTalkBg(true),m_Shark(false)
	{}

	StoryData::StoryData()
	{

	}

	StoryData::~StoryData()
	{
		for (auto i : m_MapStoryData)
		{
			for (auto j : i.second)
			{
				CC_SAFE_RELEASE(j);
				j = nullptr;
			}
			i.second.clear();
		}
		m_MapStoryData.clear();
		m_SpineData.clear();
		m_MapConfig.clear();
	}

	bool StoryData::init()
	{
		unsigned long size = 0;
		unsigned char *buff = CCFileUtils::sharedFileUtils()->getFileData("csv/story/StoryConfig.json","r",&size);
		string data ((const char*)buff,size);
		rapidjson::Document doc;
		doc.Parse<0>(data.c_str());
		if ( doc.HasParseError())
		{
			CCLOG("[ *ERROR ] StroyData::init %s",doc.GetParseError());
			return true;
		}
		if ( !doc.IsObject() || !doc.HasMember("StoryConfig") )
		{
			CCLOG("[ *ERROR ] StroyData::init Create Json Project Fail");
			return true;
		}
		rapidjson::Value& Vec = doc["StoryConfig"];		//得到键对应的数组信息
		if (!Vec.IsArray())							//判断是否为数组
		{
			CCLOG("[ *ERROR ] StroyData::init Can Not't Find Vec");
			return true;
		}
		m_MapConfig.clear();
		for (rapidjson::SizeType i = 0;i<Vec.Size();i++)
		{
			rapidjson::Value& json_step = Vec[i];		//得到数组里单步引导数据
			StoryStep* story = StoryStep::create();
			if (json_step.HasMember("stage"))
			{
				rapidjson::Value & var = json_step["stage"];
				if (json_step.HasMember("files"))
				{
					rapidjson::Value & vecFile = json_step["files"];
					if (!vecFile.IsArray())
					{
						CCLOG("[ *TIPS ] StoryLayer::init Lost stage id 3");
						continue;
					}
					vector<TypeAndStoryID> VecInfo;
					for (rapidjson::SizeType j=0;j<vecFile.Size();j++)
					{
						int first = 0;
						int second = 0;
						rapidjson::Value& File_step = vecFile[j];
						if (File_step.HasMember("storyfile"))
						{
							rapidjson::Value & file = File_step["storyfile"];
							first = file.GetInt();
						}else{
							CCLOG("[ *TIPS ] StoryLayer::init Lost stage id 4");
							continue;
						}
						if (File_step.HasMember("storytype"))
						{
							rapidjson::Value & type = File_step["storytype"];
							second = type.GetInt();
						}else{
							CCLOG("[ *TIPS ] StoryLayer::init Lost stage id 3");
							continue;
						}
						VecInfo.push_back(make_pair(first,second));
					}
					m_MapConfig[var.GetInt()] = VecInfo;
				}else{
					CCLOG("[ *TIPS ] StoryLayer::init Lost stage id 2");
					continue;
				}
			}else{
				CCLOG("[ *TIPS ] StoryLayer::init Lost stage id  1");
				continue;
			}
		}
		return true;
	}
	//用判定文件的方式来判断是否出剧情？应该再添加一些数据信息来做,这样更安全避免对文件进行更多的判定和操作
	void StoryData::initStoryData( int stageid )
	{
		map<int,vector<TypeAndStoryID>>::iterator iter = m_MapConfig.find(stageid);
		if (iter != m_MapConfig.end())
		{
			for (auto i: iter->second)
			{
				char str[60] = {0};
				sprintf(str,"csv/story/%d.json",i.first);
				LoadFile(str,i.second);
			}
		}else{
			CCLOG("[ TIPS] StroyData::initStoryData (the %d stage have not storyFile)",stageid);
		}
	}

	void StoryData::LoadFile( const char* FileName ,int type )
	{
		unsigned long size = 0;
		unsigned char *buff = CCFileUtils::sharedFileUtils()->getFileData(FileName,"r",&size);
		string data ((const char*)buff,size);
		rapidjson::Document doc;
		doc.Parse<0>(data.c_str());
		if ( doc.HasParseError())
		{
			CCLOG("[ *ERROR ] StroyData::LoadFile %s",doc.GetParseError());
			return;
		}
		if ( !doc.IsObject() || !doc.HasMember("story") )
		{
			CCLOG("[ *ERROR ] StroyData::LoadFile Create Json Project Fail");
			return ;
		}
		rapidjson::Value& Vec = doc["story"];		//得到键对应的数组信息
		if (!Vec.IsArray())							//判断是否为数组
		{
			CCLOG("[ *ERROR ] StroyData::LoadFile Can Not't Find Vec");
			return ;
		}
		vector<StoryStep*> VecStep;
		for (rapidjson::SizeType i = 0;i<Vec.Size();i++)
		{
			rapidjson::Value& json_step = Vec[i];		//得到数组里单步引导数据
			StoryStep* story = StoryStep::create();
			if (json_step.HasMember("ID"))
			{
				rapidjson::Value & var = json_step["ID"];
				story->setstoryId(var.GetInt());
			}else{
				CCLOG("[ *TIPS ] StoryLayer::LoadFile Lost stpe id");
				continue;
			}
			if (json_step.HasMember("Texture"))
			{
				rapidjson::Value & var = json_step["Texture"];
				story->setTexture(var.GetInt());
			}
			if (json_step.HasMember("spine"))
			{
				rapidjson::Value & var = json_step["spine"];
				story->setSpine(var.GetBool());
			}
			if (json_step.HasMember("BGM"))
			{
				rapidjson::Value & var = json_step["BGM"];
				story->setBGM(var.GetInt());
			}
			if (json_step.HasMember("SFX"))
			{
				rapidjson::Value & var = json_step["SFX"];
				story->setSFX(var.GetInt());
			}
			if (json_step.HasMember("Head_Image"))
			{
				rapidjson::Value & var = json_step["Head_Image"];
				story->setRoleID(var.GetInt());
			}
			if (json_step.HasMember("Action"))
			{
				rapidjson::Value & var = json_step["Action"];
				story->setAction(var.GetString());
			}
			if (json_step.HasMember("Head_Shark"))
			{
				rapidjson::Value & var = json_step["Head_Shark"];
				story->setRoleShark(var.GetBool());
			}
			if (json_step.HasMember("Head_x"))
			{
				rapidjson::Value & var = json_step["Head_x"];
				story->setRolex(var.GetInt());
			}
			if (json_step.HasMember("Head_y"))
			{
				rapidjson::Value & var = json_step["Head_y"];
				story->setRoley(var.GetInt());
			}
			if (json_step.HasMember("D_right"))
			{
				rapidjson::Value & var = json_step["D_right"];
				story->setright(var.GetBool());
			}
			if (json_step.HasMember("TurnImage"))
			{
				rapidjson::Value & var = json_step["TurnImage"];
				story->setTurn(var.GetBool());
			}
			if (json_step.HasMember("Name"))
			{
				rapidjson::Value & var = json_step["Name"];
				story->setRoleName(var.GetString());
			}
			if (json_step.HasMember("Content"))
			{
				rapidjson::Value & var = json_step["Content"];
				story->setConten(var.GetString());
			}
			if (json_step.HasMember("Content_x"))
			{
				rapidjson::Value & var = json_step["Content_x"];
				story->setCx(var.GetInt());
			}
			if (json_step.HasMember("Content_y"))
			{
				rapidjson::Value & var = json_step["Content_y"];
				story->setCy(var.GetInt());
			}
			if (json_step.HasMember("ShowTalkBackImage"))
			{
				rapidjson::Value & var = json_step["ShowTalkBackImage"];
				story->setShowTalkBg(var.GetBool());
			}
			if (json_step.HasMember("Black"))
			{
				rapidjson::Value & var = json_step["Black"];
				story->setBlack(var.GetBool());
			}
			if (json_step.HasMember("Aside"))
			{
				rapidjson::Value & var = json_step["Aside"];
				story->setAside(var.GetBool());
			}
			if (json_step.HasMember("AsideTime"))
			{
				rapidjson::Value & var = json_step["AsideTime"];
				story->settime(var.GetDouble());
			}
			if (json_step.HasMember("AllShark"))
			{
				rapidjson::Value & var = json_step["AllShark"];
				story->setShark(var.GetBool());
			}
			story->retain();
			VecStep.push_back(story);
		}
		if (VecStep.size())
		{
			map<StoryType,vector<StoryStep*>>::iterator iter = m_MapStoryData.find((StoryType)type);
			if (iter != m_MapStoryData.end())
				CCLOG("[*ERROR] StroyData::LoadFile Line576");
			m_MapStoryData[ (StoryType)type ] = VecStep;
		}else{
			CCLOG("[ *ERROR ] StroyData::LoadFile Line578");
		}
	}
	//按设计要求，该方法应该由warManage来调用。
	vector<StoryStep*>* StoryData::getStory( StoryType storytype )
	{
		map<StoryType,vector<StoryStep*>>::iterator iter = m_MapStoryData.find(storytype);
		if (iter != m_MapStoryData.end())
			return &iter->second;
		return nullptr;
	}

	map<StoryType,vector<StoryStep*>>* StoryData::getStoryMap()
	{
		return &m_MapStoryData;
	}
}
