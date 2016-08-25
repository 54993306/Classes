/************************************************************* 
*
*
*		Data : 2016.8.16
*	
*		Name : BattleTrap
*
*		Author : Lin_Xiancheng
*
*		Description : ����Ĳ��ֱ�¶����ͬ�Ľ���ڲ�������ڲ�ʵ����Ӧ���߼�
*
*
*************************************************************/

#ifndef __BattleTrap_H_
#define __BattleTrap_H_
#include "cocos2d.h"
using namespace cocos2d;
using namespace std;
namespace BattleSpace
{
	class TrapData;
	class BaseRole;
	enum struct sTrapType;
	enum struct sRoleNature;
	class BattleTrap : public CCObject
	{
	public:
		static BattleTrap* createDefult(const TrapData* pData);
		static BattleTrap* createTrap(const TrapData* pData);
	public:
		int getTrapID() const;
		int getTrapModel() const;
		sTrapType getTrapType() const;
		int getGridIndex() const;	
		int getAtbType() const;
		float getRate() const;							//������ֵ
		int getDamage() const;
		int getRotation() const;
	public:					
		virtual void updateState(float dt);	
		virtual bool throughJudge(BaseRole* pRole = nullptr);					//���������⣬����Ҫ��һ���ƶ�ģʽ�Ŀɴ�Խ�ж�
		CC_SYNTHESIZE(bool,mActivate,Activate);			//���εļ���״̬
		CC_SYNTHESIZE(BaseRole*,mTriggerRole,TriggerRole);
	protected:
		BattleTrap(const TrapData* pData);
		virtual void ExcuteAI();						//Ĭ�ϵĵ���AI����,����Ҫ��������������д�÷���
		virtual bool RoleJudge();
		virtual void TrapImpact();
		virtual void booldImpact();
		virtual void buffImpact();
	protected:
		bool mExcute;
		float mTriggerSpace;
		const TrapData* mConstData;						//�洢һ�����ݵ�ָ��,�����һ��const�Ĵ����ǣ�TrapManage��������trapData�����
	};
};
#endif