/************************************************************* 
 *
 *
 *		Data : 2016.5.26
 *	
 *		Name : BattleTrap
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __BattleTrap__
#define __BattleTrap__

#include <protos/common/trap_common.pb.h>

struct BattleTrap
{
	BattleTrap();
	int  id;					//关卡地形ID
	int  terrainId;				//地形标识ID
	std::string name;				//地形名称
	int  terrainType;			//地形类型
	int  pro_type;				//影响属性
	int  pro_rate;				//影响属性值
	int  damage;				//地形造成的伤害
	int  posX;					//在地图上的位置X
	int  posY;					//在地图上的位置Y
	int  triggerNum;			//被触发次数
	int	 existNum;				//存在时间
	int  batch;					//批次
	//CBuff buff;					//地形附带buf 
	void readData(const protos::common::Trap &terr);
};

#endif