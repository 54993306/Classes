/************************************************************* 
 *
 *
 *		Data : 2016.5.26
 *	
 *		Name : BattleField
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
*************************************************************/

#ifndef __BattleField__
#define __BattleField__

#include "protos/stage_protocol.pb.h"

//场地效果类型与参数说明
//风暴：参数1表示风向(1聚拢，2扩散，3向左，4向右，5向上，6向下)，参数2表示移动格子数
//陨石：参数1表示伤害，参数2为0
//死者：参数1、2为0，参数3为出现的怪物
//黑暗：无
//泥地：参数1表示基本范围变化类型(文档中的5种类型)，参数2为0
//复活：无
//场地效果
struct BattleField
{
	BattleField();
	//CMonster monster;	//参数3(死者效果参数)
	int id;				//陷阱id 
	int trapType;		//效果类型 (1 风暴，2 陨石，3 死者，4 黑暗，5 泥地，6 复活)
	std::string name;		//效果名称
	int interval;		//发动间隔(秒)
	int limit;			//最大发动次数
	int param1;			//参数1
	int param2;			//参数2
	int batch;			//批次
	void readData(const protos::FieldEff &fieldeff);
};

#endif