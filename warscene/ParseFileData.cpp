#include "ParseFileData.h"
#include "tools/ToolDefine.h"
#include "model/DataCenter.h"
#include "model/WarManager.h"
#include "ConstNum.h"
//解析可移动格子文件
void ParseMoveGrid( int stageid,vector<int> & vec )
{
	unsigned long size = 0;
	unsigned char *buff = CCFileUtils::sharedFileUtils()->getFileData("csv/MoveGrids.json","r",&size);
	string data ((const char*)buff,size);
	rapidjson::Document doc;
	doc.Parse<0>(data.c_str());
	if ( doc.HasParseError())
	{
		CCLOG("[ *ERROR ] ParseFileData::ParseMoveGrid %s",doc.GetParseError());
		return;
	}
	if ( !doc.IsObject() || !doc.HasMember("MoveGrids") )
	{
		CCLOG("[ *ERROR ] ParseFileData::ParseMoveGrid Create Json Project Fail");
		return;
	}
	rapidjson::Value& Vec = doc["MoveGrids"];		//得到键对应的数组信息
	if (!Vec.IsArray())								//判断是否为数组
	{
		CCLOG("[ *ERROR ]  ParseFileData::ParseMoveGrid Can Not't Find  COSTGRIDS");
		return ;
	}
	vec.clear();
	for (rapidjson::SizeType i = 0;i<Vec.Size();i++)
	{
		rapidjson::Value& json_step = Vec[i];
		if (json_step.HasMember("id"))
		{
			rapidjson::Value & var = json_step["id"];
			if (var.GetInt() != stageid)				//只存储当前关卡的可移动范围
				continue;
			if (json_step.HasMember("grid"))
			{
				rapidjson::Value & par = json_step["grid"];
				for (int j=par.GetInt();j< C_CAPTAINGRID;j++)
					vec.push_back(j);
			}
			if (json_step.HasMember("grids"))
			{
				rapidjson::Value& pVec = json_step["grids"];		//得到键对应的数组信息
				if (!pVec.IsArray())								//判断是否为数组
				{
					CCLOG("[ *ERROR ] ParseFileData::ParseMoveGrid Can Not't Find grids Vec");
					continue;
				}
				for (rapidjson::SizeType j = 0;j<pVec.Size();j++)
				{
					rapidjson::Value& json_step = pVec[j];
					vec.push_back(json_step.GetInt());
				}
			}
			break;													//只存储当前关卡的可移动范围
		}else{
			CCLOG("[ *ERROR ] ParseFileData::ParseMoveGrid Lost stage id");
			continue;
		}
	}
	if (!vec.size())
	{
		for (int j=C_BEGINGRID;j< C_CAPTAINGRID;j++)				//默认情况处理(都可以移动)
			vec.push_back(j);
	}
}
//解析增加cost格子文件
void ParseAddCostGrid( int stageid,vector<int> & vec )
{
	unsigned long size = 0;
	unsigned char *buff = CCFileUtils::sharedFileUtils()->getFileData("csv/CostGrids.json","r",&size);
	string data ((const char*)buff,size);
	rapidjson::Document doc;
	doc.Parse<0>(data.c_str());
	if ( doc.HasParseError())
	{
		CCLOG("[ *ERROR ] WarMapLayer::initGrids %s",doc.GetParseError());
		return;
	}
	if ( !doc.IsObject() || !doc.HasMember("COSTGRIDS") )
	{
		CCLOG("[ *ERROR ] WarMapLayer::initGrids Create Json Project Fail");
		return;
	}
	rapidjson::Value& Vec = doc["COSTGRIDS"];		//得到键对应的数组信息
	if (!Vec.IsArray())								//判断是否为数组
	{
		CCLOG("[ *ERROR ]  WarMapLayer::initGrids Can Not't Find  COSTGRIDS");
		return ;
	}
	vec.clear();
	for (rapidjson::SizeType i = 0;i<Vec.Size();i++)
	{
		rapidjson::Value& json_step = Vec[i];
		if (json_step.HasMember("id"))
		{
			rapidjson::Value & var = json_step["id"];
			if (var.GetInt() != stageid)
				continue;
			if (json_step.HasMember("grid"))
			{
				rapidjson::Value & par = json_step["grid"];
				for (int j=par.GetInt();j< C_GRID_ROW * C_GRID_COL;j++)
					vec.push_back(j);
			}
			if (json_step.HasMember("grids"))
			{
				rapidjson::Value& pVec = json_step["grids"];		//得到键对应的数组信息
				if (!pVec.IsArray())								//判断是否为数组
				{
					CCLOG("[ *ERROR ]  WarMapLayer::initGrids Can Not't Find grids Vec");
					continue; ;
				}
				for (rapidjson::SizeType j = 0;j<pVec.Size();j++)
				{
					rapidjson::Value& json_step = pVec[j];
					vec.push_back(json_step.GetInt());
				}
			}
			break;														//只存储当前关卡的范围
		}else{
			CCLOG("[ *ERROR ] WarMapLayer::initGrids Lost stage id");
			continue;
		}
	}
	if (!vec.size())
	{
		for (int j=C_STANDGRID;j< C_GRID_ROW * C_GRID_COL;j++)			//默认情况处理
			vec.push_back(j);
	}
}
