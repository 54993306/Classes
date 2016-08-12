#ifndef _TEERAIN_MANAGER_H_
#define _TEERAIN_MANAGER_H_
#include "cocos2d.h"
#include "Battle/BattleTrap.h"
#include "Battle/BuffData.h"
using namespace cocos2d;
using namespace std;
namespace BattleSpace
{
	class BaseRole;
	//  [12/10/2014 xc_lin]
	/**************** 地形数据 *****************/
	class terData : public CCObject
	{
	public:
		terData();
		~terData();
		virtual bool init();
		CREATE_FUNC(terData);
		map<unsigned int,int> m_Effects;
		map<unsigned int,int> m_Musics;
		int getEffect(int id);
		int getMusic(int id);
	private:

	};

	/**************** 地形伤害 *****************/
	class terRound :public CCObject
	{
	public:
		terRound();
		virtual ~terRound();
		CREATE_FUNC(terRound);
		inline bool init(){return true;}
	public:
		int m_TerID;									//地形id
		int m_AliveID;									//目标id
		int m_effect;									//地形伤害特效
		int m_music;									//效果音效
		map<unsigned int,int> m_LostHp;					//丢失血量
		map<unsigned int,int> m_CurrHp;					//伤害后血量
		map<unsigned int,unsigned int> m_CurrMaxHp;		//最大血量
	private:

	};

	/*************** 地形数据 **********************/
	class Terrain : public CCObject
	{
	public:
		Terrain();
		virtual ~Terrain(){};
		CREATE_FUNC(Terrain);
	public:
		CC_SYNTHESIZE(unsigned int,m_terrainID,TerrainID);
		CC_SYNTHESIZE(int,m_Type,Type);					//类型
		CC_SYNTHESIZE(int,m_ProType,ProType);			//属性影响类型
		CC_SYNTHESIZE(int,m_ProNum,ProNum);				//属性影响数值
		CC_SYNTHESIZE(int,m_GridIndex,GridIndex);		//位置
		CC_SYNTHESIZE(int,m_HurtNum,TerrainHurt);		//伤害值		
		CC_SYNTHESIZE(int,m_TriggerNum,TriggerNum);		//触发次数。
		CC_SYNTHESIZE(int,m_ExistNum,ExistNum);			//存在回合数。
		CC_SYNTHESIZE(int,m_TerEffect,TerEffect);		//地形特效id
		CC_SYNTHESIZE(int,m_AliveEffect,AliveEffect);	//地形伤害特效
		CC_SYNTHESIZE(int,m_AliveMusic,AliveMusic);		//地形伤害音效
		//RoleBuffData m_Buff;
		inline void setName(const char* name){m_name = name;}
		inline string getName(){return m_name;}
	protected:
		inline bool init(){return true;}
	private:		
		string m_name;				//特效名称
	};

	/************* 地形管理	**************************/
	typedef map<unsigned int,Terrain*> Terrainobj;	

	class TerrainManager : public CCObject
	{
	public:
		TerrainManager(){};
		virtual ~TerrainManager();
		CREATE_FUNC(TerrainManager);	

		void addTerrain(Terrain* terrain);
		void ClearNullityTerrain();
		void removeTerrainById(int terId);
		Terrain* getTerrain(unsigned int terrainID);
		CCArray* getTerrains();
		Terrain* getTerrainByGrid(char grid);
		bool addAliveAtGrid(unsigned int terrainID,char grid);
		void AttributeDispose(BaseRole* alive,Terrain* ter = nullptr);	//处理地形对武将属性的影响
		void clear();
		void initData(vector<BattleTrap>& VecTerrain);
		terRound* TerrainAI(BaseRole* alive);
		void reduceRoundNum();									//减少所有地形持续回合数
	protected:
		bool init(){return true;};
		Terrainobj m_terrainobj;											//战场上的地形
	};
};
#endif // !_TEERAIN_MANAGER_H_
