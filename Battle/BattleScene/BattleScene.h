#ifndef _WAR_SCENE_H_
#define _WAR_SCENE_H_

/************************************************************* 
 *
 *
 *		Data : 2016.6.7
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/
#include "AppUI.h"
#include "Global.h"
namespace google {
	namespace protobuf{
		class Message;
	}
}
namespace BattleSpace
{
	class StoryLayer;
	class BattleMapLayer;
	class BattleRoleLayer;
	class TerrainLayer;
	class CombatGuideLayer;
	class CombatLogic;
	class DropItem;
	class WarControl;
	class WarBackLayer;
	class BattleClose;
	enum WarSceneTouchPriority
	{
		guideLayerPriority = -11,		//引导层
		StoryPriority,					//剧情层优先级
		GameEditorPriority,
		WarControlPriority,				//UI层
		AliveLayerPriority,				//人物层优先级
	};

	class BattleScene : public CScene,public CCTargetedTouchDelegate
	{
	public:
		BattleScene();
		virtual ~BattleScene();
		DEFINE_SCENE_CLASS(BattleScene);
		virtual void onCreate();
		virtual void onEnter();
		virtual void onEnterTransitionDidFinish();
		virtual void onExit();
		void AddEvent();
		void RemoveEvent();
		void LayerMoveEnd(CCObject* ob);
		virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
		virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
		virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
		virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
		void cReturnLayer(CCObject* ob);
	public:
		CC_SYNTHESIZE(CCNode*,m_MoveLayer,MoveLayer);
		CC_SYNTHESIZE(BattleMapLayer*,m_MapLayer,BattleMapLayer);
		CC_SYNTHESIZE(BattleRoleLayer*,m_AliveLayer,BattleRoleLayer);
		CC_SYNTHESIZE(TerrainLayer*,m_TerrainLayer,TerrainLayer);
		CC_SYNTHESIZE(WarControl*,m_UILayer,WarUI);
		CC_SYNTHESIZE(StoryLayer*,m_StoryLayer,StoryLayer);
		CC_SYNTHESIZE(CombatLogic*,m_Loginc,CombatLogic);
		CC_SYNTHESIZE(CombatGuideLayer*,m_GuideLayer,CombatGuideLayer);
		CC_SYNTHESIZE(bool,mMoveState,MoveState);
		void OnBattleFinish(int type, google::protobuf::Message *msg);
		void onWordBossFinish(int type, google::protobuf::Message *msg);
		void onPVEBattleFinish(int type, google::protobuf::Message *msg);
	protected:
		CCPoint m_StartPos;				//记录触摸起始点
		DropItem* _dropItem;
		CCTouch * m_Touch;
		WarBackLayer* mBackLayer;
		BattleClose* mBattleClose;
	};
};
#endif // !_WAR_SCENE_H_