/************************************************************* 
*
*
*		Data : 2016.8.16
*	
*		Name : BattleTrap
*
*		Author : Lin_Xiancheng
*
*		Description : 差异的部分暴露出相同的借口在差异类的内部实现相应的逻辑
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
		float getRate() const;							//有正负值
		int getDamage() const;
		int getRotation() const;
	public:					
		virtual void updateState(float dt);	
		virtual bool throughJudge(BaseRole* pRole = nullptr);					//除了属性外，还需要做一个移动模式的可穿越判断
		CC_SYNTHESIZE(bool,mActivate,Activate);			//地形的激活状态
		CC_SYNTHESIZE(BaseRole*,mTriggerRole,TriggerRole);
	protected:
		BattleTrap(const TrapData* pData);
		virtual void ExcuteAI();						//默认的地形AI方法,有需要的情况子类可以重写该方法
		virtual bool RoleJudge();
		virtual void TrapImpact();
		virtual void booldImpact();
		virtual void buffImpact();
	protected:
		bool mExcute;
		float mTriggerSpace;
		const TrapData* mConstData;						//存储一个数据的指针,这个带一个const的代价是，TrapManage跟数据类trapData耦合了
	};
};
#endif