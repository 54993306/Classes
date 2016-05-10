/** 
 *
 *		Data : 2016.4.12
 *
 *		Name : HeroRole
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : �ҷ��佫���ݽṹ
 *
 */


#include "Battle/GameRoleData.h"
#include <protos/protocol.h>

class HeroRole : public GameRoleData
{
public:
	HeroRole();
	static HeroRole*Create(const protos::common::Hero &hero);
	bool initHeroData(const protos::common::Hero &hero);
public:
	CC_SYNTHESIZE(bool		,Captain			,Captain);						//�Ƿ�Ϊ�ҷ���˧
	CC_SYNTHESIZE(int		,AlertType			,AlertType);					//������������
	CC_SYNTHESIZE(int		,AlertRange			,AlertRange);					//��������Χ
	CC_SYNTHESIZE(int		,Cooling			,Cooling);						//������������CD
	CC_SYNTHESIZE(int		,InitCost			,InitCost);						//��ʼCOST
	CC_SYNTHESIZE(int		,MaxCost			,MaxCost);						//COST����
	CC_SYNTHESIZE(int		,UseCost			,UseCost);						//��������cost
	CC_SYNTHESIZE(int		,CostRecover		,CostRecover);					//cost�ָ��ٶ�(1/s)

private:

};