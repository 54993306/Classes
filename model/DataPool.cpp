#include "DataPool.h"
#include "Global.h"

template<typename K, typename V> 
V* TMap<K,V>::getElementById(const K& k)
{
	typename KMap::iterator iter = kmap.find(k);
	if (iter!= kmap.end())
	{
		return &iter->second;
	}
	return nullptr;
}

template<typename K, typename V> 
void  TMap<K,V>::addElement(const K& k, const V& v)
{
	typename KMap::iterator iter = kmap.find(k);
	if (iter == kmap.end())
	{
		kmap[k] = v;
	}
}


CDataPool::CDataPool()
{
}
CDataPool::~CDataPool()
{
	
}

CStageInfoRes* CDataPool::getStageInfoById(int id)
{
	return m_stageInfoMap.getElementById(id);
}

void CDataPool::setStageInfoById(int id, const CStageInfoRes& res)
{
	m_stageInfoMap.addElement(id,res);
}



