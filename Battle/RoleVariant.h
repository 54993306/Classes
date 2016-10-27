/************************************************************* 
*
*
*		Data : 2016.8.29
*	
*		Name : RoleVariant
*
*		Author : Lin_Xiancheng
*
*		Description : 
*
*
*************************************************************/

#ifndef __RoleVariant_H_
#define __RoleVariant_H_
#include "cocos2d.h"
using namespace cocos2d;
using namespace std;
namespace BattleSpace
{
	enum struct eVariantState
	{
		eNull					= 0,
		eInVariant				= 1,	//在变身状态
		eCooldown				= 2,	//变身CD状态
		eCooling				= 3,	//变身冷却状态
	};
	class BaseRole;
	class RoleConfigData;
	class RoleVariant : public CCObject
	{
	protected:
		RoleVariant(BaseRole* pRole);
		virtual bool init();
	public:
		virtual ~RoleVariant(){};
		static RoleVariant* CreateVariant(BaseRole* pRole);
		void updateVariant(float dt);
	public:
		virtual void PropertyChange();
		virtual bool isVariant();
	protected:
		virtual bool variantJudge();
		virtual void variantExcute();
		virtual void updateRage(float pRate);
	protected:
		float mUserTime;
		float mTime;
		float mTouchTime;
		bool mVariantProcess;
		bool mBlinkState;
		BaseRole* mRole;
		RoleConfigData* mConfigData;	//武将变身数据都是从配置文件这边获取
		eVariantState mVariantState;
	};
};
#endif