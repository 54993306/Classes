#include "BuffManage.h"
#include "Battle/BaseRole.h"
#include "model/DataCenter.h"
#include "Battle/RoleObject/HPObject.h"
#include "Battle/BaseRole.h"
#include "Battle/RoleObject/RoleObject.h"
#include "Battle/ConstNum.h"
#include "Battle/BattleMessage.h"
#include "Battle/BaseRoleData.h"
#include "Battle/skEffectData.h"
#include "Battle/BuffData.h"
#include "Battle/RoleObjectMacro.h"

using namespace cocos2d::extension;
namespace BattleSpace
{
#define BUFFMAX (8)

	BuffManage::BuffManage():mRole(nullptr)
	{}

	BuffManage::~BuffManage()
	{
		for (auto buf:m_BuffMap)
		{
			CC_SAFE_RELEASE(buf.second);
		}
		m_BuffMap.clear();
		for (auto p : m_EffectMap)
			p.second.clear();
		m_EffectMap.clear();
		mRole = nullptr;
	}

	void BuffManage::Buffclear()
	{
		vector<int> Vec;
		for (auto i:m_BuffMap)
			Vec.push_back(i.first);
		for (auto j:Vec)
			removeBuf(j);
		m_BuffMap.clear();
		m_EffectMap.clear();
	}

	vector<CCNode*>* BuffManage::getVecEffect( int buffid )
	{
		BuffEffectMapList::iterator iter = m_EffectMap.find(buffid);
		if (iter != m_EffectMap.end())
			return &iter->second;
		return nullptr;
	}

	void BuffManage::AddEffectVec( int buffid,vector<CCNode*>& Vec )
	{
		BuffEffectMapList::iterator iter = m_EffectMap.find(buffid);
		if (iter != m_EffectMap.end())
		{
			CCLOG("[ ERROR ] BuffManage::AddEffectVec BuffEffect Vector Repeat id=%d",buffid);
			return ;
		}
		if (Vec.size())
			m_EffectMap[buffid] = Vec;
	}

	void BuffManage::AddBuff(const BuffData* pData)
	{
		BuffInfo* buff = getbuff(pData->getBuffID());
		if (buff)
		{
			buff->setBuffDuration(pData->getBuffDuration());
			return;
		}
		if (AddBuffLogic(pData))
			return;
		BuffInfo* tBuffinfo = BuffInfo::create(pData);
		tBuffinfo->retain();									//clear方法里面safa release的作用就是在这了
		ExcuteBuff(tBuffinfo);									//执行buff逻辑
		m_BuffMap[tBuffinfo->getBuffID()] = tBuffinfo;			//map添加信息的方法	
		if (mRole->getHp()<=0)
			mRole->roleDie();
		CCLOG("[ Tips ] BuffManage::AddBuff succeed bufID = %d",pData->getBuffID());
		NOTIFICATION->postNotification(B_AddBuff,mRole);
	}

	BuffInfo* BuffManage::getbuff(int bufID)
	{
		BuffMap::iterator iter = m_BuffMap.find(bufID);
		if (iter != m_BuffMap.end())
			return iter->second;
		return nullptr;
	}

	BuffInfo* BuffManage::getbuffbyType(int buftype)
	{
		for (auto i : m_BuffMap)
		{
			if (i.second->getBuffType() == buftype)
				return i.second;
		}
		return nullptr;
	}

	void BuffManage::removeBuf(int id)
	{
		BuffMap::iterator iter = m_BuffMap.find(id);
		if (iter != m_BuffMap.end())								//按ket值和具体内容来进行索引的
		{
			BuffEffectMapList::iterator piter = m_EffectMap.find(id);
			if (piter != m_EffectMap.end())
			{
				for (auto i:piter->second)
				{
					i->removeFromParentAndCleanup(true);			//移除buff相应的效果
					i = nullptr;
				}				
				m_EffectMap.erase(piter);
				NOTIFICATION->postNotification(B_RemoveBuff,mRole);//刷新剩余图标信息
			}else{
				CCLOG("[ ERROR ] BuffManage::removeBuf BuffEffect Vector is NULL");
			}
			if (iter->second->getHandle())
				ExcuteBuff(iter->second,false);						/*消除原有buf的影响*/
			CC_SAFE_RELEASE(iter->second);
			iter->second = nullptr;
			m_BuffMap.erase(iter);
		}
	}

	void BuffManage::clearDbuf()
	{
		vector<int> DBuff;
		for (auto i:m_BuffMap)
		{
			if (i.second->getIsDBuff())
				DBuff.push_back(i.first);
		}
		for (auto j:DBuff)
			removeBuf(j);
	}

	void BuffManage::upDateBuff( float dt )
	{
		vector<int> RemoveFirst;
		for (auto iter:m_BuffMap)					//c++11遍历map
		{
			BuffInfo* info = iter.second;
			if (info->getBuffDuration()<dt)				//持续时间已经达到,标记移除buff
			{
				info->setBuffDuration(0);
				RemoveFirst.push_back(iter.first);
				continue;
			}
			info->setBuffDuration(info->getBuffDuration() - dt);
			info->setExTime(dt);
			if (info->getExcute())
				ExcuteBuff(info);					//执行buff逻辑
		}
		for (auto i:RemoveFirst)					//执行移除操作
			removeBuf(i);
		if (mRole->getHp()<=0)
			mRole->roleDie();
	}

	//返回true表示不添加新Buf
	bool BuffManage::AddBuffLogic(const BuffData* pData)
	{
		BuffInfo* oldBuff = getbuffbyType(pData->getBuffType());
		if (oldBuff)															/*存在相同类型的buf*/
		{
			if (oldBuff->getIsDBuff() == pData->getIsDBuff())								/*同为增益或减益*/
			{
				if(oldBuff->getBuffLevel() > pData->getBuffLevel())
					return true;
				CCLOG(" [oldBuff_Level <= newBuf_Level] oldBufid:%d,newBufid:%d",oldBuff->getBuffID(),pData->getBuffID());
				removeBuf(oldBuff->getBuffID());									/*高级替换低级*/
			}else{
				CCLOG("newBUFF=%d Replace oldBUFF=%d",pData->getBuffID(),oldBuff->getBuffID());									
				removeBuf(oldBuff->getBuffID());									/*增减益互相替换*/
			}
		}else{
			if (m_BuffMap.size() < BUFFMAX)									
				return false;
			int bufid = 0;														/*超过8个类型的buf存在时移除持续时间最短的buf*/
			float dration = 0;
			for(auto i:m_BuffMap)												//找到持续时间最短的buff的ID
			{
				if (!dration)
				{
					dration = i.second->getBuffDuration();
				}else{
					if (dration > i.second->getBuffDuration())
					{
						dration = i.second->getBuffDuration();
						bufid = i.second->getBuffID();
					}
				}
			}
			removeBuf(bufid);	
			CCLOG("buffmap.size > 8 remove dration min bufID : %d ",bufid);
		}
		return false;	
	}
	//边界值处理，扣的属性值最低只能为0，增量最高有上限？,buf处理类型分发函数
	void BuffManage::ExcuteBuff(BuffInfo*bfinfo, bool handel /*= true*/)
	{
		bfinfo->setExcute(false);
		if (bfinfo->getHandle()==handel)		//处理过的再次处理的或没处理过的移除处理的
			return;
		switch ((sBuffType)bfinfo->getBuffType())
		{
		case sBuffType::eClearDebuff:
			{
				if (!bfinfo->getHandle()&&handel)
				{
					bfinfo->setHandle(true);
					clearDbuf();
				}			
			}break;
		case sBuffType::eCurrBlood:
		case sBuffType::eFiring:
		case sBuffType::eBleed:
			{
				if (mRole->getMonsterSpecies() == sMonsterSpecies::eWorldBoss)
					return;			//世界boss免疫buff效果
				int tNumber = bufCurrHpHandle(bfinfo,handel);							/*1当前血量*/		
				if (!tNumber)break;
				NOTIFICATION->postNotification(B_UpdateBuffEffect,mRole);				//每次加减血时显示一次buff的特效
				if (bfinfo->getIsDBuff())
				{			
					mRole->getRoleObject()->playerNum(PlayHpType::generalType,tNumber);
				}else{
					mRole->getRoleObject()->playerNum(PlayHpType::gainType,tNumber);
				}
			}break;
		case sBuffType::eAttack:{
			bufAtkHandle(bfinfo,handel);							/*2、攻击*/
						   }break;
		case sBuffType::eDefense:{
			bufDefHandle(bfinfo,handel);							/*3、防御*/
						   }break;
		case sBuffType::eCrit:{
			bufCritHandle(bfinfo,handel);							/*4、暴击*/
						   }break;
		case sBuffType::eDodge:{
			bufDogeHandle(bfinfo,handel);							/*5、闪避*/
							}break;
		case sBuffType::eHit:{
			bufHitHandle(bfinfo,handel);							/*6、命中*/
						   }break;	
		case sBuffType::eMoveSpeed:{
			bufMSpeedHandle(bfinfo,handel);							/*7、移动速度*/
								 }break;
		case sBuffType::eAttackSpeed:{
			bufAtkSpeedHandle(bfinfo,handel);						/*8、攻击速度*/
								}break;
		case sBuffType::eUnMove:									/*100、定身*/
			{
				if (handel)
				{
					mRole->setMove(false);
					bfinfo->setHandle(true);
				}else{
					mRole->setMove(true);
				}
			}break;
		case sBuffType::eHurt:{
			bufHrtHandle(bfinfo,handel);							/*1000、伤害*/		
						   }break;
		case sBuffType::eMaxBlood:{
			bufMaxHpHandle(bfinfo,handel);							/*1001、最大血量*/
							 }break;
		default:
			CCLOG("[*ERROR]BuffManage::ExcuteBuff UnExist BUFF Type=%d",bfinfo->getBuffType());
			break;
		}
	}
	/*1、当前血量*/
	int  BuffManage::bufCurrHpHandle(BuffInfo* bfinfo,bool handel)
	{
		if (!handel)return 0;
		int changeHPNum = 0;
		if (bfinfo->getChangeNum())//血量按数值变化
		{
			CCLOG("aliveID = %d,afterCURRHP = %d",mRole->getAliveID(),mRole->getHp());
			if (bfinfo->getIsDBuff())
			{
				mRole->setHp(mRole->getHp() - bfinfo->getChangeNum());

			}else{
				mRole->setHp(mRole->getHp() + bfinfo->getChangeNum());
			}
			changeHPNum = bfinfo->getChangeNum();
			CCLOG("bufType CURRHP= %d,changeNum = %d,aliveHP=%d",sBuffType::eCurrBlood,bfinfo->getChangeNum(),mRole->getHp());
		}
		if (bfinfo->getPrecent())//影响武将血量百分比
		{
			CCLOG("aliveID = %d,afterCURRHP = %d",mRole->getAliveID(),mRole->getHp());
			changeHPNum = bfinfo->getPrecent() * 0.01f * mRole->getHp();
			if (bfinfo->getIsDBuff())
			{
				mRole->setHp(mRole->getHp() - changeHPNum);			//扣血和加血统一显示一次即可
			}else{
				mRole->setHp(mRole->getHp() + changeHPNum);
			}
			CCLOG("bufType CURRHP= %d,changePe = %d,aliveHP=%d",sBuffType::eCurrBlood,bfinfo->getPrecent(),mRole->getHp());
		}
		if (!handel)CCLOG("/------------------------Remove----------------------------/");
		CCLOG("buffId = %d,buffType=%d,changeNum=%d,changePe=%d",
			bfinfo->getBuffID(),bfinfo->getBuffType(),bfinfo->getChangeNum(),bfinfo->getPrecent());
		CCLOG("/----------------------------------------------------------/");
		return changeHPNum;
	}
	/*2、攻击*/
	void BuffManage::bufAtkHandle(BuffInfo* bfinfo,bool handel)
	{
		if (bfinfo->getChangeNum())		//影响武将攻击数值
		{
			CCLOG("aliveID = %d,afterAtk = %d",mRole->getAliveID(),mRole->getAtk());
			if (bfinfo->getIsDBuff() == handel)
			{
				mRole->setAtk(mRole->getAtk() - bfinfo->getChangeNum());
			}else{
				mRole->setAtk(mRole->getAtk() + bfinfo->getChangeNum());
			}
			CCLOG("bufType ATK= %d,changeNum = %d,aliveAtk = %d",sBuffType::eAttack,bfinfo->getChangeNum(),mRole->getAtk());
		}
		if (bfinfo->getPrecent())		//影响武将攻击百分比
		{
			CCLOG("aliveID = %d,afterAtk = %d",mRole->getAliveID(),mRole->getAtk());
			float atkNum =  mRole->getBaseData()->getRoleAttack();
			float num = bfinfo->getPrecent() * 0.01f * atkNum;
			if (bfinfo->getIsDBuff() == handel)
			{
				mRole->setAtk(mRole->getAtk() - num);
			}else{
				mRole->setAtk(mRole->getAtk() + num);
			}
			CCLOG("bufType ATK= %d,changePe = %d,aliveAtk = %d",sBuffType::eAttack,bfinfo->getPrecent(),mRole->getAtk());
		}
		if (!handel)CCLOG("//------------------------Remove---------------------------");
		CCLOG("Id = %d,Type= %d,changeNum= %d,changePe= %d",
			bfinfo->getBuffID(),bfinfo->getBuffType(),bfinfo->getChangeNum(),bfinfo->getPrecent());
		if (handel)
			CCLOG("/----------------------------------------------------------/");
		else
			CCLOG("----------------------------------------------------------//");
		bfinfo->setHandle(true);//设置buf为处理过
	}
	/*3、防御*/
	void BuffManage::bufDefHandle(BuffInfo* bfinfo,bool handel)
	{
		if (bfinfo->getChangeNum())//影响武将防御数值
		{
			CCLOG("aliveID = %d,afterDEF = %d",mRole->getAliveID(),mRole->getDef());
			if (bfinfo->getIsDBuff() == handel)
			{
				mRole->setDef(mRole->getDef() - bfinfo->getChangeNum());
			}else{
				mRole->setDef(mRole->getDef() + bfinfo->getChangeNum());
			}
			CCLOG("bufType DEF= %d,changeNum = %d,aliveDEF=%d",sBuffType::eDefense,bfinfo->getChangeNum(),mRole->getDef());
		}
		if (bfinfo->getPrecent())//影响武将防御百分比
		{
			CCLOG("aliveID = %d,afterDEF = %d",mRole->getAliveID(),mRole->getDef());
			float defNum = mRole->getBaseData()->getRoleDefense();
			float num = bfinfo->getPrecent() * 0.01f * defNum;
			if (bfinfo->getIsDBuff() == handel)
			{
				mRole->setDef(mRole->getDef() - num);
			}else{
				mRole->setDef(mRole->getDef() + num);
			}
			CCLOG("bufType DEF= %d,changePe = %d,aliveDEF=%d",sBuffType::eDefense,bfinfo->getPrecent(),mRole->getDef());
		}
		if (!handel)CCLOG("//------------------------Remove----------------------------");
		CCLOG("Id = %d,Type= %d,changeNum= %d,changePe= %d",
			bfinfo->getBuffID(),bfinfo->getBuffType(),bfinfo->getChangeNum(),bfinfo->getPrecent());
		if (handel)
			CCLOG("/----------------------------------------------------------/");
		else
			CCLOG("----------------------------------------------------------//");
		bfinfo->setHandle(true);
	}
	/*4、暴击*/
	void BuffManage::bufCritHandle(BuffInfo* bfinfo,bool handel)
	{
		if (bfinfo->getChangeNum())//影响武将暴击数值
		{
			CCLOG("aliveID = %d,afterCRI = %d",mRole->getAliveID(),mRole->getCrit());
			if (bfinfo->getIsDBuff() == handel)
			{
				mRole->setCrit(mRole->getCrit() - bfinfo->getChangeNum());
			}else{
				mRole->setCrit(mRole->getCrit() + bfinfo->getChangeNum());
			}
			CCLOG("bufType CRI= %d,changeNum = %d,aliveCRI=%d",sBuffType::eCrit,bfinfo->getChangeNum(),mRole->getCrit());
		}
		if (bfinfo->getPrecent())//影响武将暴击百分比
		{
			CCLOG("aliveID = %d,afterCRI = %d",mRole->getAliveID(),mRole->getCrit());
			float critNum = mRole->getBaseData()->getRoleCrit();
			float num = bfinfo->getPrecent() * 0.01f * critNum;
			if (bfinfo->getIsDBuff() == handel)
			{
				mRole->setCrit(mRole->getCrit() - num);
			}else{
				mRole->setCrit(mRole->getCrit() + num);
			}
			CCLOG("bufType CRI= %d,changePe = %d,aliveCRI=%d",sBuffType::eCrit,bfinfo->getPrecent(),mRole->getCrit());
		}
		if (!handel)CCLOG("//------------------------Remove----------------------------");
		CCLOG("Id = %d,Type= %d,changeNum= %d,changePe= %d",
			bfinfo->getBuffID(),bfinfo->getBuffType(),bfinfo->getChangeNum(),bfinfo->getPrecent());
		if (handel)
			CCLOG("/----------------------------------------------------------/");
		else
			CCLOG("----------------------------------------------------------//");
		bfinfo->setHandle(true);
	}
	/*5、闪避*/
	void BuffManage::bufDogeHandle( BuffInfo* bfinfo,bool handel )
	{
		if (bfinfo->getChangeNum())//影响武将暴击数值
		{
			CCLOG("aliveID = %d,afterDoge = %d",mRole->getAliveID(),mRole->getDoge());
			if (bfinfo->getIsDBuff() == handel)
			{
				mRole->setDoge(mRole->getDoge() - bfinfo->getChangeNum());
			}else{
				mRole->setDoge(mRole->getDoge() + bfinfo->getChangeNum());
			}
			CCLOG("bufType Doge= %d,changeNum = %d,aliveDoge=%d",sBuffType::eDodge,bfinfo->getChangeNum(),mRole->getDoge());
		}
		if (bfinfo->getPrecent())//影响武将暴击百分比
		{
			CCLOG("aliveID = %d,afterDoge = %d",mRole->getAliveID(),mRole->getDoge());
			float critNum = mRole->getBaseData()->getRoleDodge();
			float num = bfinfo->getPrecent() * 0.01f * critNum;
			if (bfinfo->getIsDBuff() == handel)
			{
				mRole->setDoge(mRole->getDoge() - num);
			}else{
				mRole->setDoge(mRole->getDoge() + num);
			}
			CCLOG("bufType Doge= %d,changePe = %d,aliveDoge=%d",sBuffType::eDodge,bfinfo->getPrecent(),mRole->getDoge());
		}
		if (!handel)CCLOG("//------------------------Remove----------------------------");
		CCLOG("Id = %d,Type= %d,changeNum= %d,changePe= %d",
			bfinfo->getBuffID(),bfinfo->getBuffType(),bfinfo->getChangeNum(),bfinfo->getPrecent());
		if (handel)
			CCLOG("/----------------------------------------------------------/");
		else
			CCLOG("----------------------------------------------------------//");
		bfinfo->setHandle(true);
	}
	/*6、命中*/
	void BuffManage::bufHitHandle(BuffInfo* bfinfo,bool handel)
	{
		if (bfinfo->getChangeNum())//影响武将命中数值
		{
			CCLOG("aliveID = %d,beforeHIT = %d",mRole->getAliveID(),mRole->getHit());
			if (bfinfo->getIsDBuff() == handel)
			{
				mRole->setHit(mRole->getHit() - bfinfo->getChangeNum());
			}else{
				mRole->setHit(mRole->getHit() + bfinfo->getChangeNum());
			}
			CCLOG("bufType HIT= %d,changeNum = %d,aliveHit=%d",sBuffType::eHit,bfinfo->getChangeNum(),mRole->getHit());
		}
		if (bfinfo->getPrecent())//影响武将命中百分比
		{
			CCLOG("aliveID = %d,beforeHIT = %d",mRole->getAliveID(),mRole->getHit());
			float hitNum = mRole->getBaseData()->getRoleHit();
			float num = bfinfo->getPrecent() * 0.01f * hitNum;
			if (bfinfo->getIsDBuff() == handel)
			{
				mRole->setHit(mRole->getHit() - num);
			}else{
				mRole->setHit(mRole->getHit() + num);
			}
			CCLOG("bufType HIT= %d,changePe = %d,aliveHit=%d",sBuffType::eHit,bfinfo->getPrecent(),mRole->getHit());
		}
		if (!handel)CCLOG("//------------------------Remove----------------------------");
		CCLOG("Id = %d,Type= %d,changeNum= %d,changePe= %d",
			bfinfo->getBuffID(),bfinfo->getBuffType(),bfinfo->getChangeNum(),bfinfo->getPrecent());
		if (handel)
			CCLOG("/----------------------------------------------------------/");
		else
			CCLOG("----------------------------------------------------------//");
		bfinfo->setHandle(true);//设置buf为处理过
	}
	/*7、移动速度 移动时间越长速度越慢*/
	void BuffManage::bufMSpeedHandle( BuffInfo* bfinfo,bool handel )
	{
		if (bfinfo->getChangeNum())//影响武将移动速度数值    
		{
			CCLOG("aliveID = %d,before MSpeed = %f",mRole->getAliveID(),mRole->getMoveSpeed());
			if (bfinfo->getIsDBuff() == handel)
			{
				mRole->setMoveSpeed(mRole->getMoveSpeed() + bfinfo->getChangeNum());
			}else{
				float speed = mRole->getMoveSpeed() - bfinfo->getChangeNum();
				if (speed <= 0.2f)
					speed = 0.2f;				//武将移动最快速度极限值处理
				mRole->setMoveSpeed(speed);
			}
			CCLOG("bufType MSpeed= %d,changeNum = %d,alive MSpeed=%f",sBuffType::eMoveSpeed,bfinfo->getChangeNum(),mRole->getMoveSpeed());
		}
		if (bfinfo->getPrecent())//影响武将移动速度百分比
		{
			CCLOG("aliveID = %d,before MSpeed = %f",mRole->getAliveID(),mRole->getMoveSpeed());
			float speed =mRole->getBaseData()->getMoveSpeed();					//s/格
			float num = bfinfo->getPrecent() * 0.01f;
			if (bfinfo->getIsDBuff() == handel)
			{
				float cSpeed = speed/(speed/mRole->getMoveSpeed()-num);
				if (cSpeed < 0.2f)
					cSpeed = 0.2f;
				mRole->setMoveSpeed(cSpeed);
			}else{
				float cSpeed = speed/(speed/mRole->getMoveSpeed()+num);
				if (cSpeed < 0.2f)
					cSpeed = 0.2f;
				mRole->setMoveSpeed(cSpeed);
			}
			CCLOG("bufType MSpeed= %d,changePe = %d,alive MSpeed=%d",sBuffType::eMoveSpeed,bfinfo->getPrecent(),mRole->getMoveSpeed());
		}
		if (!handel)CCLOG("//------------------------Remove----------------------------");
		CCLOG("Id = %d,Type= %d,changeNum= %d,changePe= %d",
			bfinfo->getBuffID(),bfinfo->getBuffType(),bfinfo->getChangeNum(),bfinfo->getPrecent());
		if (handel)
			CCLOG("/----------------------------------------------------------/");
		else
			CCLOG("----------------------------------------------------------//");
		bfinfo->setHandle(true);//设置buf为处理过
	}
	/*8、攻击速度*/
	void BuffManage::bufAtkSpeedHandle( BuffInfo* bfinfo,bool handel )
	{
		//传过来的数值和百分比都是对敏捷造成的影响
		//攻速 = 2 - (敏捷*0.2/400)
		float CurrAgility = ( 2-mRole->getAtkInterval() )*400/0.2f;			//当前敏捷值
		if (bfinfo->getChangeNum())//影响武将攻击速度数值
		{
			CCLOG("aliveID = %d,before AtkSpeed = %f",mRole->getAliveID(),mRole->getAtkInterval());
			if (bfinfo->getIsDBuff() == handel)
			{
				CurrAgility -= bfinfo->getChangeNum();
			}else{
				CurrAgility += bfinfo->getChangeNum();
			}
			mRole->setAtkInterval(2-(CurrAgility*0.2f/400));
			CCLOG("bufType AtkSpeed= %d,changeNum = %d,alive AtkSpeed=%f",sBuffType::eMoveSpeed,bfinfo->getChangeNum(),mRole->getAtkInterval());
		}
		if (bfinfo->getPrecent())//影响武将移动速度百分比
		{
			CCLOG("aliveID = %d,before AtkSpeed = %f",mRole->getAliveID(),mRole->getAtkInterval());
			float BeingAgility = ( 2-mRole->getBaseData()->getAttackSpeed() )*400/0.2f;		//增减的敏捷值
			if (bfinfo->getIsDBuff() == handel)
			{
				CurrAgility -= bfinfo->getPrecent() *0.01f* BeingAgility;
			}else{
				CurrAgility += bfinfo->getPrecent() *0.01f* BeingAgility;
			}
			mRole->setAtkInterval(2-(CurrAgility*0.2f/400));
			CCLOG("bufType AtkSpeed= %d,changePe = %d,alive AtkSpeed=%f",sBuffType::eMoveSpeed,bfinfo->getPrecent(),mRole->getAtkInterval());
		}
		if (!handel)CCLOG("//------------------------Remove----------------------------");
		CCLOG("Id = %d,Type= %d,changeNum= %d,changePe= %d",
			bfinfo->getBuffID(),bfinfo->getBuffType(),bfinfo->getChangeNum(),bfinfo->getPrecent());
		if (handel)
			CCLOG("/----------------------------------------------------------/");
		else
			CCLOG("----------------------------------------------------------//");
		bfinfo->setHandle(true);//设置buf为处理过
	}
	/*1000、伤害*/
	void BuffManage::bufHrtHandle(BuffInfo* bfinfo,bool handel)
	{
		//伤害的计算公式应该是一个固定的计算方法传入攻击对象和受击对象然后得出伤害,流血可以通过这个实现，伤害的体现又是在扣血体现出来的。
		if (bfinfo->getChangeNum())
		{
			CCLOG("bufType is HRT,beforeHurt=%d",mRole->getHrt());
			if (bfinfo->getIsDBuff() == handel)
			{
				mRole->setHrt(mRole->getHrt() - bfinfo->getChangeNum());	//在通过伤害来计算血量的时候伤害加上这个值
			}else{
				mRole->setHrt(mRole->getHrt() + bfinfo->getChangeNum());
			}
			CCLOG("bufType HRT= %d,changeNum = %d,afterHurt=%d",sBuffType::eHurt,bfinfo->getChangeNum(),mRole->getHrt());
		}
		if (bfinfo->getPrecent())
		{
			//在受到伤害的基础上影响武将的伤害百分比
			CCLOG("bufType is HRT,beforeHurtpe=%d",mRole->getHrtpe());
			if ( bfinfo->getIsDBuff() == handel)
			{
				mRole->setHrtpe(mRole->getHrtpe() - bfinfo->getPrecent());	//在通过伤害来计算血量的时候伤害加上这个值
			}else{
				mRole->setHrtpe(mRole->getHrtpe() + bfinfo->getPrecent());
			}
			CCLOG("bufType HRT= %d,changePE = %d,AfterHurtPe=%d",sBuffType::eHurt,bfinfo->getPrecent(),mRole->getHrtpe());
		}
		bfinfo->setHandle(true);//设置buf为处理过
	}
	/*1001、改变MAX血量*/
	int BuffManage::bufMaxHpHandle(BuffInfo* bfinfo,bool handel)
	{
		int changeHPNum = 0;
		if (bfinfo->getChangeNum())//血量按数值变化
		{
			CCLOG("aliveID = %d,afterHPMAX = %d",mRole->getAliveID(),mRole->getMaxHp());
			if (bfinfo->getIsDBuff() == handel)
			{
				mRole->setMaxHp(mRole->getMaxHp() - bfinfo->getChangeNum());
			}else{
				mRole->setMaxHp(mRole->getMaxHp() + bfinfo->getChangeNum());
			}
			changeHPNum = bfinfo->getChangeNum();
			CCLOG("bufType HPMAX= %d,changeNum = %d,aliveMaxHp=%d",sBuffType::eMaxBlood,bfinfo->getChangeNum(),mRole->getMaxHp());
		}
		if (bfinfo->getPrecent())//影响武将血量百分比
		{
			CCLOG("aliveID = %d,afterHPMAX = %d",mRole->getAliveID(),mRole->getMaxHp());
			changeHPNum = bfinfo->getPrecent() * 0.01f * mRole->getBaseData()->getRoleHp();
			if (bfinfo->getIsDBuff() == handel)
			{
				mRole->setMaxHp(mRole->getMaxHp() - changeHPNum);			//扣血和加血统一显示一次即可
			}else{
				mRole->setMaxHp(mRole->getMaxHp() + changeHPNum);
			}
			CCLOG("bufType HPMAX= %d,changePe = %d,aliveHP=%d",sBuffType::eMaxBlood,bfinfo->getPrecent(),mRole->getMaxHp());
		}
		bfinfo->setHandle(true);
		return changeHPNum;
	}
}