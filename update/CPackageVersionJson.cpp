#include "CPackageVersionJson.h"


CPackageVersionJson::CPackageVersionJson()
{

}

const CPackageVersionData& CPackageVersionJson::getDataById( std::string sVersion )
{
	for(PackageVersionDataList::iterator iterator=_data.begin(); iterator!=_data.end(); iterator++)
	{
		const CPackageVersionData& sData = *iterator;
		if(sData.sVersion.compare(sVersion) == 0)
		{
			return sData;
		}
	}
	return _data[0];
	CCLOG("CPackageVersionData& CPackageVersionJson::getDataById");
}

void CPackageVersionJson::reloadFile(std::string sFile)
{
	//清理之前的
	_data.clear();

	//路径
	sFile = CCFileUtils::sharedFileUtils()->fullPathForFilename(sFile.c_str());
	if (!CCFileUtils::sharedFileUtils()->isFileExist(sFile))
	{
		CCLOG("[ ERROR ] CPackageVersionJson::reloadFile_isFileExist");
		return;
	}
	unsigned long size = 0;
	unsigned char *buff = CCFileUtils::sharedFileUtils()->getFileData(sFile.c_str(),"r",&size);
	string contentStr ((const char*)buff, size);
	rapidjson::Document doc;
	doc.Parse<0>(contentStr.c_str());

	if (doc.HasParseError())
	{    
		CCLOG("[ ERROR ] CPackageVersionJson::reloadFile_doc.HasParseError()");
		return;
	}   
	else if (doc.IsObject() && doc.HasMember("package_version_info"))
	{    
		const rapidjson::Value &array =doc["package_version_info"];

		if (array.IsArray())
		{    
			for(unsigned int i=0; i<array.Size(); ++i)
			{  
				const rapidjson::Value &single = array[i];

				CPackageVersionData data;

				if(single.HasMember("version"))data.sVersion = single["version"].GetString();

				if(single.HasMember("zip_count"))data.iZipNum= single["zip_count"].GetInt();

				if(single.HasMember("total_size"))data.iSize = single["total_size"].GetInt();

				_data.push_back(data);
			}
		}
	}
}

CPackageVersionData CPackageVersionJson::getAimPackageVersion( std::string sApkVersion )
{
	CPackageVersionData sVersionTemp ={"", 0, 1};
	int iSize = _data.size();
	for(unsigned int i=0; i<iSize; i++)
	{
		const CPackageVersionData& indexData = _data.at(i);
		
		//找到一个小于等于apk版本号的package版本
		if(sApkVersion.compare(indexData.sVersion)>=0)
		{
			//找到的第一个版本
			if(sVersionTemp.sVersion.compare("")==0)
			{
				sVersionTemp = indexData;
			}
			//之前已经找到了版本，比较新版本大小
			else
			{
				if(sVersionTemp.sVersion.compare(indexData.sVersion)<0)
				{
					sVersionTemp = indexData;
				}
			}
		}
	}

	return sVersionTemp;
}
