#include "CHeroBodyShowData.h"


CHeroBodyShowData::CHeroBodyShowData()
{

}

const CHeroBodyShowInfo& CHeroBodyShowData::getDataById( int iId )
{
	for(CHeroBodyShowDataList::iterator iterator=_data.begin(); iterator!=_data.end(); iterator++)
	{
		const CHeroBodyShowInfo& sData = *iterator;
		if(sData.iModelId == iId)
		{
			return sData;
		}
	}
	return _data[0];
	CCLOG("CHeroBodyShowInfo& CHeroBodyShowData::getDataById");
}

void CHeroBodyShowData::reloadFile()
{
	//清理之前的
	_data.clear();
	//路径
	string strPath = CCFileUtils::sharedFileUtils()->fullPathForFilename("csv/heroBodyShowData.json");
	if (!CCFileUtils::sharedFileUtils()->isFileExist(strPath))
	{
		CCLOG("[ ERROR ] CHeroBodyShowData::reloadFile_isFileExist");
		return;
	}
	unsigned long size = 0;
	unsigned char *buff = CCFileUtils::sharedFileUtils()->getFileData(strPath.c_str(),"r",&size);
	string contentStr ((const char*)buff, size);
	rapidjson::Document doc;
	doc.Parse<0>(contentStr.c_str());

	if (doc.HasParseError())
	{    
		CCLOG("[ ERROR ] CHeroBodyShowData::reloadFile_doc.HasParseError()");
		return;
	}   
	else if (doc.IsObject() && doc.HasMember("heroBodyShowData"))
	{    
		const rapidjson::Value &array =doc["heroBodyShowData"];

		if (array.IsArray())
		{    
			for(unsigned int i=0; i<array.Size(); ++i)
			{  
				const rapidjson::Value &single = array[i];

				CHeroBodyShowInfo data;

				if(single.HasMember("model_id"))data.iModelId = single["model_id"].GetInt();

				if(single.HasMember("pos1_x"))data.pos.x= single["pos1_x"].GetInt();

				if(single.HasMember("pos1_y"))data.pos.y = single["pos1_y"].GetInt();

				if(single.HasMember("scale"))data.fScale = single["scale"].GetDouble();

				if(single.HasMember("flipX"))data.bFlipX = single["flipX"].GetBool();

				if(single.HasMember("iRoate"))data.iRoate = single["iRoate"].GetInt();

				if(single.HasMember("head_pic"))data.sPic = single["head_pic"].GetString();

				_data.push_back(data);
			}
		}
	}
}