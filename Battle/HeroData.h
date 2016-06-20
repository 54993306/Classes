/************************************************************* 
 *
 *
 *		Data : 2016.6.6
 *	
 *		Name : HeroData
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __HeroData_H_
#define __HeroData_H_
#include "Battle/BaseRoleData.h"
namespace BattleSpace{
	//гЂал
	class HeroData :public BaseRoleData 
	{
	public:
		HeroData();
		static HeroData* create();
		void readData(const protos::common::Hero* hero);
	};
};
#endif