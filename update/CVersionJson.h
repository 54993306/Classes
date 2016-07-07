#pragma once

/******************************************************
*文件名称:	CVersionJson.h
*编写日期:	2016-6-14-11:52
*编写作者:	YPF
*功能描述:	游戏版本的json配置信息
*******************************************************/

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