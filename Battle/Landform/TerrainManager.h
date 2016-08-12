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
	/**************** �������� *****************/
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

	/**************** �����˺� *****************/
	class terRound :public CCObject
	{
	public:
		terRound();
		virtual ~terRound();
		CREATE_FUNC(terRound);
		inline bool init(){return true;}
	public:
		int m_TerID;									//����id
		int m_AliveID;									//Ŀ��id
		int m_effect;									//�����˺���Ч
		int m_music;									//Ч����Ч
		map<unsigned int,int> m_LostHp;					//��ʧѪ��
		map<unsigned int,int> m_CurrHp;					//�˺���Ѫ��
		map<unsigned int,unsigned int> m_CurrMaxHp;		//���Ѫ��
	private:

	};

	/*************** �������� **********************/
	class Terrain : public CCObject
	{
	public:
		Terrain();
		virtual ~Terrain(){};
		CREATE_FUNC(Terrain);
	public:
		CC_SYNTHESIZE(unsigned int,m_terrainID,TerrainID);
		CC_SYNTHESIZE(int,m_Type,Type);					//����
		CC_SYNTHESIZE(int,m_ProType,ProType);			//����Ӱ������
		CC_SYNTHESIZE(int,m_ProNum,ProNum);				//����Ӱ����ֵ
		CC_SYNTHESIZE(int,m_GridIndex,GridIndex);		//λ��
		CC_SYNTHESIZE(int,m_HurtNum,TerrainHurt);		//�˺�ֵ		
		CC_SYNTHESIZE(int,m_TriggerNum,TriggerNum);		//����������
		CC_SYNTHESIZE(int,m_ExistNum,ExistNum);			//���ڻغ�����
		CC_SYNTHESIZE(int,m_TerEffect,TerEffect);		//������Чid
		CC_SYNTHESIZE(int,m_AliveEffect,AliveEffect);	//�����˺���Ч
		CC_SYNTHESIZE(int,m_AliveMusic,AliveMusic);		//�����˺���Ч
		//RoleBuffData m_Buff;
		inline void setName(const char* name){m_name = name;}
		inline string getName(){return m_name;}
	protected:
		inline bool init(){return true;}
	private:		
		string m_name;				//��Ч����
	};

	/************* ���ι���	**************************/
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
		void AttributeDispose(BaseRole* alive,Terrain* ter = nullptr);	//������ζ��佫���Ե�Ӱ��
		void clear();
		void initData(vector<BattleTrap>& VecTerrain);
		terRound* TerrainAI(BaseRole* alive);
		void reduceRoundNum();									//�������е��γ����غ���
	protected:
		bool init(){return true;};
		Terrainobj m_terrainobj;											//ս���ϵĵ���
	};
};
#endif // !_TEERAIN_MANAGER_H_
