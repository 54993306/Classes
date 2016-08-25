

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
namespace BattleSpace
{
	class RoleObject;
	class AliveObject;
	class BaseRole;
	class EffectObject;
	class WarManager;
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

	class BattleRoleLayer:public BaseLayer
	{
	public:
		CREATE_FUNC(BattleRoleLayer);
		BattleRoleLayer();
		virtual ~BattleRoleLayer();
		virtual void onEnter();
		virtual void onExit();
		virtual bool init();
		virtual void update(float delta); 
	public:
		void addEvent();
		void removeEvent();
		void removeMessage(CCObject* ob);
		virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
		virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
		virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
		virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent); 
	public:
		int getTouchGrid(CCTouch* pTouch);
		void initActobject(CCObject* ob);
		CCArray* getAlivesOb(int AliveType = AliveType_All);
		void AddActToGrid(RoleObject* alive,int grid);
		void LayerShake(CCObject* ob);
		void roleEntranceBattle(CCObject* ob);
		void initTouchAlive(BaseRole* alive);
		BaseRole* getAliveByMoveGrid(int grid);
		/*************************************************************************/
		void clearAlivesPauseMark();
		void heroWinAction(CCObject* ob);
		void createBatchMonster(CCObject* ob);
		void createMoveTarget();
		void createLayerColor();
		void createActObjects();
		void initMoveActObject(RoleObject* aliveOb);
		void lucencyActObject(bool lucency);												//对其他武将执行透明操作
		bool touchInAlive(int grid , const CCPoint& p);

		void changeLight(CCObject* ob);
		void SkillCreateRole(CCObject* ob);
		void changeTestState(CCObject* ob);
		void roleStand(CCObject* ob);
		CC_SYNTHESIZE_READONLY(bool,mtestState,testState);
	protected:
		CCLayerColor*	m_LayerColor;
		CCNode*			m_MoveNode;
		CCNode*			m_AliveNode;
		BaseRole*		m_TouchAlive;		//被触摸的武将id
		RoleObject*		m_MoveActObject;		//拖拽移动对象
		WarManager*		mManage;
		CCPoint			m_TouchOffs;		//触摸偏移
		int				m_grid;				//记录移动对象的格子位置
		bool			m_TouchAliveBtn;	//是否是按钮传递的触摸

	};
};
#endif // !_WAR_ALIVE_LAYER_H_