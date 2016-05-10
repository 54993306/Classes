#include "BuffManage.h"
#include "Global.h"
#include "GMessage.h"
#include "DataDefine.h"
#include "DataCenter.h"
#include "scene/alive/AliveDefine.h"
#include "warscene/ConstNum.h"

using namespace cocos2d;
using namespace cocos2d::extension;
#define BUFFMAX (8)

BuffInfo::BuffInfo()
	:m_Id(0)				//bufID
	,m_Type(0)				//buf类型
	,m_Duration(0)			//持续回合数
	,m_ChangeNum(0)			//buf影响数值
	,m_Precent(0)			//影响百分比
	,m_Rate(0)				//触发概率(必定添加但是不一定触发，不触发则不显示移除出buf管理器)
	,m_Dbuf(false)			//buf是否为减益buf
	,m_bfname("")			//bf名称
	,m_Target(0)			//buf添加的目标
	,m_Level(0)				//buf等级
	,m_Excute(false)		//执行buff
	,m_Handle(false)		//默认未处理buf
	,m_ExTime(0)
	,m_AddFirst(true)
{}

void BuffInfo::setExTime(float var)
{
	m_ExTime += var;
	if (m_ExTime < C_BUFFTIME)
		return;
	m_ExTime = 0;
	m_Excute = true;
}
float BuffInfo::getExTime(){return m_ExTime;}

BuffManage::BuffManage():m_alive(nullptr),m_BufeffectID(0)
{}

BuffManage::~BuffManage()
{
	Buffclear();
	m_alive = nullptr;
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

void BuffManage::AddBuff(CBuff& buf)
{
	if (m_alive->getHp()<=0||!m_alive->getBattle()||!m_alive->getActObject())
	{
		CCLOG("[ TIPS ] BuffManage::AddBuff Add Faild");
		return;											//统一的做安全性判断处理
	}
	BuffInfo* buff = getbuff(buf.buffId);
	if (buff)
	{
		buff->setDuration(buf.duration);
		return;
	}
	if (AddBuffLogic(buf))
		return;
	BuffInfo* buffinfo = BuffInfo::create();
	buffinfo->setID(buf.buffId);
	buffinfo->setType(buf.buffType);
	buffinfo->setDuration(buf.duration);
	buffinfo->setChangeNum(buf.damage);
	buffinfo->setPrecent(buf.damage_rate);
	buffinfo->setRate(buf.useRate);
	buffinfo->setDbuf(buf.debuf);
	buffinfo->setbfname(buf.name);
	buffinfo->setTarget(buf.pTarget);
	buffinfo->setLevel(buf.level);
	buffinfo->retain();									//clear方法里面safa release的作用就是在这了
	ExcuteBuff(buffinfo);								//执行buff逻辑
	m_BuffMap[buffinfo->getID()] = buffinfo;			//map添加信息的方法	
	if (m_alive->getHp()<=0)
		m_alive->getActObject()->AliveDie();
	CCLOG("[ Tips ] BuffManage::AddBuff succeed bufID = %d",buf.buffId);
	NOTIFICATION->postNotification(ADDBUFF,m_alive);
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
		if (i.second->getType() == buftype)
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
			NOTIFICATION->postNotification(REMOVEBUFF,m_alive);//刷新剩余图标信息
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
		if (i.second->getDbuf())
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
		if (info->getDuration()<dt)				//持续时间已经达到,标记移除buff
		{
			info->setDuration(0);
			RemoveFirst.push_back(iter.first);
			continue;
		}
		info->setDuration(info->getDuration() - dt);
		info->setExTime(dt);
		if (info->getExcute())
			ExcuteBuff(info);					//执行buff逻辑
	}
	for (auto i:RemoveFirst)					//执行移除操作
		removeBuf(i);
	if (m_alive->getHp()<=0)
		m_alive->getActObject()->AliveDie();
}

//返回true表示不添加新Buf
bool BuffManage::AddBuffLogic(CBuff& buff)
{
	BuffInfo* oldBuff = getbuffbyType(buff.buffType);
	if (oldBuff)															/*存在相同类型的buf*/
	{
		if (oldBuff->getDbuf() == buff.debuf)								/*同为增益或减益*/
		{
			if(oldBuff->getLevel() > buff.level)
				return true;
			CCLOG(" [oldBuff_Level <= newBuf_Level] oldBufid:%d,newBufid:%d",oldBuff->getID(),buff.buffId);
			removeBuf(oldBuff->getID());									/*高级替换低级*/
		}else{
			CCLOG("newBUFF=%d Replace oldBUFF=%d",buff.buffId,oldBuff->getID());									
			removeBuf(oldBuff->getID());									/*增减益互相替换*/
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
				dration = i.second->getDuration();
			}else{
				if (dration > i.second->getDuration())
				{
					dration = i.second->getDuration();
					bufid = i.second->getID();
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
	switch (bfinfo->getType())
	{
	case CLEBF:
		{
			if (!bfinfo->getHandle()&&handel)
			{
				bfinfo->setHandle(true);
				clearDbuf();
			}			
		}break;
	case CURRHP:
	case FIRING:
	case BLEED:
		{
			if (m_alive->getAliveType() == AliveType::WorldBoss)
				return;			//世界boss免疫buff效果
			int num = bufCurrHpHandle(bfinfo,handel);							/*1当前血量*/		
			if (!num)break;
			NOTIFICATION->postNotification(UPBUFFEFFECT,m_alive);				//每次加减血时显示一次buff的特效
			if (bfinfo->getDbuf())
			{			
				m_alive->getActObject()->getHp()->playerNum(m_alive->getActObject(),num,generalType);
			}else{
				m_alive->getActObject()->getHp()->playerNum(m_alive->getActObject(),num,gainType);
			}
		}break;
	case ATK:{
		bufAtkHandle(bfinfo,handel);							/*2、攻击*/
			 }break;
	case DEF:{
		bufDefHandle(bfinfo,handel);							/*3、防御*/
			 }break;
	case CRI:{
		bufCritHandle(bfinfo,handel);							/*4、暴击*/
			 }break;
	case DOGE:{
		bufDogeHandle(bfinfo,handel);							/*5、闪避*/
			  }break;
	case HIT:{
		bufHitHandle(bfinfo,handel);							/*6、命中*/
			 }break;	
	case MOVESPEED:{
		bufMSpeedHandle(bfinfo,handel);							/*7、移动速度*/
				   }break;
	case ATKSPEED:{
		bufAtkSpeedHandle(bfinfo,handel);						/*8、攻击速度*/
				  }break;
	case CONTROL_ONE:											/*100、定身*/
		{
			if (handel)
			{
				m_alive->setMove(false);
				bfinfo->setHandle(true);
			}else{
				m_alive->setMove(true);
			}
		}break;
	case HRT:{
		bufHrtHandle(bfinfo,handel);							/*1000、伤害*/		
			 }break;
	case HPMAX:{
		bufMaxHpHandle(bfinfo,handel);							/*1001、最大血量*/
			   }break;
	default:
		CCLOG("[*ERROR]BuffManage::ExcuteBuff UnExist BUFF Type=%d",bfinfo->getType());
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
		CCLOG("aliveID = %d,afterCURRHP = %d",m_alive->getAliveID(),m_alive->getHp());
		if (bfinfo->getDbuf())
		{
			m_alive->setHp(m_alive->getHp() - bfinfo->getChangeNum());
			
		}else{
			m_alive->setHp(m_alive->getHp() + bfinfo->getChangeNum());
		}
		changeHPNum = bfinfo->getChangeNum();
		CCLOG("bufType CURRHP= %d,changeNum = %d,aliveHP=%d",CURRHP,bfinfo->getChangeNum(),m_alive->getHp());
	}
	if (bfinfo->getPrecent())//影响武将血量百分比
	{
		CCLOG("aliveID = %d,afterCURRHP = %d",m_alive->getAliveID(),m_alive->getHp());
		changeHPNum = bfinfo->getPrecent() * 0.01f * m_alive->getHp();
		if (bfinfo->getDbuf())
		{
			m_alive->setHp(m_alive->getHp() - changeHPNum);			//扣血和加血统一显示一次即可
		}else{
			m_alive->setHp(m_alive->getHp() + changeHPNum);
		}
		CCLOG("bufType CURRHP= %d,changePe = %d,aliveHP=%d",CURRHP,bfinfo->getPrecent(),m_alive->getHp());
	}
	if (!handel)CCLOG("/------------------------Remove----------------------------/");
	CCLOG("buffId = %d,buffType=%d,changeNum=%d,changePe=%d",
		bfinfo->getID(),bfinfo->getType(),bfinfo->getChangeNum(),bfinfo->getPrecent());
	CCLOG("/----------------------------------------------------------/");
	return changeHPNum;
}
/*2、攻击*/
void BuffManage::bufAtkHandle(BuffInfo* bfinfo,bool handel)
{
	if (bfinfo->getChangeNum())		//影响武将攻击数值
	{
		CCLOG("aliveID = %d,afterAtk = %d",m_alive->getAliveID(),m_alive->getAtk());
		if (bfinfo->getDbuf() == handel)
		{
			m_alive->setAtk(m_alive->getAtk() - bfinfo->getChangeNum());
		}else{
			m_alive->setAtk(m_alive->getAtk() + bfinfo->getChangeNum());
		}
		CCLOG("bufType ATK= %d,changeNum = %d,aliveAtk = %d",ATK,bfinfo->getChangeNum(),m_alive->getAtk());
	}
	if (bfinfo->getPrecent())		//影响武将攻击百分比
	{
		CCLOG("aliveID = %d,afterAtk = %d",m_alive->getAliveID(),m_alive->getAtk());
		float atkNum =  m_alive->role->atk;
		float num = bfinfo->getPrecent() * 0.01f * atkNum;
		if (bfinfo->getDbuf() == handel)
		{
			m_alive->setAtk(m_alive->getAtk() - num);
		}else{
			m_alive->setAtk(m_alive->getAtk() + num);
		}
		CCLOG("bufType ATK= %d,changePe = %d,aliveAtk = %d",ATK,bfinfo->getPrecent(),m_alive->getAtk());
	}
	if (!handel)CCLOG("//------------------------Remove---------------------------");
	CCLOG("Id = %d,Type= %d,changeNum= %d,changePe= %d",
		bfinfo->getID(),bfinfo->getType(),bfinfo->getChangeNum(),bfinfo->getPrecent());
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
		CCLOG("aliveID = %d,afterDEF = %d",m_alive->getAliveID(),m_alive->getDef());
		if (bfinfo->getDbuf() == handel)
		{
			m_alive->setDef(m_alive->getDef() - bfinfo->getChangeNum());
		}else{
			m_alive->setDef(m_alive->getDef() + bfinfo->getChangeNum());
		}
		CCLOG("bufType DEF= %d,changeNum = %d,aliveDEF=%d",DEF,bfinfo->getChangeNum(),m_alive->getDef());
	}
	if (bfinfo->getPrecent())//影响武将防御百分比
	{
		CCLOG("aliveID = %d,afterDEF = %d",m_alive->getAliveID(),m_alive->getDef());
		float defNum = m_alive->role->def;
		float num = bfinfo->getPrecent() * 0.01f * defNum;
		if (bfinfo->getDbuf() == handel)
		{
			m_alive->setDef(m_alive->getDef() - num);
		}else{
			m_alive->setDef(m_alive->getDef() + num);
		}
		CCLOG("bufType DEF= %d,changePe = %d,aliveDEF=%d",DEF,bfinfo->getPrecent(),m_alive->getDef());
	}
	if (!handel)CCLOG("//------------------------Remove----------------------------");
	CCLOG("Id = %d,Type= %d,changeNum= %d,changePe= %d",
		bfinfo->getID(),bfinfo->getType(),bfinfo->getChangeNum(),bfinfo->getPrecent());
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
		CCLOG("aliveID = %d,afterCRI = %d",m_alive->getAliveID(),m_alive->getCrit());
		if (bfinfo->getDbuf() == handel)
		{
			m_alive->setCrit(m_alive->getCrit() - bfinfo->getChangeNum());
		}else{
			m_alive->setCrit(m_alive->getCrit() + bfinfo->getChangeNum());
		}
		CCLOG("bufType CRI= %d,changeNum = %d,aliveCRI=%d",CRI,bfinfo->getChangeNum(),m_alive->getCrit());
	}
	if (bfinfo->getPrecent())//影响武将暴击百分比
	{
		CCLOG("aliveID = %d,afterCRI = %d",m_alive->getAliveID(),m_alive->getCrit());
		float critNum = m_alive->role->crit;
		float num = bfinfo->getPrecent() * 0.01f * critNum;
		if (bfinfo->getDbuf() == handel)
		{
			m_alive->setCrit(m_alive->getCrit() - num);
		}else{
			m_alive->setCrit(m_alive->getCrit() + num);
		}
		CCLOG("bufType CRI= %d,changePe = %d,aliveCRI=%d",CRI,bfinfo->getPrecent(),m_alive->getCrit());
	}
	if (!handel)CCLOG("//------------------------Remove----------------------------");
	CCLOG("Id = %d,Type= %d,changeNum= %d,changePe= %d",
		bfinfo->getID(),bfinfo->getType(),bfinfo->getChangeNum(),bfinfo->getPrecent());
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
		CCLOG("aliveID = %d,afterDoge = %d",m_alive->getAliveID(),m_alive->getDoge());
		if (bfinfo->getDbuf() == handel)
		{
			m_alive->setDoge(m_alive->getDoge() - bfinfo->getChangeNum());
		}else{
			m_alive->setDoge(m_alive->getDoge() + bfinfo->getChangeNum());
		}
		CCLOG("bufType Doge= %d,changeNum = %d,aliveDoge=%d",DOGE,bfinfo->getChangeNum(),m_alive->getDoge());
	}
	if (bfinfo->getPrecent())//影响武将暴击百分比
	{
		CCLOG("aliveID = %d,afterDoge = %d",m_alive->getAliveID(),m_alive->getDoge());
		float critNum = m_alive->role->dodge;
		float num = bfinfo->getPrecent() * 0.01f * critNum;
		if (bfinfo->getDbuf() == handel)
		{
			m_alive->setDoge(m_alive->getDoge() - num);
		}else{
			m_alive->setDoge(m_alive->getDoge() + num);
		}
		CCLOG("bufType Doge= %d,changePe = %d,aliveDoge=%d",DOGE,bfinfo->getPrecent(),m_alive->getDoge());
	}
	if (!handel)CCLOG("//------------------------Remove----------------------------");
	CCLOG("Id = %d,Type= %d,changeNum= %d,changePe= %d",
		bfinfo->getID(),bfinfo->getType(),bfinfo->getChangeNum(),bfinfo->getPrecent());
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
		CCLOG("aliveID = %d,beforeHIT = %d",m_alive->getAliveID(),m_alive->getHit());
		if (bfinfo->getDbuf() == handel)
		{
			m_alive->setHit(m_alive->getHit() - bfinfo->getChangeNum());
		}else{
			m_alive->setHit(m_alive->getHit() + bfinfo->getChangeNum());
		}
		CCLOG("bufType HIT= %d,changeNum = %d,aliveHit=%d",HIT,bfinfo->getChangeNum(),m_alive->getHit());
	}
	if (bfinfo->getPrecent())//影响武将命中百分比
	{
		CCLOG("aliveID = %d,beforeHIT = %d",m_alive->getAliveID(),m_alive->getHit());
		float hitNum = m_alive->role->hit;
		float num = bfinfo->getPrecent() * 0.01f * hitNum;
		if (bfinfo->getDbuf() == handel)
		{
			m_alive->setHit(m_alive->getHit() - num);
		}else{
			m_alive->setHit(m_alive->getHit() + num);
		}
		CCLOG("bufType HIT= %d,changePe = %d,aliveHit=%d",HIT,bfinfo->getPrecent(),m_alive->getHit());
	}
	if (!handel)CCLOG("//------------------------Remove----------------------------");
	CCLOG("Id = %d,Type= %d,changeNum= %d,changePe= %d",
		bfinfo->getID(),bfinfo->getType(),bfinfo->getChangeNum(),bfinfo->getPrecent());
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
		CCLOG("aliveID = %d,before MSpeed = %f",m_alive->getAliveID(),m_alive->getMSpeed());
		if (bfinfo->getDbuf() == handel)
		{
			m_alive->setMSpeed(m_alive->getMSpeed() + bfinfo->getChangeNum());
		}else{
			float speed = m_alive->getMSpeed() - bfinfo->getChangeNum();
			if (speed <= 0.2f)
				speed = 0.2f;				//武将移动最快速度极限值处理
			m_alive->setMSpeed(speed);
		}
		CCLOG("bufType MSpeed= %d,changeNum = %d,alive MSpeed=%f",MOVESPEED,bfinfo->getChangeNum(),m_alive->getMSpeed());
	}
	if (bfinfo->getPrecent())//影响武将移动速度百分比
	{
		CCLOG("aliveID = %d,before MSpeed = %f",m_alive->getAliveID(),m_alive->getMSpeed());
		float speed =m_alive->role->MoveSpeed;					//s/格
		float num = bfinfo->getPrecent() * 0.01f;
		if (bfinfo->getDbuf() == handel)
		{
			float cSpeed = speed/(speed/m_alive->getMSpeed()-num);
			if (cSpeed < 0.2f)
				cSpeed = 0.2f;
			m_alive->setMSpeed(cSpeed);
		}else{
			float cSpeed = speed/(speed/m_alive->getMSpeed()+num);
			if (cSpeed < 0.2f)
				cSpeed = 0.2f;
			m_alive->setMSpeed(cSpeed);
		}
		CCLOG("bufType MSpeed= %d,changePe = %d,alive MSpeed=%d",MOVESPEED,bfinfo->getPrecent(),m_alive->getMSpeed());
	}
	if (!handel)CCLOG("//------------------------Remove----------------------------");
	CCLOG("Id = %d,Type= %d,changeNum= %d,changePe= %d",
		bfinfo->getID(),bfinfo->getType(),bfinfo->getChangeNum(),bfinfo->getPrecent());
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
	float CurrAgility = ( 2-m_alive->getAtkInterval() )*400/0.2f;			//当前敏捷值
	if (bfinfo->getChangeNum())//影响武将攻击速度数值
	{
		CCLOG("aliveID = %d,before AtkSpeed = %f",m_alive->getAliveID(),m_alive->getAtkInterval());
		if (bfinfo->getDbuf() == handel)
		{
			CurrAgility -= bfinfo->getChangeNum();
		}else{
			CurrAgility += bfinfo->getChangeNum();
		}
		m_alive->setAtkInterval(2-(CurrAgility*0.2f/400));
		CCLOG("bufType AtkSpeed= %d,changeNum = %d,alive AtkSpeed=%f",MOVESPEED,bfinfo->getChangeNum(),m_alive->getAtkInterval());
	}
	if (bfinfo->getPrecent())//影响武将移动速度百分比
	{
		CCLOG("aliveID = %d,before AtkSpeed = %f",m_alive->getAliveID(),m_alive->getAtkInterval());
		float BeingAgility = ( 2-m_alive->role->atkInterval )*400/0.2f;		//增减的敏捷值
		if (bfinfo->getDbuf() == handel)
		{
			CurrAgility -= bfinfo->getPrecent() *0.01f* BeingAgility;
		}else{
			CurrAgility += bfinfo->getPrecent() *0.01f* BeingAgility;
		}
		m_alive->setAtkInterval(2-(CurrAgility*0.2f/400));
		CCLOG("bufType AtkSpeed= %d,changePe = %d,alive AtkSpeed=%f",MOVESPEED,bfinfo->getPrecent(),m_alive->getAtkInterval());
	}
	if (!handel)CCLOG("//------------------------Remove----------------------------");
	CCLOG("Id = %d,Type= %d,changeNum= %d,changePe= %d",
		bfinfo->getID(),bfinfo->getType(),bfinfo->getChangeNum(),bfinfo->getPrecent());
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
		CCLOG("bufType is HRT,beforeHurt=%d",m_alive->getHrt());
		if (bfinfo->getDbuf() == handel)
		{
			m_alive->setHrt(m_alive->getHrt() - bfinfo->getChangeNum());	//在通过伤害来计算血量的时候伤害加上这个值
		}else{
			m_alive->setHrt(m_alive->getHrt() + bfinfo->getChangeNum());
		}
		CCLOG("bufType HRT= %d,changeNum = %d,afterHurt=%d",HRT,bfinfo->getChangeNum(),m_alive->getHrt());
	}
	if (bfinfo->getPrecent())
	{
		//在受到伤害的基础上影响武将的伤害百分比
		CCLOG("bufType is HRT,beforeHurtpe=%d",m_alive->getHrtpe());
		if ( bfinfo->getDbuf() == handel)
		{
			m_alive->setHrtpe(m_alive->getHrtpe() - bfinfo->getPrecent());	//在通过伤害来计算血量的时候伤害加上这个值
		}else{
			m_alive->setHrtpe(m_alive->getHrtpe() + bfinfo->getPrecent());
		}
		CCLOG("bufType HRT= %d,changePE = %d,AfterHurtPe=%d",HRT,bfinfo->getPrecent(),m_alive->getHrtpe());
	}
	bfinfo->setHandle(true);//设置buf为处理过
}
/*1001、改变MAX血量*/
int BuffManage::bufMaxHpHandle(BuffInfo* bfinfo,bool handel)
{
	int changeHPNum = 0;
	if (bfinfo->getChangeNum())//血量按数值变化
	{
		CCLOG("aliveID = %d,afterHPMAX = %d",m_alive->getAliveID(),m_alive->getMaxHp());
		if (bfinfo->getDbuf() == handel)
		{
			m_alive->setMaxHp(m_alive->getMaxHp() - bfinfo->getChangeNum());
		}else{
			m_alive->setMaxHp(m_alive->getMaxHp() + bfinfo->getChangeNum());
		}
		changeHPNum = bfinfo->getChangeNum();
		CCLOG("bufType HPMAX= %d,changeNum = %d,aliveMaxHp=%d",HPMAX,bfinfo->getChangeNum(),m_alive->getMaxHp());
	}
	if (bfinfo->getPrecent())//影响武将血量百分比
	{
		CCLOG("aliveID = %d,afterHPMAX = %d",m_alive->getAliveID(),m_alive->getMaxHp());
		changeHPNum = bfinfo->getPrecent() * 0.01f * m_alive->role->hp;
		if (bfinfo->getDbuf() == handel)
		{
			m_alive->setMaxHp(m_alive->getMaxHp() - changeHPNum);			//扣血和加血统一显示一次即可
		}else{
			m_alive->setMaxHp(m_alive->getMaxHp() + changeHPNum);
		}
		CCLOG("bufType HPMAX= %d,changePe = %d,aliveHP=%d",HPMAX,bfinfo->getPrecent(),m_alive->getMaxHp());
	}
	bfinfo->setHandle(true);
	return changeHPNum;
}