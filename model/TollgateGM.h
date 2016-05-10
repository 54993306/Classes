#ifndef TOLLGATEGM_H_
#define TOLLGATEGM_H_
#include "DataDefine.h"
class TollgateGM
{
public:
	TollgateGM();
	~TollgateGM();
    TollgateCfg* getCfg(int id);
	const vector<TollgateCfg *>& getCfgList();
	inline void setCurrentID(int id){m_currID = id;}
	inline int getCurrentID(){return m_currID;}
	void addTollgate(TollgateCfg *tollgate);//添加关卡
	void clearTollgate();
protected:
	void parse();
protected:
	//map<int,TollgateCfg*> m_list;
	vector<TollgateCfg *> m_list;
	int m_currID;//当前选中关卡ID
};
#endif // !TOLLGATEGM_H_
