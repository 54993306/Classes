/** 
 *
 *
 *		Data : 2016.4.12
 *	
 *		Name : GameRoleBaseData
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : ��Ϸ��ÿ���佫����Ҫ��������չ��Ϣ
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