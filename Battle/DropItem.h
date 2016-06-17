#ifndef _DROPITEMDISPOSE_H_
#define _DROPITEMDISPOSE_H_
#include "AppUI.h"
namespace BattleSpace{
	class WarControl;
	class BattleRoleLayer;
	/************************************************************************/
	/*          
	//  [2/3/2015 xc_lin]
	武将死亡道具掉落的情况处理
	武将死亡时将自己以消息的形式发出来,在这里接收到这个消息,而后进行装备掉落处理
	*/
	/************************************************************************/
	class DropItem : public CCObject
	{
	public:
		virtual bool init();
		DropItem();
		~DropItem();
		CREATE_FUNC(DropItem);
		void AddEvent();
		void RemoveEnent();
		void ItemDropDispose(CCObject* ob);
		void setWarUi(WarControl* ui,BattleRoleLayer*layer);
		void changeGoldNum(CCObject*ob);
	private:
		WarControl* m_ui;
		BattleRoleLayer* m_alive;
	};
};
#endif