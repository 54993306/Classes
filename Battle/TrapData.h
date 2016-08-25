/************************************************************* 
 *
 *
 *		Data : 2016.5.26
 *	
 *		Name : BattleTrap
 *
 *		Author : Lin_Xiancheng
 *
 *		Description �� 
 *
 *
*************************************************************/

#ifndef __BattleTrap__
#define __BattleTrap__

namespace protos{
	namespace common{
		class Trap;
	}
}

#include "cocos2d.h"
using namespace cocos2d;

namespace BattleSpace
{
	enum struct sTrapType
	{
		eNullType			= 0,//
		eFireObstacle		= 100,//�������ϰ� Ԥ���Ŀ��ܴ������ͣ�Ŀǰ������
		eFireTrap			= 1,//�����Ե���
		eWaterTrap			= 2,//ˮ���Ե���
		eWoodTrap			= 3,//ľ���Ե���
		eObstacle			= 4,//�ϰ������(�����佫������ͨ��)
		ePlayerObstacle		= 5,//��Ҳ���ͨ��
		eMonsterObstacle	= 6,//���ﲻ��ͨ��
	};

	class BuffData;
	class TrapData : public CCObject
	{
	public:
		TrapData();
		virtual ~TrapData();
		static TrapData* createTrap(const protos::common::Trap *pTrap);
		CC_SYNTHESIZE(int,mTrapID,TrapID);
		CC_SYNTHESIZE(int,mTrapModel,TrapModel);			//����ID��Ҳ�ǵ���Ч��ID��ÿ�����ε�Ч���ǹ̶���
		CC_SYNTHESIZE(sTrapType,mTrapType,TrapType);		//��������
		CC_SYNTHESIZE(int,mPosition,Position);				//λ��
		CC_SYNTHESIZE(int,mAttribute,AtbType);				//��������
		CC_SYNTHESIZE(float,mRate,Rate);					//Ӱ�����
		CC_SYNTHESIZE(int,mDmage,Dmage);					//��ɵ��˺�
		CC_SYNTHESIZE(int,mTouch,TouchNum);					//����Ч��ID
		CC_SYNTHESIZE(int,mRound,RoundNum);					//��������Ч��ID
		CC_SYNTHESIZE(int,mRotatione,Rotatione);			//������ת�Ƕ�
		const BuffData* getTrapBuff() const;
	protected:
		void readData(const protos::common::Trap *pTrap);
	private:
		BuffData* mTrapBuff;								//���θ���buff �п���Ϊһ���б�
	};
	//
}


#endif