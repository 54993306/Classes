#include "CombatGuideManage.h"
#include "CombatGuideData.h"
#include "CombatGuideLayer.h"
#include "Battle/BattleScene/BattleScene.h"
#include "Battle/BattleLayer/BattleRoleLayer.h"
#include "Battle/ConstNum.h"
#include "common/CGameSound.h"
namespace BattleSpace{
	CombatGuideManage::CombatGuideManage()
		:m_CurrStepIndex(0),mGuideState(false),m_GuideLayer(nullptr)
	{}

	CombatGuideManage::~CombatGuideManage()
	{
		clearGuideData(true);
	}

	void CombatGuideManage::clearGuideData(bool quit /*=false*/)
	{
		for(MapCombatGuideData::iterator iter = m_GuideData.begin(); iter != m_GuideData.end();++iter)
			CC_SAFE_RELEASE(iter->second);
		m_GuideData.clear();
		if (quit)
		{
			m_GuideLayer = nullptr;
			mGuideState = false;
		}
	}

	bool CombatGuideManage::LoadJsonFile(const char* path)
	{
		unsigned long size = 0;
		char FilePath [60] = {0};
		sprintf(FilePath,"csv/guide/%s.json",path);
		unsigned char *buff = CCFileUtils::sharedFileUtils()->getFileData(FilePath,"r",&size);
		string data ((const char*)buff,size);
		rapidjson::Document doc;
		doc.Parse<0>(data.c_str());
		if ( doc.HasParseError())
		{
			CCLOG("[ *ERROR ] CombatGuideData::LoadJsonFile %s",doc.GetParseError());
			return false;
		}
		if ( !doc.IsObject() || !doc.HasMember("guid") )
		{
			CCLOG("[ *ERROR ] CombatGuideData::LoadJsonFile Create Json Project Fail");
			return false;
		}
		rapidjson::Value& VecGrid = doc["guid"];		//得到键对应的数组信息
		if (!VecGrid.IsArray())							//判断是否为数组
		{
			CCLOG("[ *ERROR ] CombatGuideData::LoadJsonFile Can Not't Find Vec");
			return false;
		}
		clearGuideData();
		for (rapidjson::SizeType i = 0;i<VecGrid.Size();i++)
		{
			rapidjson::Value& json_step = VecGrid[i];		//得到数组里单步引导数据
			CombatGuideStep* guide_step = CombatGuideStep::create();
			if (json_step.HasMember("id"))
			{
				rapidjson::Value & var = json_step["id"];
				guide_step->setId(var.GetInt());
			}else{
				CCLOG("[ *TIPS ] CombatGuideData::LoadJsonFile Lost stpe id");
				continue;
			}
			if (json_step.HasMember("backid"))
			{
				rapidjson::Value & var = json_step["backid"];
				guide_step->setBackID(var.GetInt());
			}else{
				CCLOG("[ *TIPS ] CombatGuideData::LoadJsonFile Lost stpe frontid");
			}
			if (json_step.HasMember("nextid"))
			{
				rapidjson::Value & var = json_step["nextid"];
				guide_step->setNextID(var.GetInt());
			}else{
				CCLOG("[ *TIPS ] CombatGuideData::LoadJsonFile Lost stpe backid");
			}
			if (json_step.HasMember("type"))
			{
				rapidjson::Value & var = json_step["type"];
				guide_step->setType(var.GetInt());
			}else{
				CCLOG("[ *TIPS ] CombatGuideData::LoadJsonFile Lost stpe type");
			}

			if (json_step.HasMember("Uitype"))
			{
				rapidjson::Value & var = json_step["Uitype"];
				guide_step->setUiType(var.GetInt());
			}
			if (json_step.HasMember("beginGrid"))
			{
				rapidjson::Value & var = json_step["beginGrid"];
				guide_step->setBegingrid(var.GetInt());
			}
			if (json_step.HasMember("FnishGrid"))
			{
				rapidjson::Value & var = json_step["FnishGrid"];
				guide_step->setFinishgrid(var.GetInt());
			}
			if (json_step.HasMember("RoleImage"))
			{
				rapidjson::Value & var = json_step["RoleImage"];
				guide_step->setRoleImage(var.GetInt());
			}
			if (json_step.HasMember("RoleFlipX"))
			{
				rapidjson::Value & var = json_step["RoleFlipX"];
				guide_step->setRoleFlipX(var.GetBool());
			}
			if (json_step.HasMember("RoleScale"))
			{
				rapidjson::Value & var = json_step["RoleScale"];
				guide_step->setRoleScale(var.GetDouble());
			}
			if (json_step.HasMember("RolePoint_x"))
			{
				rapidjson::Value & var = json_step["RolePoint_x"];
				guide_step->m_RolePoint.x = var.GetInt();
			}

			if (json_step.HasMember("RolePoint_y"))
			{
				rapidjson::Value & var = json_step["RolePoint_y"];
				guide_step->m_RolePoint.y = var.GetInt();
			}
			if (json_step.HasMember("Name"))
			{
				rapidjson::Value & var = json_step["Name"];
				guide_step->setName(var.GetString());
			}
			if (json_step.HasMember("Name_x"))
			{
				rapidjson::Value & var = json_step["Name_x"];
				guide_step->m_NamePoint.x = var.GetInt();
			}

			if (json_step.HasMember("Name_y"))
			{
				rapidjson::Value & var = json_step["Name_y"];
				guide_step->m_NamePoint.y = var.GetInt();
			}
			if (json_step.HasMember("describe"))
			{
				rapidjson::Value & var = json_step["describe"];
				guide_step->setDescribe(var.GetString());
			}
			if (json_step.HasMember("desc_x"))
			{
				rapidjson::Value & var = json_step["desc_x"];
				guide_step->m_Despoint.x = var.GetInt();
			}
			if (json_step.HasMember("desc_y"))
			{
				rapidjson::Value & var = json_step["desc_y"];
				guide_step->m_Despoint.y = var.GetInt();
			}
			if (json_step.HasMember("size"))
			{
				rapidjson::Value & var = json_step["size"];
				guide_step->setsize(var.GetInt());
			}
			if (json_step.HasMember("describe_color"))
			{
				rapidjson::Value & var = json_step["describe_color"];
				guide_step->setDescribeFontColor(var.GetInt());
			}
			if (json_step.HasMember("describe_HorizontalAlignment"))
			{
				rapidjson::Value & var = json_step["describe_HorizontalAlignment"];
				guide_step->setDescribeAligment(var.GetInt());
			}
			if (json_step.HasMember("describe_anchorpoint_x"))
			{
				rapidjson::Value & var = json_step["describe_anchorpoint_x"];
				guide_step->m_DescribeAnchor.x  = var.GetInt();
			}
			if (json_step.HasMember("describe_anchorpoint_y"))
			{
				rapidjson::Value & var = json_step["describe_anchorpoint_y"];
				guide_step->m_DescribeAnchor.y  = var.GetInt();
			}
			if (json_step.HasMember("describe_Dimensions_x"))
			{
				rapidjson::Value & var = json_step["describe_Dimensions_x"];
				guide_step->m_DescribeDimensions.width = var.GetInt();
			}
			if (json_step.HasMember("describe_Dimensions_y"))
			{
				rapidjson::Value & var = json_step["describe_Dimensions_y"];
				guide_step->m_DescribeDimensions.height = var.GetInt();
			}
			if (json_step.HasMember("Captain"))
			{
				rapidjson::Value & var = json_step["Captain"];
				guide_step->setCaptain(var.GetBool());
			}
			if (json_step.HasMember("Reset"))
			{
				rapidjson::Value & var = json_step["Reset"];
				guide_step->setReset(var.GetBool());
			}
			if (json_step.HasMember("AddCost"))
			{
				rapidjson::Value & var = json_step["AddCost"];
				guide_step->setAddCost(var.GetInt());
			}
			if (json_step.HasMember("LockLayer"))
			{
				rapidjson::Value & var = json_step["LockLayer"];
				guide_step->setLockLayer(var.GetBool());
			}
			if (json_step.HasMember("BlackLayer"))
			{
				rapidjson::Value & var = json_step["BlackLayer"];
				guide_step->setBlackLayer(var.GetBool());
			}
			if (json_step.HasMember("BlackBottom"))
			{
				rapidjson::Value & var = json_step["BlackBottom"];
				guide_step->setBlackBottom(var.GetBool());
			}
			if(json_step.HasMember("ImageData"))
			{
				rapidjson::Value& imageDataList = json_step["ImageData"];		//得到键对应的数组信息
				if (!imageDataList.IsArray())							//判断是否为数组
				{
					CCLOG("[ *ERROR ] CombatGuideData::LoadJsonFile Line=194");
				}
				else
				{
					for (rapidjson::SizeType j = 0;j<imageDataList.Size();j++)
					{
						rapidjson::Value& data = imageDataList[j];		//得到数组里单步引导数据
						ImageData imageData;
						if (data.HasMember("isSpine"))
						{

							imageData.isSpine = data["isSpine"].GetBool();
							if (data.HasMember("File"))
							{
								rapidjson::Value & var = data["File"];
								std::string id= var.GetString();
								imageData.spineID = atoi(id.c_str());
							}
						}else{
							if (data.HasMember("File"))
							{
								rapidjson::Value & var = data["File"];
								imageData.sFile = var.GetString();
							}
						}

						if (data.HasMember("Scale"))
						{
							rapidjson::Value & var = data["Scale"];
							imageData.fScale = var.GetDouble();
						}
						if (data.HasMember("PosX"))
						{
							rapidjson::Value & var = data["PosX"];
							imageData.pos.x = var.GetInt();
						}
						if (data.HasMember("PosY"))
						{
							rapidjson::Value & var = data["PosY"];
							imageData.pos.y = var.GetInt();
						}
						if (data.HasMember("FlipX"))
						{
							rapidjson::Value & var = data["FlipX"];
							imageData.bFlipX = var.GetBool();
						}
						if (data.HasMember("FlipY"))
						{
							rapidjson::Value & var = data["FlipY"];
							imageData.bFlipY= var.GetBool();
						}
						if (data.HasMember("FlipY"))
						{
							rapidjson::Value & var = data["FlipY"];
							imageData.bFlipY= var.GetBool();
						}
						if (data.HasMember("Rotation"))
						{
							rapidjson::Value & var = data["Rotation"];
							imageData.iRotation= var.GetInt();
						}
						if (data.HasMember("MoveDir"))
						{
							rapidjson::Value & var = data["MoveDir"];
							imageData.iDir= var.GetInt();
						}
						if (data.HasMember("Zorder"))
						{
							rapidjson::Value & var = data["Zorder"];
							imageData.iZorder= var.GetInt();
						}
						if (data.HasMember("SendMsg"))
						{
							rapidjson::Value & var = data["SendMsg"];
							imageData.SendMsg= var.GetInt();
						}
						guide_step->m_imageData.push_back(imageData);
					}
				}
			}
			if (json_step.HasMember("VecCompel"))
			{
				rapidjson::Value& VecCompel = json_step["VecCompel"];		//得到键对应的数组信息
				if (!VecCompel.IsArray())							//判断是否为数组
				{
					CCLOG("[ *ERROR ] CombatGuideData::LoadJsonFile Line=168");
					return false;
				}
				for (rapidjson::SizeType j = 0;j<VecCompel.Size();j++)
				{
					rapidjson::Value& ValCompel = VecCompel[j];		//得到数组里单步引导数据
					AliveCompel compel;
					if (ValCompel.HasMember("ModeID"))
					{
						rapidjson::Value & var = ValCompel["ModeID"];
						compel.model = var.GetInt();
					}else{
						break;
					}
					if (ValCompel.HasMember("grid"))
					{
						rapidjson::Value & var = ValCompel["grid"];
						compel.grid = var.GetInt();
					}else{
						break;
					}
					guide_step->m_VecCompel.push_back(compel);
				}
			}
			addGuideStep(guide_step);
		}
		return true;
	}

	void CombatGuideManage::addGuideStep(CombatGuideStep* step)
	{
		if (!step)return;
		CombatGuideStep* u_step = getGuideStep(step->getId());
		if (u_step)
		{
			if (u_step == step)
			{
				CCLOG("[ *ERROR ] CombatGuideManage::addGuideStep Add Second");
			}else{
				CCLOG("[ *ERROR ] CombatGuideManage::addGuideStep ID Repeat");
			}
			return ;
		}
		step->retain();
		m_GuideData[step->getId()] = step;
	}

	CombatGuideStep* CombatGuideManage::getGuideStep(int id)
	{
		MapCombatGuideData::iterator iter = m_GuideData.find(id);
		if (iter != m_GuideData.end())
			return iter->second;
		return nullptr;
	}

	void CombatGuideManage::setGuide(const char* FilePath,int beginStep /*= 1*/)
	{
		m_CurrStepIndex = beginStep;
		if ( ! LoadJsonFile(FilePath))			//根据剧情id加载剧情文件
		{
			mGuideState = false;
			m_GuideLayer->setTouchEnabled(false);
			return;
		}
		EnterGuide();
	}

	void CombatGuideManage::EnterGuide()							//让剧情层表现出剧情的信息
	{
		CombatGuideStep* step = getGuideStep(m_CurrStepIndex);
		if (!step)
		{
			CCLOG("[ *ERROR ] CombatGuideManage::EnterGuide Step Is NULL");
			ExitGuide();
			return;
		}
		mGuideState = true;
		m_GuideLayer->setTouchEnabled(true);
		m_GuideLayer->setVisible(true);
		m_GuideLayer->setGuideStep(step);
	}

	void CombatGuideManage::NextStep()
	{
		PlayEffectSound(SFX_423);	

		CombatGuideStep* Curr_step = getGuideStep(m_CurrStepIndex);
		if (Curr_step)
		{
			Curr_step->setFinish(true);
			if (Curr_step->getNextID() == INVALID_GRID)				//最后一步判断处理
			{
				ExitGuide(); 
				return;
			}
			if (Curr_step->getNextID())
				NextStepExcute(Curr_step);
		}else{
			ExitGuide(); 
			CCLOG("[ *ERROR ]CombatGuideManage::NextGuide Curr Guide Is NULL");
		}
	}

	void CombatGuideManage::NextStepExcute( CombatGuideStep* Curr_step )
	{
		CombatGuideStep* next_step = getGuideStep(Curr_step->getNextID());
		if (next_step)
		{
			m_CurrStepIndex = Curr_step->getNextID();
			m_GuideLayer->ClearGuideLayer();
			m_GuideLayer->setGuideStep(next_step);
		}else{
			ExitGuide();
			CCLOG("[ *ERROR ]CombatGuideManage::NextGuide Next Guide Is NULL");
		}
	}

	void CombatGuideManage::ExitGuide()								//移除剧情层显示内容
	{
		mGuideState = false;
		m_GuideLayer->ClearGuideLayer();
		m_GuideLayer->setTouchEnabled(false);
	}

	CombatGuideStep* CombatGuideManage::getCurrStep()
	{
		if (getGuideStep(m_CurrStepIndex))
		{
			return getGuideStep(m_CurrStepIndex);
		}
		return nullptr;
	}

	bool CombatGuideManage::moveGuideJudge( int pGrid,bool pNextStep /*=false*/ )
	{
		CombatGuideStep* tStep = getCurrStep();
		if (tStep &&  mGuideState)
		{
			if ((tStep->getType()==AliveMove_Type || tStep->getType() == CallAalive_Type))
			{
				if (pGrid != tStep->getFinishgrid())
					return false;								//固定格子才算完成(在移动区域内即算完成)
				if (pNextStep)
					NextStep();
			}
		}
		return true;
	}

	bool CombatGuideManage::isRest()
	{
		CombatGuideStep* tStep = getCurrStep();
		if (mGuideState && tStep &&  tStep->getReset() )
			return true;
		return false;
	}

}