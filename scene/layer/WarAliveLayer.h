﻿

#ifndef _WAR_ALIVE_LAYER_H_
#define _WAR_ALIVE_LAYER_H_

/************************************************************* 
 *
 *
 *		Data : 2016.5.23
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 人物挂载层
 *
 *
 *************************************************************/
#include "AppUI.h"
#include "scene/layer/LayerManager.h"
namespace BattleSpace{
	class ActObject;
	class AliveObject;
	class BaseRole;
	class EffectObject;
	class WarManager;
	class WarMapData;
	class MoveObject;

	enum CreateType
	{
		DefaultCreat = 0,
		SceneTrap,
		FlyType_Tag, 
		AliveType_All = 100,
		AliveType_Hero,
		AliveType_Monster,
	};

	class WarAliveLayer:public BaseLayer
	{
	public:
		CREATE_FUNC(WarAliveLayer);
		WarAliveLayer();
		virtual ~WarAliveLayer();
		virtual void onEnter();
		virtual void onExit();
		virtual bool init();
		virtual void update(float delta); 
	public:
		void addEvent();
		void removeEvent();
		void removeMessage();
		virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
		virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
		virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
		virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent); 
	public:
		bool WorldBossJudge();			//需要重构的方法,应该抽象为关卡对武将移位的影响策略
		int getTouchGrid(CCTouch* pTouch);
		void initActobject(BaseRole* alive,int createType = 0);
		CCArray* getAlivesOb(int AliveType = AliveType_All);
		void AddActToGrid(ActObject* alive,int grid);
		void MoveAliveToGrid();
		void AliveObEffect(ActObject* alive,int createType = DefaultCreat);
		float AliceMoveExcute(BaseRole* alive,int Action = 2/*Walk_Index*/);
		void LayerShake(CCObject* ob);
		void aliveEntranceBattle(CCObject* ob);
		void initTouchAlive(BaseRole* alive);
		BaseRole* getAliveByMoveGrid(int grid);
		CC_SYNTHESIZE(CCLayerColor*,m_LayerColor,LayerColor);
		CC_SYNTHESIZE(CCNode*,m_AliveNode,AliveNode);			//挂载武将节点
		CC_SYNTHESIZE(CCNode*,m_MoveNode,MoveNode);				//挂载移动节点
		/*************************************************************************/
		bool unCommonAlive();
		void clearAlivesPauseMark();
		void heroWinAction();
		void createBatchMonster(int batchNumber);
		bool guideJudge(bool nextStep = false);
		void monsterSoleSprite(ActObject* aliveOb);
		void createMoveTarget();
		void createLayerColor();
		void createActObjects();
		void initActObjectOffs(ActObject* aliveOb,int grid);
		void initActObjectPosition(ActObject* aliveOb,int grid);
		void initMoveActObject(ActObject* aliveOb);
		void lucencyActObject(bool lucency);												//对其他武将执行透明操作
		bool touchInAlive(int grid , CCPoint& p);
		bool aliveMoveJudge();
		vector<int> getDestinations(BaseRole* pAlive,int pGrid);
		vector<BaseRole*> getAliveInArea(vector<int>& pAreas);
		bool absentInMoveArea();
		bool borderJudge(BaseRole* pAlive,vector<int>& pVector);
		void moveSwappingAlives(vector<BaseRole*>& pVector,int pOffs);
		bool callAliveJudge(vector<int>& pDestinations);
		bool swappingRule(vector<int>& pDestinations);
		bool vectorIntersection(vector<int>& pVector,vector<int>& ptVector);
	protected:
		BaseRole*		m_TouchAlive;		//被触摸的武将id
		ActObject*		m_MoveActObject;		//拖拽移动对象
		WarManager*		m_Manage;
		WarMapData*		m_map;
		CCPoint			m_TouchOffs;		//触摸偏移
		int				m_grid;				//记录移动对象的格子位置
		bool			m_TouchAliveBtn;	//是否是按钮传递的触摸
	};
};
#endif // !_WAR_ALIVE_LAYER_H_