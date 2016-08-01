#include "ParseFileData.h"
#include "tools/ToolDefine.h"
#include "model/DataCenter.h"
#include "model/WarManager.h"
#include "Battle/ConstNum.h"
#include "Battle/BattleTools.h"
using namespace BattleSpace;
//�������ƶ������ļ�
void ParseBattleGrid( int pStage,vector<int> &pMoveGrids,vector<int> &pUndefinedGrids,vector<int> &pEnterArea,vector<int> &pOtherEnter )
{
	pUndefinedGrids.clear();
	pMoveGrids.clear();
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
	for (rapidjson::SizeType i = 0;i<Vec.Size();i++)
	{
		rapidjson::Value& json_step = Vec[i];
		if (json_step.HasMember("id"))
		{
			rapidjson::Value & var = json_step["id"];
			if (var.GetInt() != pStage)								//ֻ�洢��ǰ�ؿ��Ŀ��ƶ���Χ
				continue;
			if (json_step.HasMember("grid"))
			{
				rapidjson::Value & par = json_step["grid"];
				for (int j=par.GetInt();j< C_CAPTAINGRID;j++)
					pMoveGrids.push_back(j);
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
					pMoveGrids.push_back(json_step.GetInt());
				}
			}
			if (json_step.HasMember("undefine"))
			{
				rapidjson::Value& pVec = json_step["undefine"];		//�õ�����Ӧ��������Ϣ
				if (!pVec.IsArray())								//�ж��Ƿ�Ϊ����
				{
					CCLOG("[ *ERROR ] ParseFileData::ParseMoveGrid Can Not't Find undefine Vec");
					continue;
				}
				for (rapidjson::SizeType j = 0;j<pVec.Size();j++)
				{
					rapidjson::Value& json_step = pVec[j];
					pUndefinedGrids.push_back(json_step.GetInt());
				}
			}
			if (json_step.HasMember("EnterGrid"))
			{
				rapidjson::Value & par = json_step["EnterGrid"];
				for (int j=par.GetInt();j< C_CAPTAINGRID;j++)
					pEnterArea.push_back(j);
			}
			if (json_step.HasMember("EnterGrids"))
			{
				rapidjson::Value& pVec = json_step["EnterGrids"];		//�õ�����Ӧ��������Ϣ
				if (!pVec.IsArray())								//�ж��Ƿ�Ϊ����
				{
					CCLOG("[ *ERROR ] ParseFileData::ParseMoveGrid Can Not't Find EnterGrids Vec");
					continue;
				}
				for (rapidjson::SizeType j = 0;j<pVec.Size();j++)
				{
					rapidjson::Value& json_step = pVec[j];
					pEnterArea.push_back(json_step.GetInt());
				}
			}
			if (json_step.HasMember("OtherEnter"))
			{
				rapidjson::Value & par = json_step["OtherEnter"];
				for (int tGrid = C_PVEStopGrid;tGrid <= par.GetInt();tGrid++)
					pOtherEnter.push_back( tGrid );
			}
			break;													//ֻ�洢��ǰ�ؿ��Ŀ��ƶ���Χ
		}else{
			CCLOG("[ *ERROR ] ParseFileData::ParseMoveGrid Lost stage id");
			continue;
		}
	}
	if (pMoveGrids.empty())
	{
		for (int j=C_BEGINGRID;j< C_CAPTAINGRID;j++)				//Ĭ���������(�������ƶ�)
			pMoveGrids.push_back(j);
	}
	if (pEnterArea.empty())
		pEnterArea.assign(pMoveGrids.begin(),pMoveGrids.end());
	if (pOtherEnter.empty())
		pOtherEnter.assign(pMoveGrids.begin(),pMoveGrids.end());
	VectorUnique(pOtherEnter,true);
	VectorUnique(pEnterArea);
	VectorUnique(pMoveGrids);
	VectorUnique(pUndefinedGrids);
}
//��������cost�����ļ�
void ParseAddCostGrid( int pStage,vector<int> & pAddCostArea )
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
	pAddCostArea.clear();
	for (rapidjson::SizeType i = 0;i<Vec.Size();i++)
	{
		rapidjson::Value& json_step = Vec[i];
		if (json_step.HasMember("id"))
		{
			rapidjson::Value & var = json_step["id"];
			if (var.GetInt() != pStage)
				continue;
			if (json_step.HasMember("grid"))
			{
				rapidjson::Value & par = json_step["grid"];
				for (int j=par.GetInt();j< C_GRID_ROW * C_GRID_COL;j++)
					pAddCostArea.push_back(j);
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
					pAddCostArea.push_back(json_step.GetInt());
				}
			}
			break;														//ֻ�洢��ǰ�ؿ��ķ�Χ
		}else{
			CCLOG("[ *ERROR ] WarMapLayer::initGrids Lost stage id");
			continue;
		}
	}
	if (!pAddCostArea.size())
	{
		for (int j=C_STANDGRID;j< C_GRID_ROW * C_GRID_COL;j++)			//Ĭ���������
			pAddCostArea.push_back(j);
	}
	VectorUnique(pAddCostArea);
}

void parseSpineModelFile( vector<int>& pVector )
{
	pVector.clear();
	CSVFile* pFile = (CSVFile*)FileUtils::sharedFileUtils()->loadCSVFile("csv/SpineIDFile.csv");
	for (int pIndex=0;pIndex < pFile->getRowNum(); pIndex++)
		pVector.push_back(atoi(pFile->get(pIndex,0)));
	VectorUnique(pVector);
	FileUtils::sharedFileUtils()->releaseFile("csv/SpineIDFile.csv");
}
