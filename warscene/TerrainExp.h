#ifndef _TER_EXPRESSION_H_
#define _TER_EXPRESSION_H_
#include "AppUI.h"
/************************************************************************/
/*          
		地形显示处理					//  [9/9/2014 xc_lin]
*/
/************************************************************************/
namespace BattleSpace{
	class ActObject;
	class WarAlive;
	class terRound;

	class TerrainExp : public CCObject
	{
	public:
		TerrainExp();
		virtual ~TerrainExp();
		virtual bool init();
		CREATE_FUNC(TerrainExp);
		void playerTerrainEffect(CCObject* ob);
		float DataHandle(WarAlive* alive,float time , terRound* round,CCArray* m_task);		//对数据进行二次处理调试数据在此处进行
		float StateHandle(ActObject* aliveOb,WarAlive* alive,float sysTime ,float lostHp ,float currHp,CCArray* m_task);	//所有的类似的对象都应该是这样的两个函数
	private:

	};
};
#endif