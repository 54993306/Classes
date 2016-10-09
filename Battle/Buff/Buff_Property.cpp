/************************************************************* 
 *
 *
 *		Data : 2016.9.20
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/
#include "Battle/Buff/Buff_Property.h"
#include "Battle/BattleMessage.h"
#include "Battle/RoleObjectMacro.h"
#include "Battle/BaseRoleData.h"
namespace BattleSpace
{
	Buff_Blood::Buff_Blood( const BuffData*pData,BaseRole*pRole ):BattleBuff(pData,pRole)
	{

	}

	Buff_Blood* Buff_Blood::CreateBuff( const BuffData*pData,BaseRole*pRole )
	{
		Buff_Blood* tBuff = new Buff_Blood(pData,pRole);
		if (tBuff && tBuff->init())
		{
			tBuff->autorelease();
			return tBuff;
		}else{
			delete tBuff;
			tBuff = nullptr;
			return nullptr;
		}
	}

	void Buff_Blood::ExcuteLogic(bool pExcute)
	{
		if ( !pExcute ) return;
		if (getRole()->getMonsterSpecies() == sMonsterSpecies::eWorldBoss)
			return;			//世界boss免疫buff效果
		bNotification->postNotification(B_UpdateBuffEffect,getRole());				//每次加减血时显示一次buff的特效
		if (getDBuff())
		{			
			getRole()->playBooldNum(PlayHpType::generalType, bloodChange() );
		}else{
			getRole()->playBooldNum(PlayHpType::gainType, bloodChange() );
		}
		if (getRole()->getHp()<=0)						//掉血类buff才会出现这样的情况
			getRole()->roleDie();
	}

	int Buff_Blood::bloodChange()
	{
		int tHpNum = 0;
		if (getBF_Number())//血量按数值变化
		{
			tHpNum = getBF_Number();
			if (getDBuff())
			{
				getRole()->setHp(getRole()->getHp() - tHpNum);
			}else{
				getRole()->setHp(getRole()->getHp() + tHpNum);
			}
		}
		if (getBF_Rate())//影响武将血量百分比
		{
			tHpNum += getBF_Rate() * 0.01f * getRole()->getHp();
			if (getDBuff())
			{
				getRole()->setHp(getRole()->getHp() - tHpNum);			//扣血和加血统一显示一次即可
			}else{
				getRole()->setHp(getRole()->getHp() + tHpNum);
			}
		}
		return tHpNum;
	}

	/*******************************************************************/
	Buff_Attack::Buff_Attack( const BuffData*pData,BaseRole*pRole ):BattleBuff(pData,pRole)
	{}

	Buff_Attack* Buff_Attack::CreateBuff( const BuffData*pData,BaseRole*pRole )
	{
		Buff_Attack* tBuff = new Buff_Attack(pData,pRole);
		if (tBuff && tBuff->init())
		{
			tBuff->autorelease();
			return tBuff;
		}else{
			delete tBuff;
			tBuff = nullptr;
			return nullptr;
		}
	}

	void Buff_Attack::ExcuteLogic( bool pExcute )
	{
		if (getBF_Number())		//影响武将攻击数值
		{
			if (getDBuff() == pExcute)
			{
				getRole()->setAtk(getRole()->getAtk() - getBF_Number());
			}else{
				getRole()->setAtk(getRole()->getAtk() + getBF_Number());
			}
		}
		if ( getBF_Rate() )		//影响武将攻击百分比
		{
			float tRate = getBF_Rate() * getRole()->getBaseData()->getRoleAttack();
			if (getDBuff() == pExcute)
			{
				getRole()->setAtk(getRole()->getAtk() - tRate);
			}else{
				getRole()->setAtk(getRole()->getAtk() + tRate);
			}
		}
	}

	/*******************************************************************/
	Buff_Defense::Buff_Defense( const BuffData*pData,BaseRole*pRole ):BattleBuff(pData,pRole)
	{}

	Buff_Defense* Buff_Defense::CreateBuff( const BuffData*pData,BaseRole*pRole )
	{
		Buff_Defense* tBuff = new Buff_Defense(pData,pRole);
		if (tBuff && tBuff->init())
		{
			tBuff->autorelease();
			return tBuff;
		}else{
			delete tBuff;
			tBuff = nullptr;
			return nullptr;
		}
	}

	void Buff_Defense::ExcuteLogic( bool pExcute )
	{
		if ( getBF_Number() )//影响武将防御数值
		{
			if ( getDBuff() == pExcute )
			{
				getRole()->setDef(getRole()->getDef() - getBF_Number());
			}else{
				getRole()->setDef(getRole()->getDef() + getBF_Number());
			}
		}
		if ( getBF_Rate() )//影响武将防御百分比
		{
			float tRate = getBF_Rate() * getRole()->getBaseData()->getRoleDefense();
			if ( getDBuff() == pExcute )
			{
				getRole()->setDef(getRole()->getDef() - tRate);
			}else{
				getRole()->setDef(getRole()->getDef() + tRate);
			}
		}
	}

	/*******************************************************************/
	Buff_Crit::Buff_Crit( const BuffData*pData,BaseRole*pRole ):BattleBuff(pData,pRole)
	{}

	Buff_Crit* Buff_Crit::CreateBuff( const BuffData*pData,BaseRole*pRole )
	{
		Buff_Crit* tBuff = new Buff_Crit(pData,pRole);
		if (tBuff && tBuff->init())
		{
			tBuff->autorelease();
			return tBuff;
		}else{
			delete tBuff;
			tBuff = nullptr;
			return nullptr;
		}
	}

	void Buff_Crit::ExcuteLogic( bool pExcute )
	{
		if ( getBF_Number() )//影响武将暴击数值
		{
			if (getDBuff() == pExcute)
			{
				getRole()->setCrit(getRole()->getCrit() - getBF_Number());
			}else{
				getRole()->setCrit(getRole()->getCrit() + getBF_Number());
			}
		}
		if ( getBF_Rate() )//影响武将暴击百分比
		{
			float tRate = getBF_Rate() * getRole()->getBaseData()->getRoleCrit();
			if (getDBuff() == pExcute)
			{
				getRole()->setCrit(getRole()->getCrit() - tRate);
			}else{
				getRole()->setCrit(getRole()->getCrit() + tRate);
			}
		}
	}

	/*******************************************************************/
	Buff_Dodge::Buff_Dodge( const BuffData*pData,BaseRole*pRole ):BattleBuff(pData,pRole)
	{}

	Buff_Dodge* Buff_Dodge::CreateBuff( const BuffData*pData,BaseRole*pRole )
	{
		Buff_Dodge* tBuff = new Buff_Dodge(pData,pRole);
		if (tBuff && tBuff->init())
		{
			tBuff->autorelease();
			return tBuff;
		}else{
			delete tBuff;
			tBuff = nullptr;
			return nullptr;
		}
	}

	void Buff_Dodge::ExcuteLogic( bool pExcute )
	{
		if (getBF_Number())//影响武将暴击数值
		{
			if (getDBuff() == pExcute)
			{
				getRole()->setDoge(getRole()->getDoge() - getBF_Number());
			}else{
				getRole()->setDoge(getRole()->getDoge() + getBF_Number());
			}
		}
		if ( getBF_Rate() )//影响武将暴击百分比
		{
			float tRate = getBF_Rate() * getRole()->getBaseData()->getRoleDodge();
			if (getDBuff() == pExcute)
			{
				getRole()->setDoge(getRole()->getDoge() - tRate);
			}else{
				getRole()->setDoge(getRole()->getDoge() + tRate);
			}
		}
	}

	/*******************************************************************/
	Buff_HitRate::Buff_HitRate( const BuffData*pData,BaseRole*pRole ):BattleBuff(pData,pRole)
	{}

	Buff_HitRate* Buff_HitRate::CreateBuff( const BuffData*pData,BaseRole*pRole )
	{
		Buff_HitRate* tBuff = new Buff_HitRate(pData,pRole);
		if (tBuff && tBuff->init())
		{
			tBuff->autorelease();
			return tBuff;
		}else{
			delete tBuff;
			tBuff = nullptr;
			return nullptr;
		}
	}

	void Buff_HitRate::ExcuteLogic( bool pExcute )
	{
		if (getBF_Number())//影响武将命中数值
		{
			if (getDBuff() == pExcute)
			{
				getRole()->setHit(getRole()->getHit() - getBF_Number());
			}else{
				getRole()->setHit(getRole()->getHit() + getBF_Number());
			}
		}
		if (getBF_Rate())//影响武将命中百分比
		{
			float tRate = getBF_Rate() * getRole()->getBaseData()->getRoleHit();
			if (getDBuff() == pExcute)
			{
				getRole()->setHit(getRole()->getHit() - tRate);
			}else{
				getRole()->setHit(getRole()->getHit() + tRate);
			}
		}
	}

	/*******************************************************************/
	Buff_MoveSpeed::Buff_MoveSpeed( const BuffData*pData,BaseRole*pRole ):BattleBuff(pData,pRole)
	{}

	Buff_MoveSpeed* Buff_MoveSpeed::CreateBuff( const BuffData*pData,BaseRole*pRole )
	{
		Buff_MoveSpeed* tBuff = new Buff_MoveSpeed(pData,pRole);
		if (tBuff && tBuff->init())
		{
			tBuff->autorelease();
			return tBuff;
		}else{
			delete tBuff;
			tBuff = nullptr;
			return nullptr;
		}
	}

	void Buff_MoveSpeed::ExcuteLogic( bool pExcute)
	{
		if ( getBF_Number() )//影响武将移动速度数值    
		{
			if (getDBuff() == pExcute)
			{
				getRole()->setMoveSpeed(getRole()->getMoveSpeed() + getBF_Number() );
			}else{
				float speed = getRole()->getMoveSpeed() - getBF_Number();
				if (speed <= 0.2f)
					speed = 0.2f;				//武将移动最快速度极限值处理
				getRole()->setMoveSpeed(speed);
			}
		}
		if (getBF_Rate())//影响武将移动速度百分比
		{
			float tSpeed =getRole()->getBaseData()->getMoveSpeed();					//s/格
			if (getDBuff() == pExcute)
			{
				float tNewSpeed = tSpeed/(tSpeed/getRole()->getMoveSpeed()-getBF_Rate());
				if (tNewSpeed < 0.2f)
					tNewSpeed = 0.2f;
				getRole()->setMoveSpeed(tNewSpeed);
			}else{
				float tNewSpeed = tSpeed/(tSpeed/getRole()->getMoveSpeed()+getBF_Rate());
				if (tNewSpeed < 0.2f)
					tNewSpeed = 0.2f;
				getRole()->setMoveSpeed(tNewSpeed);
			}
		}
	}

	Buff_AttackSpeed::Buff_AttackSpeed( const BuffData*pData,BaseRole*pRole ):BattleBuff(pData,pRole)
	{}

	Buff_AttackSpeed* Buff_AttackSpeed::CreateBuff( const BuffData*pData,BaseRole*pRole )
	{
		Buff_AttackSpeed* tBuff = new Buff_AttackSpeed(pData,pRole);
		if (tBuff && tBuff->init())
		{
			tBuff->autorelease();
			return tBuff;
		}else{
			delete tBuff;
			tBuff = nullptr;
			return nullptr;
		}
	}
	//传过来的数值和百分比都是对敏捷造成的影响
	void Buff_AttackSpeed::ExcuteLogic( bool pExcute )
	{
		//攻速 = 2 - (敏捷*0.2/400)
		float tAgility = ( 2-getRole()->getAtkInterval() )*400/0.2f;			//当前敏捷值
		if ( getBF_Number() )//影响武将攻击速度数值
		{
			if ( getDBuff() == pExcute )
			{
				tAgility -= getBF_Number();
			}else{
				tAgility += getBF_Number();
			}
			getRole()->setAtkInterval(2-(tAgility*0.2f/400));
		}
		if ( getBF_Rate() )//影响武将移动速度百分比
		{
			float BeingAgility = ( 2-getRole()->getBaseData()->getAttackSpeed() )*400/0.2f;		//增减的敏捷值
			if ( getDBuff() == pExcute )
			{
				tAgility -= getBF_Rate() *0.01f* BeingAgility;
			}else{
				tAgility += getBF_Rate() *0.01f* BeingAgility;
			}
			getRole()->setAtkInterval(2-(tAgility*0.2f/400));
		}
	}
}