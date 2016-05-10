/** 
 *
 *		Data : 2016.4.12
 *	
 *		Name : MonsterData
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 定义敌方数据结构
 *
 */

#include "Battle/GameRoleData.h"
#include <protos/protocol.h>

class MonsterRole : public GameRoleData
{

public:
	MonsterRole();
	static MonsterRole*Create(const protos::common::Monster &monster);
	bool initMonsterData(const protos::common::Monster &monster);
public:
	CC_SYNTHESIZE(float		,CritTime		,CritTime);						//怪物释放必杀技时间

private:



};