
/************************************************************* 
*
*
*		Data : 2016.9.6
*	
*		Name : BattleBuff
*
*		Author : Lin_Xiancheng
*
*		Description : 
*
*
*************************************************************/

#ifndef __BattleBuff_H_
#define __BattleBuff_H_
#include "cocos2d.h"
#include "Battle/BaseRole.h"
using namespace cocos2d;
using namespace std;
namespace BattleSpace
{
	enum struct sBuffState
	{
		eNull									= 0,//Buff
		eUnExcute								= 1,//buff��δ��ִ�й�
		eExcuted								= 2,//buff�Ѿ���ִ�й�
		eRemove									= 3,//buff���Ա��Ƴ�
	};

	class BuffData;
	enum struct sBuffType;
	class BattleBuff : public CCObject
	{
	protected:
		BattleBuff(const BuffData* pData,BaseRole* pRole);
		virtual bool init(); 
	public:
		static BattleBuff* CreateBuff(const BuffData* pData,BaseRole* pRole);
		static BattleBuff* CreateDefault(const BuffData* pData,BaseRole* pRole);
	public:									//�ʵ����Ƿ�װ�ԣ�ֻ��������Է���
		virtual BaseRole* getRole();
		virtual int getBuffID() const;
		virtual sBuffType getBuffType() const;
		virtual int getBF_Number() const;
		virtual float getBF_Rate() const;
		virtual int getTriggerRate() const;
		virtual bool getDBuff() const;
		virtual int getBF_Target() const;
		virtual int getBF_Level() const;
		virtual sRoleNature getBF_Property() const;	//��������(��1,ľ2,ˮ3,0�ǲ�����)
	public:
		virtual void updateBuff(float dt);
		virtual bool isRemove() const;
		virtual sBuffState getBF_State() const;
		CC_SYNTHESIZE(bool,mBF_FirastAdd,BF_FirstAdd);
		CC_SYNTHESIZE(float,mDuration,BF_Duration);
	protected:
		virtual void ExcuteLogic(bool pExcute){};
	private:
		BaseRole* mRole;
		const BuffData* mData;
		float mExcuteTime;
		sBuffState mBuffState;
	};
};
#endif