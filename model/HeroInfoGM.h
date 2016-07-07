#ifndef _HEROINFOGM_
#define _HEROINFOGM_
#include "DataDefine.h"

struct HeroInfoData
{
	int heroId;
	string heroName;
	string heroDesc;
	int iType1;	//神将，僵尸，道士
	int iType2;//近战，远战，坦克，辅助
	int iType3;//天煞僵尸，地煞僵尸
	string heroStory;	//英雄故事
};


class HeroInfoGM
{
public:
	typedef map<int,HeroInfoData*> HeroInfoCSV;
	HeroInfoGM();
	~HeroInfoGM();
	const HeroInfoData* getCfg(int id);
	void parse();
protected:
	HeroInfoCSV m_cfg;    
};
#endif // !SKILL_GM_H_
