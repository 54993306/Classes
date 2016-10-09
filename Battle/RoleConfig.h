/************************************************************* 
 *
 *
 *		Data : 2016.8.26
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/

#ifndef __ROLECONFIG__
#define __ROLECONFIG__
#include "AppUI.h"

namespace BattleSpace
{
	class RoleConfigData;
	class RoleConfig : public CCObject
	{
	protected:
		RoleConfig();
		virtual bool init();
	public:
		~RoleConfig();
		CREATE_FUNC(RoleConfig);
		void ClearData();
		void AddConfigData(RoleConfigData* Data);
		RoleConfigData* getConfigData(int ModeId);
	private:
		std::map<int,RoleConfigData*> mMapData;
		RoleConfigData* mDefultData;
	};
}
#endif