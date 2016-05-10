#include "RoleNameGM.h"
#include "tools/ToolDefine.h"
#include "Global.h"


RoleNameGM::RoleNameGM()
{
	parseRoleName();
}

RoleNameGM::~RoleNameGM()
{
	
}

void RoleNameGM::parseRoleName()
{
	CSVFile* file = (CSVFile*)FileUtils::sharedFileUtils()->loadCSVFile(CSV_ROOT("name.csv"));
	if(!file)
	{
		return;
	}
	int row = file->getRowNum();
	for(int i = 1; i < row; ++i)
	{
		m_surNameVec.push_back(file->get(i,0));
		m_seconNameVec.push_back(file->get(i,1));
		m_thirdNameVec.push_back(file->get(i,2));
	}
	FileUtils::sharedFileUtils()->releaseFile(CSV_ROOT("name.csv"));
}

std::string RoleNameGM::getRandowName()
{
	std::string name;
	int fir = CCRANDOM_0_1() * (m_surNameVec.size()-1);
	int sec = CCRANDOM_0_1() * (m_seconNameVec.size()-1);
	int third = CCRANDOM_0_1() * (m_thirdNameVec.size()-1);

	name.append(m_surNameVec.at(fir));
	name.append(m_seconNameVec.at(sec));
	name.append(m_thirdNameVec.at(third));
	return name;
}
