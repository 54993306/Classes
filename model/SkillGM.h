#ifndef SKILL_GM_H_
#define SKILL_GM_H_
#include "DataDefine.h"
class SkillGM
{
public:
	typedef map<int,SkillCfg*> SkillCSV;
	typedef map<int,SkillEffectCfg*> EffectCsv;
	SkillGM();
	~SkillGM();
	const SkillCfg* getCfg(int id);
	const SkillEffectCfg* getEffect(int id);

protected:
	void parse();
protected:
	SkillCSV m_cfg;
	EffectCsv m_effect;
};
#endif // !SKILL_GM_H_
