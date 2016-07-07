#include "MultLanguage.h"
#include "CocoStudio/json/rapidjson/rapidjson.h"

CMultLanguage* CMultLanguage::m_instance = nullptr;

CMultLanguage* CMultLanguage::getInstance()
{
	if (!m_instance)
	{
		m_instance = new CMultLanguage();
	}
	return m_instance;
}

CMultLanguage::CMultLanguage()
	:m_langType(1),m_sDefault("")
{

}

void CMultLanguage::parseFromJson(const char* strFile)
{
	//路径
	std::string sFile = CCFileUtils::sharedFileUtils()->fullPathForFilename(strFile);
	if (!CCFileUtils::sharedFileUtils()->isFileExist(sFile))
	{
		CCLOG("[ ERROR ] CMultLanguage::parseFromJson_file missing");
		return;
	}
	unsigned long size = 0;
	unsigned char *buff = CCFileUtils::sharedFileUtils()->getFileData(sFile.c_str(),"r",&size);
	string data((const char*)buff,size); 

    rapidjson::Document doc;
	doc.Parse<0>(data.c_str());

	if (doc.HasParseError())
	{
		CCLOG("doc parse happen error");
		return;
	}

	if (doc.IsArray())
	{
		for (rapidjson::SizeType i= 0; i<doc.Size();++i)
		{
			rapidjson::Value& val = doc[i];
			
			rapidjson::Value& strId = val["Id"];

			if (val.HasMember("chineseSim"))
			{
				rapidjson::Value& sim = val["chineseSim"];
				m_chineseSimMap[strId.GetInt()] = sim.GetString();
				if (sim.GetString()!="null")
				{
					m_chineseSimMap[strId.GetInt()] = sim.GetString();
				}
				else
				{
					m_chineseSimMap[strId.GetInt()] = "";
				}
			}
			if (val.HasMember("chineseCom"))
			{
				rapidjson::Value& com = val["chineseCom"];
				if (com.GetString()!="null")
				{
					m_chineseComMap[strId.GetInt()] = com.GetString();
				}
				else
				{
					m_chineseComMap[strId.GetInt()] = "";
				}
			}
			if (val.HasMember("english"))
			{
				rapidjson::Value& eng = val["english"];
				if (eng.GetString()!="null")
				{
					m_englishMap[strId.GetInt()] = eng.GetString();
				}
				else
				{
					m_englishMap[strId.GetInt()] = "";
				}
			}

			if (val.HasMember("Thailand"))
			{
				rapidjson::Value& eng = val["Thailand"];
				if (eng.GetString()!="null")
				{
					m_thailandMap[strId.GetInt()] = eng.GetString();
				}
				else
				{
					m_thailandMap[strId.GetInt()] = "";
				}
			}
		}
	}
}

const std::string& CMultLanguage::getString(int strId)
{
	if (strId == 1090)
	{
		int i =0;
	}
	if (m_langType == 1)
	{
		map<int, string>::iterator iter = m_chineseComMap.find(strId);
		if (iter!=m_chineseComMap.end())
		{
			return iter->second;
		}
	}
	else if (m_langType == 2)
	{
		map<int, string>::iterator iter = m_chineseSimMap.find(strId);
		if (iter!=m_chineseSimMap.end())
		{
			return iter->second;
		}
	}
	else if (m_langType == 3)
	{
		map<int, string>::iterator iter = m_englishMap.find(strId);
		if (iter!=m_englishMap.end())
		{
			return iter->second;
		}
	}
	else if (m_langType == 4)
	{
		map<int, string>::iterator iter = m_thailandMap.find(strId);
		if (iter!=m_thailandMap.end())
		{
			return iter->second;
		}
	}
	return m_sDefault;
}

void CMultLanguage::setLangType(int type)
{
	m_langType = type;
}

int CMultLanguage::getLangType()
{
	return m_langType;
}