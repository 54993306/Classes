#ifndef _WARHANDLE_H_
#define _WARHANDLE_H_
#include <string>
#include "AppUI.h"
/************************************************************************/
/*         
用于辅助warScene显示使用			//  [9/9/2014 xc_lin]
*/
/************************************************************************/
namespace BattleSpace{

	class BattleRoleLayer;
	class WarControl;
	class BattleScene;
	class WarManager;
	class BaseRole;
	class WarAssist : public CCObject
	{
	public:
		WarAssist();
		~WarAssist();
		virtual bool init();
		CREATE_FUNC(WarAssist);
		void setScene(BattleScene* scene);
		void displayBossWarningBegin(BaseRole* pRole = nullptr);
		void WinEffect(CCObject* ob);
		void playerMusic(CCObject* ob);
		void shake(CCObject* ob);
		void PlayBeginAnimation(CCObject* obj);
		void animationEvent(CCArmature *armature,MovementEventType movementType, const char *movementID);
		void frameEvent(CCBone *pBone, const char *str, int a, int b);
		void bigMonsterInBattle(CCObject* ob);
		void starActionEnd(CCObject* ob);
		bool isBigBoss(BaseRole* pRole);
	private:

		void changeUiVisiable( bool bVisiable );

		//new-0 地图缩放至75%  1红色阴影-闪电-警告 2.boss出场动作 2名字，称号（白光进。白光出）3.地图缩放至87%+白光+震动 4.地图缩放至100%+震动
		//moveLay移动到某一位置
		float moveLayMoveTo( CCPoint pos );
		//moveLay移动量
		float moveLayMoveBy( CCPoint pos );

		//计算moveLay相对屏幕底边中心的锚点
		CCPoint getMoveLayAnchorpointWithSceneBottomCenter();
		//moveLay重置锚点（缩放用）
		void moveLayResetAnchorpointKeepSamePos(CCPoint anchorpoint);
		//moveLay缩放至xx%
		void moveLayScaleTo(float fScale, float fTime );
		//moveLay重置锚点为zero
		void moveLayResetAnchorPoint();
		//添加红色阴影
		float addRedShadow();
		//添加闪电
		float addLighting();
		//添加警告字样（可能是方块）
		float addWarningTip(  );
		//添加白光
		float addWhiteLight( float fDelay=0 );
		//添加科技条
		float addYellowBar(  );
		//moveLay震动
		float moveLayShake(float fDelay);
		//boss出场动作
		void bossShowAppearAction();
		//名字称号
		float addInfoPanel( );
		//战斗字样
		float addFightText( );

		//fix地图边缘
		float fixMoveLayBorder( float fGap );

		//整个动画结束
		//地图缩放开始动作
		float moveLayScaleStart( float fScale, float fTime );

		//地图缩放回去动作

		void displayBossWarningEnd();

		//回调-1
		//非引导界面缩放，引导界面，不缩放(补偿偏移)
		void bigBossCallback_1();
		//回调0
		//非引导界面缩放，引导界面，不缩放(移动到左侧目标位置)
		void bigBossCallback0();
		//回调1
		//显示warning
		void bigBossCallback1();
		//回调2
		//拖屏幕,普通关卡才会拖屏幕到最左边//精英。引导关卡,不滑
		void bigBossCallback2();
		//回调3
		//boss出场动作已经播放了？
		void bigBossCallback3();
		//回调4
		//4名字称号
		void bigBossCallback4();
		//回调5
		//地图缩放至87%
		//非精英且非引导才有
		void bigBossCallback5();
		//回调6
		//地图缩放至100%+震动
		//非引导才有
		void bigBossCallback6();
		//回调7
		//精英关卡置回右边
		void bigBossCallback7();

	private:
		BattleRoleLayer* m_alive;			//用于存储战场的武将层
		WarControl* m_ui;
		BattleScene* m_scene;
		WarManager* mManage;
		bool m_bShowWarning;			//warning动画状态锁
		bool m_bIsBigBoss;					//是否为bigBoss
		int	m_iModel;							//modeID
		CCPoint m_pPosCopy;				//动画开始时的坐标
		BaseRole* mBossRole;
	};
};
#endif