#ifndef _WAR_MAP_LAYER_H_
#define _WAR_MAP_LAYER_H_
#include "AppUI.h"

/************************************************************************/
/* 
*        col-> 
*      +-------+-------+-------+-------+-------+-------+-------+ 
*  row |00     |01     |02     |03     |04     |05     |...     
*   |  |       |       |       |       |       |       |        
*   v  |       |       |       |       |       |       |       
*      +-------+-------+-------+-------+-------+-------+-------+ 
*      |10     |11     |12     |13     |14     |15     |...     
*      |       |       |       |       |       |       |      
*      |       |       |       |       |       |       |       
*      +-------+-------+-------+-------+-------+-------+-------+ 
*      |20     |21     |22     |23     |24     |25     |...    
*      |       |(xp,yp)|       |       |       |       |        height = 92
*      |       |       |       |       |       |       |        
*      +-------+-------+-------+-------+-------+-------+-------+ 
*  y   |30     |31     |32     |33     |34     |35     |...    
*  ^   |       |       |       |       |       |       |       
*  |   |       |       |       |       |       |       |       
*  |   o-------+-------+-------+-------+-------+-------+-------+ 
*  |  origin                     width  = 108
*  +------------>x 
* (0,0) 
*	@brief 绘制战场上的格子信息，我方的是绿色的，对方的是红色的。战斗背景处理
*	攻击区域绘制显示,做逻辑处理相关
*/
/************************************************************************/
namespace BattleSpace{
	enum maptag
	{
		map_Layer = 10,
		map_EyeLayer,
		map_EyeLayerUp,
		map_EyeLayerDown,
		map_guide = 50,
		map_Bg  = 1000,
		map_UnchangeBg = 500,
	};
	class MoveRule;
	class WarAliveLayer;
	class SkillRange;
	class MapBackgroundManage;
	class WarManager;
	class BaseRole;
	class GuardArea;
	class WarMapLayer:public CCNode
	{
	public:
		WarMapLayer();
		virtual ~WarMapLayer();
		CREATE_FUNC(WarMapLayer);
		virtual void onExit();
		virtual bool init();
		void addEvent();	
		void removeEvent();
		void touchAreaCancel(CCObject* ob);
		void DrawAtkArea(BaseRole* alive);
		void DrawMoveArea(BaseRole* alive);
		void DrawMoveAtkArea(CCObject* ob);
		void DrawAtkEffect(CCObject* ob);
		void setBlackBg(CCObject* ob);
		void CombatArea(CCObject* ob);
		void CancelCombatArea(CCObject* ob);
		void SkillBtnDrawSkillArea(CCObject* ob);
		void MapShake(CCObject* ob);
		void OpenGuide(int gird);
		void CancelGuide();
		void CaptainHit(CCObject* ob);
		CC_PROPERTY(CLayout*,m_MapLayer,Maplayer);
		MapBackgroundManage* getBackgroundManage();
		void DrawWarningEffect(vector<int>& pVector);
		void CostAreaTips(CCObject* ob);
	protected:
		void createBackImage();
	private:
		MapBackgroundManage* m_BackgroundManage;//背景交由它处理
		CCSpriteBatchNode* m_DisPlayArea;		//可移动区域
		CCNode* m_GridIndex;					//用于显示格子标记(测试用)
		SkillRange* m_SkillRange;				//攻击区域计算
		GuardArea* mGuardArea;					//武将警戒区域
		MoveRule* m_move;						//移动区域计算
		vector<int> m_VecGridIndex;				//存储敌方武将按钮显示的格子
		WarManager* m_Manage;
		bool m_BackArea;						//反向攻击范围内是否存在敌方目标
		bool m_FrontArea;						//正向攻击区域
	};
};
#endif // !_WAR_MAP_LAYER_H_