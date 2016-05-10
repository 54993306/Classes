#include "ParseFileData.h"
#include "tools/ToolDefine.h"
#include "model/DataCenter.h"
#include "model/WarManager.h"
#include "ConstNum.h"
//�������ƶ������ļ�
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
	rapidjson::Value& Vec = doc["MoveGrids"];		//�õ�����Ӧ��������Ϣ
	if (!Vec.IsArray())								//�ж��Ƿ�Ϊ����
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
			if (var.GetInt() != stageid)				//ֻ�洢��ǰ�ؿ��Ŀ��ƶ���Χ
				continue;
			if (json_step.HasMember("grid"))
			{
				rapidjson::Value & par = json_step["grid"];
				for (int j=par.GetInt();j< C_CAPTAINGRID;j++)
					vec.push_back(j);
			}
			if (json_step.HasMember("grids"))
			{
				rapidjson::Value& pVec = json_step["grids"];		//�õ�����Ӧ��������Ϣ
				if (!pVec.IsArray())								//�ж��Ƿ�Ϊ����
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
			break;													//ֻ�洢��ǰ�ؿ��Ŀ��ƶ���Χ
		}else{
			CCLOG("[ *ERROR ] ParseFileData::ParseMoveGrid Lost stage id");
			continue;
		}
	}
	if (!vec.size())
	{
		for (int j=C_BEGINGRID;j< C_CAPTAINGRID;j++)				//Ĭ���������(�������ƶ�)
			vec.push_back(j);
	}
}
//��������cost�����ļ�
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
	rapidjson::Value& Vec = doc["COSTGRIDS"];		//�õ�����Ӧ��������Ϣ
	if (!Vec.IsArray())								//�ж��Ƿ�Ϊ����
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
				rapidjson::Value& pVec = json_step["grids"];		//�õ�����Ӧ��������Ϣ
				if (!pVec.IsArray())								//�ж��Ƿ�Ϊ����
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
			break;														//ֻ�洢��ǰ�ؿ��ķ�Χ
		}else{
			CCLOG("[ *ERROR ] WarMapLayer::initGrids Lost stage id");
			continue;
		}
	}
	if (!vec.size())
	{
		for (int j=C_STANDGRID;j< C_GRID_ROW * C_GRID_COL;j++)			//Ĭ���������
			vec.push_back(j);
	}
}
