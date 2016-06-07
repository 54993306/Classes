#ifndef _MAPEFFECT_
#define _MAPEFFECT_

#include "AppUI.h"
/************************************************************************/  
//					功能：管理地图的显示效果
//		一个关卡批次里可能存在多个地图远近近景,地图上部和地图下部(没做)
//		一个地图上可能有多个不同类型的特效
//		一场战斗可能切换几个地图
//		特效是跟地图的文件绑定的,根据地图文件就会获取相应的特效
//		根据关卡id和批次id获取地图文件的系统
//		地图文件应该是统一命名的,写一个配置文件配置关卡和批次对应地图
/************************************************************************/
namespace BattleSpace{
	enum MapEffectType
	{
		FadeIn_FadeOut=1,
		JPG_Effect,
		PNG_Effect,
		MapEffect_Tag = 50,
	};

	class MapBackgroundData : public CCObject
	{
	public:
		MapBackgroundData():m_StageID(0),m_Batch(0),m_MapID(0),m_Turn(false),m_BackgroundID(0){};
		virtual bool init(){return true;}
		CREATE_FUNC(MapBackgroundData);
		CC_SYNTHESIZE(int,m_BackgroundID,BackgroundID);
		CC_SYNTHESIZE(int,m_StageID,StageID);
		CC_SYNTHESIZE(int,m_Batch,Batch);
		CC_SYNTHESIZE(int,m_MapID,MapID);
		CC_SYNTHESIZE(bool,m_Turn,Turn);
	};

	class MapEffectData : public CCObject
	{
	public:
		MapEffectData();
		virtual ~MapEffectData();
		virtual bool init(){return true;}
		CREATE_FUNC(MapEffectData);							
		CC_SYNTHESIZE(int,m_EffectID,EffectID);				//特效文件名
		CC_PROPERTY(int,m_type,type);						//特效类型(渐变类、动画类...)
		CC_SYNTHESIZE(float,m_scale,Scale);
		CC_SYNTHESIZE(float,m_x,PointX);
		CC_SYNTHESIZE(float,m_y,PointY);
		CC_SYNTHESIZE(float,m_AnchorPointx,AnchorPointX);
		CC_SYNTHESIZE(float,m_AnchorPointy,AnchorPointY);
	};

	typedef map< int,vector< MapEffectData* > > Map_MapEffect;
	class WarMapLayer;
	class MapBackgroundManage : public CCObject
	{
	public:
		MapBackgroundManage();
		virtual ~MapBackgroundManage();
		virtual bool init();
		void AddEffectByVec(vector< MapEffectData* >& Vec,MapEffectData* data);
		void AddEffectVecByMap(vector< MapEffectData* >& Vec,int mapID);
		bool EffectInfoinit();
		void AddBackgroundData(MapBackgroundData*data);
		bool BackgroundInfoinit();
		CREATE_FUNC(MapBackgroundManage);
		void setMap(WarMapLayer* map);
		void initWithStageBatch(int stage,int batch);
		void initWithStage(int stage);
		vector<MapEffectData*>* getMapEffect(int mapID);
		CCNode* createMapByMapID(int mapID);
		void runCloudAction();
		void relocation(CCNode* pSender);
	private:
		Map_MapEffect m_mapeffect;
		vector<MapBackgroundData*>m_VecBackground;
		WarMapLayer* m_Map;
		float m_cloudPos;
		float m_cloudAPos;
		CLayout* m_fogLay;
		CLayout* m_fogLay1;
		CLayout* m_fogLay2;
		CLayout* m_fogLay3;
	};
};
#endif