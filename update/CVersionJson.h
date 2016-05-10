#pragma once

#include "cocos-ext.h"
#include "cocos2d.h"

USING_NS_CC;
USING_NS_CC_EXT;

using namespace std;

struct CVersionData
{
	string sVersion;
	string sFile;
	int		iSize;
};

typedef vector<CVersionData> VersionDataList;	

class CVersionJson
{
public:
	CVersionJson();
	void reloadFile(std::string sFile);
	const CVersionData& getDataById(std::string sVersion);
	vector<CVersionData> getUpdateList(std::string sCurrentVersion, std::string sAimVersion);
private:
	VersionDataList _data;
};