#ifndef _WAR_SCENE_H_
#define _WAR_SCENE_H_
#include "AppUI.h"
#include "Global.h"
namespace BattleSpace{
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
	class StoryLayer;
	class WarMapLayer;
	class WarAliveLayer;
	class TerrainLayer;
	class CombatGuideLayer;
	class CombatLogic;
	class DropItem;
	class WarControl;
	enum WarSceneTouchPriority
	{
		guideLayerPriority = -11,		//引导层
		StoryPriority,					//剧情层优先级
		GameEditorPriority,
		WarControlPriority,				//UI层
		AliveLayerPriority,				//人物层优先级
	};

	enum warScengtag
	{
		gono = 0,
		win,	
		fail,
		sendBatch,
		m_alive_tag,		
		effec_tag,				//必杀技光环
		effec1_tag,
		effec2_tag,
		criticon_tag,
		attackicon_tag,
		fire_tag,
		batch_tag,
		backLayer_tag,
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
	public:
		CC_SYNTHESIZE(CCNode*,m_MoveLayer,MoveLayer);
		CC_SYNTHESIZE(WarMapLayer*,m_MapLayer,WarMapLayer);
		CC_SYNTHESIZE(WarAliveLayer*,m_AliveLayer,WarAliveLayer);
		CC_SYNTHESIZE(TerrainLayer*,m_TerrainLayer,TerrainLayer);
		CC_SYNTHESIZE(WarControl*,m_UILayer,WarUI);
		CC_SYNTHESIZE(StoryLayer*,m_StoryLayer,StoryLayer);
		CC_SYNTHESIZE(CombatLogic*,m_Loginc,CombatLogic);
		CC_SYNTHESIZE(CombatGuideLayer*,m_GuideLayer,CombatGuideLayer);
		CCArray* getTaskArray();
	protected:
		CCPoint m_StartPos;				//记录触摸起始点
		DropItem* _dropItem;
		CCTouch * m_Touch;
	};
};
#endif // !_WAR_SCENE_H_