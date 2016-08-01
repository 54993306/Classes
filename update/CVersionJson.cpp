#include "CVersionJson.h"


CVersionJson::CVersionJson()
{

}

const CVersionData& CVersionJson::getDataById( std::string sVersion )
{
	for(VersionDataList::iterator iterator=_data.begin(); iterator!=_data.end(); iterator++)
	{
		const CVersionData& sData = *iterator;
		if(sData.sVersion.compare(sVersion) == 0)
		{
			return sData;
		}
	}
	return _data[0];
	CCLOG("CVersionData& CVersionJson::getDataById");
}

void CVersionJson::reloadFile(std::string sFile)
{
	//清理之前的
	_data.clear();

	//路径
	sFile = CCFileUtils::sharedFileUtils()->fullPathForFilename(sFile.c_str());
	if (!CCFileUtils::sharedFileUtils()->isFileExist(sFile))
	{
		CCLOG("[ ERROR ] CVersionJson::reloadFile_isFileExist");
		return;
	}
	unsigned long size = 0;
	unsigned char *buff = CCFileUtils::sharedFileUtils()->getFileData(sFile.c_str(),"r",&size);
	string contentStr ((const char*)buff, size);
	rapidjson::Document doc;
	doc.Parse<0>(contentStr.c_str());

	if (doc.HasParseError())
	{    
		CCLOG("[ ERROR ] CVersionJson::reloadFile_doc.HasParseError()");
		return;
	}   
	else if (doc.IsObject() && doc.HasMember("version_info"))
	{    
		const rapidjson::Value &array =doc["version_info"];

		if (array.IsArray())
		{    
			for(unsigned int i=0; i<array.Size(); ++i)
			{  
				const rapidjson::Value &single = array[i];

				CVersionData data;

				if(single.HasMember("version"))data.sVersion = single["version"].GetString();

				if(single.HasMember("update_file"))data.sFile= single["update_file"].GetString();

				if(single.HasMember("update_size"))data.iSize = single["update_size"].GetInt();

				_data.push_back(data);
			}
		}
	}
}

vector<CVersionData> CVersionJson::getUpdateList( std::string sCurrentVersion, std::string sAimVersion )
{
	vector<CVersionData> dataBack;
	int iSize = _data.size();
	for(unsigned int i=0; i<iSize; i++)
	{
		const CVersionData& indexData = _data.at(i);
		if(sCurrentVersion.compare(indexData.sVersion)>=0)
		{
			continue;
		}
		if(sAimVersion.compare(indexData.sVersion)>=0)
		{
			dataBack.push_back(indexData);
		}
		else
		{
			break;
		}
	}

	return dataBack;
}

bool CVersionJson::isThereAForceApkUpdateVersion( std::string sCurrentVersion, std::string sAimVersion )
{
	bool bAnswer = false;

	int iSize = _data.size();
	for( int i=iSize-1; i>=0; i--)
	{
		const CVersionData& indexData = _data.at(i);
		if(sCurrentVersion.compare(indexData.sVersion)>=0)
		{
			continue;
		}
		if(sAimVersion.compare(indexData.sVersion)>=0)
		{
			if(indexData.sFile.size()<=1)
			{
				return true;
			}
		}
		else
		{
			break;
		}
	}

	return bAnswer;
}
