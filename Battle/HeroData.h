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
namespace BattleSpace
{
	class HeroData :public BaseRoleData
	{
	public:
		virtual ~HeroData(){};
		static HeroData* create(const google::protobuf::Message* pMessage = nullptr);
	public:
		CC_SYNTHESIZE(int,mAlertType,AlertType);						//��������
		CC_SYNTHESIZE(int,mAlertRange,AlertRange);						//���䷶Χ
		CC_SYNTHESIZE(int,mColdDown,ColdDown);							//����CD
		CC_SYNTHESIZE(int,mInitCost,InitCost);							//����Я��cost			����ֱ�Ӽ������������Ҫ�ٵ���Я����
		CC_SYNTHESIZE(int,mExpendCost,ExpendCost);						//��������cost
		CC_SYNTHESIZE(int,mCostSpeed,CostSpeed);						//cost�ָ��ٶ�(1/s)
		CC_SYNTHESIZE(int,mMaxCost,MaxCost);							//cost����
		CC_SYNTHESIZE(bool,mCaptain,Captain);							//�佫�Ƿ�Ϊ����״̬(ս���б�Ƕӳ�)
	protected:
		virtual void parseData(const google::protobuf::Message* pMessage);
		HeroData(const google::protobuf::Message* pMessage = nullptr);
	};
};
#endif