#ifndef REWARD_GM_H_
#define REWARD_GM_H_
#include "DataDefine.h"

struct RewardData
{
	int id;
	string name;
	string icon;
	string icon1;
	string level;
};


class RewardDataGM
{
public:
	typedef map<int,RewardData*> RewardCSV;
	RewardDataGM();
	~RewardDataGM();
	const RewardData* getRewardCfg(int id);	
	void parse();
	void parseStory();
	const char* getStoryStr(int id);	
	int getStoryLength();
protected:
	RewardCSV m_cfg;    
	vector<string> m_storyVec;
};
#endif // !SKILL_GM_H_
