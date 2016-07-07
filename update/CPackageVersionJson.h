#pragma once

/******************************************************
*文件名称:	CPackageVersionJson.h
*编写日期:	2016-6-14-11:51
*编写作者:	YPF
*功能描述:	拆分包的json配置信息
*******************************************************/

#include "cocos-ext.h"
#include "cocos2d.h"

USING_NS_CC;
USING_NS_CC_EXT;

using namespace std;

struct CPackageVersionData
{
	string	sVersion;
	int		iZipNum;
	int		iSize;
};

typedef vector<CPackageVersionData> PackageVersionDataList;	

class CPackageVersionJson
{
public:
	CPackageVersionJson();
	void reloadFile(std::string sFile);
	const CPackageVersionData& getDataById(std::string sVersion);
	CPackageVersionData getAimPackageVersion(std::string sApkVersion);

private:
	PackageVersionDataList _data;
};