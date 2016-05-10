#ifndef ROLENAME_GM_H_
#define ROLENAME_GM_H_
#include "DataDefine.h"



class RoleNameGM
{
public:
	RoleNameGM();
	~RoleNameGM();
	void parseRoleName();
	string getRandowName();
protected:
	vector<string> m_surNameVec;
	vector<string> m_seconNameVec;
	vector<string> m_thirdNameVec;
 };
#endif // !SKILL_GM_H_
