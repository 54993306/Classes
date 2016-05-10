/** 
 *
 *
 *		Data : 2016.4.12
 *	
 *		Name : GameRoleBaseData
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 游戏中每个武将都需要产生的拓展信息
 *
 *
 */

#include "Battle/RoleBaseData.h"

class BuffManage;
class GameRoleData : public RoleBaseData
{
public:
	GameRoleData();
	~GameRoleData();
	CC_SYNTHESIZE(BuffManage*,m_BuffManage,BuffManage);

};