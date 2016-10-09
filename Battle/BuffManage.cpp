
#include "Battle/BuffManage.h"
#include "Battle/BaseRole.h"
#include "Battle/RoleObject/HPObject.h"
#include "Battle/BaseRole.h"
#include "Battle/RoleObject/RoleObject.h"
#include "Battle/ConstNum.h"
#include "Battle/BattleMessage.h"
#include "Battle/BaseRoleData.h"
#include "Battle/skEffectData.h"
#include "Battle/BuffData.h"
#include "Battle/RoleObjectMacro.h"
#include "Battle/Buff/BattleBuff.h"

using namespace cocos2d::extension;

namespace BattleSpace
{
#define BUFFMAX (8)
	BuffManage::BuffManage():mRole(nullptr)
	{}

	BuffManage::~BuffManage()
	{
		for (auto buf:mBuffMap)
		{
			CC_SAFE_RELEASE(buf.second);
		}
		mBuffMap.clear();
		for (auto p : m_EffectMap)
			p.second.clear();
		m_EffectMap.clear();
		mRole = nullptr;
	}

	bool BuffManage::init()
	{
		return true;
	}

	void BuffManage::Buffclear()
	{
		vector<int> Vec;
		for (auto i:mBuffMap)
			Vec.push_back(i.first);
		for (auto j:Vec)
			removeBuf(j);
		mBuffMap.clear();
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
		if ( AddBuffLogic(pData) )
			return;
		BattleBuff* tBuff = BattleBuff::CreateBuff(pData,mRole);
		tBuff->retain();									
		mBuffMap[tBuff->getBuffID()] = tBuff;
		CCLOG("[ Tips ] BuffManage::AddBuff succeed bufID = %d",pData->getBuffID());
		bNotification->postNotification(MsgAddBuff,mRole);
	}

	BattleBuff* BuffManage::getbuff(int bufID)
	{
		BuffMap::iterator iter = mBuffMap.find(bufID);
		if (iter != mBuffMap.end())
			return iter->second;
		return nullptr;
	}

	BattleBuff* BuffManage::getbuffbyType(int buftype)
	{
		for (auto i : mBuffMap)
		{
			if (i.second->getBuffType() == (sBuffType)buftype)
				return i.second;
		}
		return nullptr;
	}

	void BuffManage::removeBuf(int id)
	{
		BuffMap::iterator iter = mBuffMap.find(id);
		if (iter == mBuffMap.end())								//按ket值和具体内容来进行索引的
			return;
		BuffEffectMapList::iterator piter = m_EffectMap.find(id);
		if (piter != m_EffectMap.end())
		{
			for (auto i:piter->second)
			{
				i->removeFromParentAndCleanup(true);			//移除buff相应的效果
				i = nullptr;
			}				
			m_EffectMap.erase(piter);							//逻辑移除通知显示，显示同步移除图标
			bNotification->postNotification(B_RemoveBuff,mRole);//刷新剩余图标信息(显示移除的操作，放在一起处理)
		}else{
			CCLOG("[ ERROR ] BuffManage::removeBuf BuffEffect Vector is NULL");
		}
		CC_SAFE_RELEASE(iter->second);
		iter->second = nullptr;
		mBuffMap.erase(iter);
	}
	//接收一个消息进清理，或是被其他地方进行调用进行的清理
	void BuffManage::clearDbuf()
	{
		vector<int> DBuff;
		for (auto i:mBuffMap)
		{
			if (i.second->getDBuff())
				DBuff.push_back(i.first);
		}
		for (auto j:DBuff)
			removeBuf(j);
	}

	void BuffManage::upDateBuff( float dt )
	{
		vector<int> DBuff;
		for (auto tPair : mBuffMap)
		{
			tPair.second->updateBuff(dt);
			if (tPair.second->isRemove())
				DBuff.push_back(tPair.first);
		}
		for (auto tBuffID : DBuff)
			removeBuf(tBuffID);
	}

	//返回true表示不添加新Buf
	bool BuffManage::AddBuffLogic(const BuffData* pData)
	{
		if ( !pData->isProperty(mRole->getBaseData()->getProperty()) )
			return true;
		BattleBuff* buff = getbuff(pData->getBuffID());
		if (buff)
		{
			buff->setBF_Duration(pData->getBuffDuration());
			return true;
		}

		BattleBuff* oldBuff = getbuffbyType(pData->getBuffType());
		if (oldBuff)															/*存在相同类型的buf*/
		{
			if (oldBuff->getDBuff() == pData->getIsDBuff())								/*同为增益或减益*/
			{
				if(oldBuff->getBF_Level() > pData->getBuffLevel())
					return true;
				CCLOG(" [oldBuff_Level <= newBuf_Level] oldBufid:%d,newBufid:%d",oldBuff->getBuffID(),pData->getBuffID());
				removeBuf(oldBuff->getBuffID());									/*高级替换低级*/
			}else{
				CCLOG("newBUFF=%d Replace oldBUFF=%d",pData->getBuffID(),oldBuff->getBuffID());									
				removeBuf(oldBuff->getBuffID());									/*增减益互相替换*/
			}
		}else{
			maxBuffJudge();
		}
		return false;	
	}

	void BuffManage::maxBuffJudge()
	{
		if (mBuffMap.size() < BUFFMAX)									
			return ;
		int bufid = 0;														/*超过8个类型的buf存在时移除持续时间最短的buf*/
		float dration = 0;
		for(auto i:mBuffMap)												//找到持续时间最短的buff的ID
		{
			if (dration)
			{
				if (dration > i.second->getBF_Duration())
				{
					dration = i.second->getBF_Duration();
					bufid = i.second->getBuffID();
				}
			}else{
				dration = i.second->getBF_Duration();
			}
		}
		removeBuf(bufid);	
		CCLOG("buffmap.size > 8 remove dration min bufID : %d ",bufid);
	}
}