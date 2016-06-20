
#include "AppUI.h"
#include "Global.h"
#include "DataCenter.h"
#include "TerrainManager.h"
#include "Global.h"
#include "model/BuffManage.h"
#include "tools/ToolDefine.h"
#include "model/WarManager.h"
#include "model/MapManager.h"
#include "common/CommonFunction.h"
#include "Battle/BaseRole.h"
#include "Battle/BaseRoleData.h"
namespace BattleSpace{
	using namespace cocos2d;

terData::terData(){}

terData::~terData()
{
	m_Effects.clear();
	m_Musics.clear();
}

bool terData::init()
{
	CSVFile* file = (CSVFile*)FileUtils::sharedFileUtils()->loadCSVFile(CSV_ROOT("terrain.csv"));
	if (! file)
	{
		CCLOG("[ *ERROR ]: terData::init");
		return false;
	}
	int row = file->getRowNum();
	for (int i = 1;i<row;++i)
	{
		int id = atoi(file->get(i,0));
		int effectID	= atoi(file->get(i,1));
		int musicID		= atoi(file->get(i,2));
		m_Effects[id]	= effectID;
		m_Musics[id]	= musicID;
	}
	FileUtils::sharedFileUtils()->releaseFile(CSV_ROOT("terrain.csv"));
	return true;
}

int terData::getEffect(int id)
{
	map<unsigned int,int>::iterator iter = m_Effects.find(id);
	if (iter != m_Effects.end())
		return iter->second;
	return 0;
}

int terData::getMusic(int id)
{
	map<unsigned int,int>::iterator iter = m_Musics.find(id);
	if (iter != m_Musics.end())
		return iter->second;
	return 0;
}

terRound::terRound()
	:m_TerID(-1)
	,m_AliveID(-1)
	,m_effect(-1)
	,m_music(-1)
{}

terRound::~terRound()
{
	m_LostHp.clear();
	m_CurrHp.clear();
	m_CurrMaxHp.clear();
}

Terrain::Terrain()
	:m_terrainID(-1)			//地形id
	,m_Type(-1)		//地形类型是否可以站立
	,m_GridIndex(-1)
	,m_TriggerNum(1)
	,m_HurtNum(-1)
	,m_TerEffect(-1)
	,m_name("")
	,m_ExistNum(0)
	,m_ProType(0)
	,m_ProNum(0)
	,m_AliveMusic(0)
{}

TerrainManager::~TerrainManager()
{
	clear();
}

void TerrainManager::clear()
{
	for(Terrainobj::iterator iter = m_terrainobj.begin(); iter != m_terrainobj.end();++iter)
	{
		CC_SAFE_RELEASE(iter->second);
	}
	m_terrainobj.clear();
}

//把地形添加到容器中
void TerrainManager::addTerrain(Terrain* terrain)
{
	do 
	{
		if (!terrain)break;
		Terrain* pterrain = getTerrain(terrain->getTerrainID());
		if (pterrain == terrain)break;
		if (pterrain)
		{
			CCLOG("TerrainManager has exists TerrainID:%d",terrain->getTerrainID());
			break;
		}
		terrain->retain();
		m_terrainobj[terrain->getTerrainID()] = terrain;
	} while (0);
}
//把地形从容器中移除（与武将类似）
void TerrainManager::ClearNullityTerrain()
{
	CCArray* arr = getTerrains();
	CCObject* obj = nullptr;
	CCARRAY_FOREACH(arr , obj)
	{
		Terrain* ter = (Terrain*)obj;
		if (ter && (ter->getExistNum() <= 0 || ter->getTriggerNum() <=0)) 
			m_terrainobj.erase(ter->getTerrainID()); //map的移除操作
	}
}
//根据id移除地形
void TerrainManager::removeTerrainById(int terId)
{
	Terrain* ter = getTerrain(terId);
	if (ter)
	{
		if (!ter->getExistNum() || !ter->getTriggerNum())
			m_terrainobj.erase(ter->getTerrainID());
	}else{ outPutERRORMsg("TerrainManager::removeTerrainById", (void*)false); }
}

//根据id得到地形
Terrain* TerrainManager::getTerrain(unsigned int terrainID)
{
	Terrainobj::iterator iter = m_terrainobj.find(terrainID);
	if (iter != m_terrainobj.end())
		return iter->second;
	return nullptr;
}
//添加服务器传过来的地形
void TerrainManager::initData(vector<BattleTrap>& VecTerrain)
{
	clear();
	terData* data = nullptr/*DataCenter::sharedData()->getWar()->getTerData()*/;
	for (int i =0 ; i<VecTerrain.size() ; i++)
	{
		Terrain* terrain = Terrain::create();
		BattleTrap* cterrain = & VecTerrain.at(i);				//这个对象会一直存在，且只存在一个每一次新的获取就会重新赋值
		terrain->setTerrainID(i+1000);							//自定义的地形id在当前场景地形的唯一id
		terrain->setTerEffect(cterrain->terrainId);				//用ID来表示绘制地形的特效
		terrain->setGridIndex(cterrain->posX*4+cterrain->posY);
		terrain->setName(cterrain->name.c_str());
		terrain->setTerrainHurt(cterrain->damage);
		terrain->setType(cterrain->terrainType);
		terrain->setProType(cterrain->pro_type);
		terrain->setProNum(cterrain->pro_rate);
		terrain->setTriggerNum(cterrain->triggerNum);
		terrain->setExistNum(cterrain->existNum);
		terrain->setAliveEffect(data->getEffect(cterrain->terrainId));
		terrain->setAliveMusic(data->getMusic(cterrain->terrainId));
		//terrain->m_Buff = cterrain->buff;							//对象复制
		this->addTerrain(terrain);
	}
}
//得到所有的地形
CCArray* TerrainManager::getTerrains()
{
	do 
	{
		if (m_terrainobj.empty())break;
		CCArray* arr = CCArray::create();
		for (Terrainobj::iterator iter = m_terrainobj.begin() ; iter != m_terrainobj.end() ; ++iter)
			arr->addObject(iter->second);
		return arr;
	} while (0);
	return nullptr;
}

//得到当前位置的地形
Terrain* TerrainManager::getTerrainByGrid(char grid)
{
	for (Terrainobj::iterator iter = m_terrainobj.begin() ; iter != m_terrainobj.end(); ++iter)
	{
		if (iter->second->getGridIndex() == grid)
			return iter->second;
	}
	return nullptr;
}
//改变地形位置
bool TerrainManager::addAliveAtGrid(unsigned int terrainID,char grid)
{
	Terrain* ter = getTerrainByGrid(grid);
	if (ter && ter->getTerrainID() != terrainID)
		return false;
	ter = getTerrain(terrainID);
	if (ter)
	{
		ter->setGridIndex(grid);
		return true;
	}
	return false;
}

terRound* TerrainManager::TerrainAI(BaseRole* alive)
{
	if (alive)
	{
		AttributeDispose(alive);
		Terrain* ter = getTerrainByGrid(alive->getGridIndex());
		if (ter && ter->getTriggerNum()>0 && ter->getExistNum()>0)
		{
			ter->setTriggerNum(ter->getTriggerNum() - 1);						//触发次数减1
			terRound* round = terRound::create();
			round->m_TerID = ter->getTerrainID();
			round->m_AliveID = alive->getAliveID();
			if (abs(alive->getHp()) > abs(ter->getTerrainHurt()))
			{
				alive->setHp(alive->getHp() - ter->getTerrainHurt());			//地形搞不死人，最少剩下1点血量
			}else{
				alive->setHp(1);										
			}	
			unsigned int hitID = alive->getAliveID();
			round->m_LostHp[hitID] = -ter->getTerrainHurt();
			round->m_CurrMaxHp[hitID] = alive->getMaxHp();
			round->m_CurrHp[hitID] = alive->getHp();							//相对于传入伤害的血量 当前为10
			round->m_effect = ter->getAliveEffect();							//地形伤害效果
			round->m_music = ter->getAliveMusic();								//地形伤害音效
			AttributeDispose(alive,ter);										//地形影响属性处理
			int ranNum = CCRANDOM_0_1()*100;
			//if (ranNum < ter->m_Buff.getTriggerRate())										//添加地形buf
			//{
			//	BuffManage* bfmg = alive->getBuffManage();							
			//	//bfmg->AddBuff(ter->buff);
			//}else{	CCLOG("Terrain Buf Add Fail terId = %d,ranNum=%d,useRate=%d",ter->getTerrainID(),ranNum,ter->m_Buff.getTriggerRate());	}
			return round;
		}else{ return nullptr; }
	}else{ return nullptr; }
}
//减少地形持续回合数
void TerrainManager::reduceRoundNum()
{
	CCArray* arr = getTerrains();
	CCObject* obj = nullptr;
	CCARRAY_FOREACH(arr,obj)
	{
		Terrain* ter = (Terrain*)obj;
		ter->setExistNum(ter->getExistNum() -1);
		//ter->setTriggerNum(ter->getTriggerNum() - 1);
	}
	ClearNullityTerrain();	//清理无效地形
}

void TerrainManager::AttributeDispose(BaseRole* alive,Terrain* ter)
{
	if(outPutERRORMsg("TerrainManager::AttributeDispose",alive))return;
	bool dispose = true;		//默认为添加武将地形效果
	int atbType = 0;
	int atbTypeNum = 0;
	int usType = alive->getTerType();
	if (ter)
	{
		atbType = ter->getProType();
		atbTypeNum = ter->getProNum();
	}else{
		atbType = usType;
		atbTypeNum = alive->getTerTypeNum();
		dispose = false;
	}
	if(!atbType)return;

	switch ((sProperty)atbType)
	{
	case sProperty::eAttack:
		{
			if (dispose)
			{
				if (!usType)alive->setAtk(alive->getAtk() + atbTypeNum);
			}else{
				alive->setAtk(alive->getAtk() - atbTypeNum);
			}
		}
		break;
	case sProperty::eDefense:
		{
			if (dispose)
			{
				if (!usType)alive->setDef(alive->getDef() + atbTypeNum);
			}else{
				alive->setDef(alive->getDef() - atbTypeNum);
			}
		}
		break;
	case sProperty::eBlood:
		{
			if (dispose)
			{
				if (!usType)alive->setMaxHp(alive->getMaxHp() + atbTypeNum);
			}else{
				alive->setMaxHp(alive->getMaxHp() - atbTypeNum);
			}
		}
		break;
	case sProperty::eHit:
		{
			if (dispose)
			{
				if (!usType)alive->setHit(alive->getHit() + atbTypeNum);
			}else{
				alive->setHit(alive->getHit() - atbTypeNum);
			}
		}
		break;
	case sProperty::eCrit:
		{
			if (dispose)
			{
				if (!usType)alive->setCrit(alive->getCrit() + atbTypeNum);
			}else{
				alive->setCrit(alive->getCrit() - atbTypeNum);
			}
		}
		break;
	default:
		break;
	}
	if (dispose)
	{
		alive->setTerType(atbType);
		alive->setTerTypeNum(atbTypeNum);
	}else{
		alive->setTerType(0);
		alive->setTerTypeNum(0);
	}
}
}
