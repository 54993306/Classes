#ifndef SHARE_GM_H_
#define SHARE_GM_H_
#include "DataDefine.h"

struct ShareData
{
	int id;
	string desc;
	string url;
};


class ShareGM
{
public:
	typedef map<int,ShareData*> ShareMap;
	ShareGM();
	~ShareGM();
	const ShareData* getCfg(int id);
	void parseData();
protected:
	ShareMap m_dataMap;     //进阶任务
};
#endif // !SKILL_GM_H_
