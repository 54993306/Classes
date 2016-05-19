
#ifndef _WAR_ALIVE_LAYER_H_
#define _WAR_ALIVE_LAYER_H_
#include "AppUI.h"
#include "scene/layer/LayerManager.h"
class ActObject;
class AliveObject;
class WarAlive;
class EffectObject;
class WarManager;
class WarMapData;
class MoveObj;
/****************************************************************************/
/*
	绘制战场上的武将，并实际进行武将移动的地方，武将层	//  [7/28/2014 xc_lin]
*/
/****************************************************************************/

enum CreateType
{
	DefaultCreat = 0,
	SceneTrap,
	FlyType_Tag, 
	AliveType_All = 100,
	AliveType_Hero,
	AliveType_Monster,
};
typedef pair<int ,MoveObj*> GridMoveObj;

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
	bool WorldBossJudge(WarAlive* alive,int grid);			//需要重构的方法,应该抽象为关卡对武将移位的影响策略
	bool BorderJudge(WarAlive* alive,vector<int>&Vec);
	void initActobject(WarAlive* alive,int createType = 0);
	CCArray* getAlivesOb(int AliveType = AliveType_All);
	void AddActToGrid(ActObject* alive,int grid);
	void MoveAliveToGrid(WarAlive* alive,int grid);
	void AliveObEffect(ActObject* alive,int createType = DefaultCreat);
	float AliceMoveExcute(WarAlive* alive,int Action = 2/*Walk_Index*/);
	void LayerShake(CCObject* ob);
	void aliveEntranceBattle(CCObject* ob);
	void TouchAlive(WarAlive* alive);
	MoveObj* getMoveByGrid(int grid);
	CC_SYNTHESIZE(CCLayerColor*,m_LayerColor,LayerColor);
	CC_SYNTHESIZE(CCNode*,m_AliveNode,AliveNode);			//挂载武将节点
	CC_SYNTHESIZE(CCNode*,m_MoveNode,MoveNode);				//挂载移动节点
	/*************************************************************************/
	void clearAlivesPauseMark();
	void heroWinAction();
	void createBatchMonster(int batchNumber);
	bool moveGuide(int grid,bool nextStep);
	void monsterSoleSprite(ActObject* aliveOb);
	void createMoveTarget();
	void createLayerColor();
	void createActObjects();
	void initActObjectOffs(ActObject* aliveOb,int grid);
	void initActObjectPosition(ActObject* aliveOb,int grid);
	void creatMoveObject(WarAlive* alive);
	void initMoveActObject(ActObject* aliveOb);
	void lucencyActObject(bool lucency);												//对其他武将执行透明操作
	bool touchInAlive(int grid , CCPoint& p);
	bool aliveMoveJudge(WarAlive*alive,int grid);
	void mapAliveMove(map<int,WarAlive*>& MoveMap);
	bool aliveBattleJudge(WarAlive* pAlive,vector<int>& pDestinationGrid );
	bool aliveCoverJudge(map<int,GridMoveObj>& pDestinationObject,vector<int>& pDestinationGrid);
	bool swappingJudge(map<int,GridMoveObj>& pDestinationObject,MoveObj* pAliveMoveObj);
	
protected:
	WarAlive*		m_TouchAlive;		//被触摸的武将id
	ActObject*		m_MoveActObject;		//拖拽移动对象
	WarManager*		m_Manage;
	WarMapData*		m_map;
	CCPoint			m_TouchOffs;		//触摸偏移
	int				m_grid;				//记录移动对象的格子位置
	bool			m_TouchAliveBtn;	//是否是按钮传递的触摸
};
#endif // !_WAR_ALIVE_LAYER_H_