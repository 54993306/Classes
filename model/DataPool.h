#ifndef DATA_POOL_H_
#define DATA_POOL_H_
#include "DataDefine.h"
#include "tollgate/Chapter.h"

template<typename K, typename V> 
class TMap
{
public:	
	typedef map<K,V> KMap;
	V* getElementById(const K& k);
	void   addElement(const K& k, const V& v);
private:
	//KMap::iterator iter;
	KMap kmap;
};

class CDataPool
{
public:
	CDataPool();
	~CDataPool();
	CStageInfoRes* getStageInfoById(int id);
	void setStageInfoById(int id, const CStageInfoRes& res);
protected:
	
private:
	typedef TMap<int,CStageInfoRes> StageInfoMap;
	StageInfoMap m_stageInfoMap;

};

#endif // !SKILL_GM_H_
