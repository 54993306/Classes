#include "CPlayerSkillData.h"


CPlayerSkillData::CPlayerSkillData()
{

}

const CFrameActionData& CPlayerSkillData::getDataById( int iId )
{
	for(CFrameActionDataList::iterator iterator=_data.begin(); iterator!=_data.end(); iterator++)
	{
		const CFrameActionData& frameActionData = *iterator;
		if(frameActionData.iModelId == iId)
		{
			return frameActionData;
		}
	}
	CCLOG("[ *ERROR ] CPlayerSkillData::getDataById Lost %d",iId);
	return _data[0];
	
}

void CPlayerSkillData::reloadFile()
{
	//清理之前的
	_data.clear();
	//路径
	string strPath = CCFileUtils::sharedFileUtils()->fullPathForFilename("csv/playerSkillData.json");//"csv/playerSkillData.json";
	if (!CCFileUtils::sharedFileUtils()->isFileExist(strPath))
	{
		CCLOG("[ ERROR ] CPlayerSkillData::reloadFile_isFileExist");
		return;
	}
	unsigned long size = 0;
	unsigned char *buff = CCFileUtils::sharedFileUtils()->getFileData(strPath.c_str(),"r",&size);
	string contentStr ((const char*)buff, size);
	rapidjson::Document doc;
	doc.Parse<0>(contentStr.c_str());

	if (doc.HasParseError())
	{    
		CCLOG("[ ERROR ] CPlayerSkillData::reloadFile_doc.HasParseError()");
		return;
	}   
	else if (doc.IsObject() && doc.HasMember("PlayerSkillData"))
	{    
		const rapidjson::Value &array =doc["PlayerSkillData"];

		if (array.IsArray())
		{    
			for(unsigned int i=0; i<array.Size(); ++i)
			{  
				const rapidjson::Value &single = array[i];

				CFrameActionData data;

				if(single.HasMember("model_id"))data.iModelId = single["model_id"].GetInt();

				if(single.HasMember("opacity1"))data.iOpacity[0] = single["opacity1"].GetInt();

				if(single.HasMember("opacity2"))data.iOpacity[1] = single["opacity2"].GetInt();

				if(single.HasMember("pos1_x"))data.pos[0].x= single["pos1_x"].GetInt();

				if(single.HasMember("pos1_y"))data.pos[0].y = single["pos1_y"].GetInt();

				if(single.HasMember("pos2_x"))data.pos[1].x = single["pos2_x"].GetInt();

				if(single.HasMember("pos2_y"))data.pos[1].y = single["pos2_y"].GetInt();

				if(single.HasMember("scale"))data.fScale = single["scale"].GetDouble();

				if(single.HasMember("flipX"))data.bFlipX = single["flipX"].GetBool();

				if(single.HasMember("iRoate"))data.iRoate = single["iRoate"].GetInt();

				if(single.HasMember("time"))data.fTime = single["time"].GetDouble();

				if(single.HasMember("head_pic"))data.sHeadPic = single["head_pic"].GetString();

				if(single.HasMember("text_pic"))data.sTextPic = single["text_pic"].GetString();

				_data.push_back(data);
			}
		}
	}
}