/************************************************************* 
 *
 *
 *		Data : 2016.6.12
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/
#include "Battle/AffectArea/AffectArea.h"
#include "Battle/AffectArea/AffectFront.h"
#include "Battle/AffectArea/AffectNearby.h"
#include "Battle/AffectArea/AffectVertical.h"
#include "Battle/AffectArea/AffectCross.h"
#include "Battle/AffectArea/AffectFrontTwo.h"
#include "Battle/AffectArea/AffectCamp.h"
#include "Battle/AffectArea/AffectTArea.h"
#include "Battle/AffectArea/AffectFrontArea.h"
#include "Battle/AffectArea/AffectCenterArea.h"
#include "Battle/AffectArea/AffectBackArea.h"
#include "Battle/AffectArea/AffectFixGrid.h"
#include "Battle/AffectArea/AffectFixAlive.h"
#include "Battle/AffectArea/AffectLowesHp.h"
#include "Battle/AffectArea/AffectMapArea.h"
#include "Battle/AffectArea/AffectFrontFourGrid.h"
#include "Battle/BaseRoleData.h"
namespace BattleSpace
{
	AffectArea* AffectArea::create(int pAffectType)
	{
		switch ((sAffectType)pAffectType) 
		{
		case sAffectType::eFront:
			{
				return AffectFront::create();
			}break;
		case sAffectType::eNearby:
			{
				return AffectNearby::create();
			}break;
		case sAffectType::eVertical:
			{
				return AffectVertical::create();
			}break;
		case sAffectType::eCross:
			{
				return AffectCross::create();
			}break;
		case sAffectType::eFrontTow:
			{
				return AffectFrontTwo::create();
			}break;
		case sAffectType::eNearbyUnself:
			{
				return AffectNearbyUnself::create();
			}break;
		case sAffectType::eEnemy:
			{
				return AffectEnemy::create();
			}break;
		case sAffectType::eOur:
			{
				return AffectOur::create();
			}break;
		case sAffectType::eAllArea:
			{
				return AffectAllArea::create();
			}break;
		case sAffectType::ePuncture:
			{
				return AffectPuncture::create();
			}break;
		case sAffectType::eTArea:
			{
				return AffectTArea::create();
			}break;
		case sAffectType::eFrontSingle:
			{
				return AffectFrontSingle::create();
			}break;
		case sAffectType::eCenterSingle:
			{
				return AffectCenterSingle::create();
			}break;
		case sAffectType::eBackSingle:
			{
				return AffectBackSingle::create();
			}break;
		case sAffectType::eFrontDisperse:
			{
				return AffectFrontDisperse::create();
			}break;
		case sAffectType::eCenterDisperse:
			{
				return AffectCenterDisperse::create();
			}break;
		case sAffectType::eBackDisperse:
			{
				return AffectBackDisperse::create();
			}break;
		case sAffectType::eFrontRow:
			{
				return AffectFrontRow::create();
			}break;
		case sAffectType::eCenterRow:
			{
				return AffectCenterRow::create();
			}break;
		case sAffectType::eBackRow:
			{
				return AffectBackRow::create();
			}break;
		case sAffectType::eAnyFixGrid:
			{
				return AffectAnyFixGrid::create();
			}break;
		case sAffectType::eAnyFixGridDisperse:
			{
				return AffectAnyFixGridDisperse::create();
			}break;
		case sAffectType::eAnyFixAlive:
			{
				return AffectAnyAlive::create();
			}break;
		case sAffectType::eAnyFixAliveDisperse:
			{
				return AffectAnyAliveDisperse::create();
			}break;
		case sAffectType::eLowestHp:
			{
				return AffectLowesHp::create();
			}break;
		case sAffectType::eMapCenterTwoLine:
			{
				return AffectMapCenterTwoLine::create();
			}break;
		case sAffectType::eFrontFourGrid:
			{
				return AffectFrontFourGrid::create();
			}break;
		case sAffectType::eLowestHpUnCaptain:
			{
				return AffectLowesHpUnCaptain::create();
			}break;
		case sAffectType::eMapAnyDoubleLine:
			{
				return AffectMapAnyDoubleLine::create();
			}break;
		default:
			{
				AffectArea* pAffect =new AffectArea();
				if (pAffect)
				{
					pAffect->autorelease();
					return pAffect;
				}else{
					delete pAffect;
					pAffect = nullptr;
					return nullptr;
				};
			}break;
		}
	}

	void AffectArea::initValidGrids( BaseRole* pRole,std::vector<int>& pValids )
	{
		if ( pRole->singleGrid() )
		{
			if (pRole->getTouchState())
			{
				pValids.push_back(pRole->getTouchGrid());
			}else{
				pValids.push_back(pRole->getGridIndex());
			}
		}else{
			moriGridRole(pRole,pValids);
		}
	}

	void AffectArea::moriGridRole( BaseRole* pRole,std::vector<int>& pValids )
	{
		vector<int> CountGrid;
		if (pRole->getTouchState())								//单格子武将触摸状态下处理(此处应进行移动状态下攻击格子特殊处理)
		{
			CountGrid = pRole->mTouchGrids;
		}else{
			CountGrid = pRole->mStandGrids;
		}
		if ( !CountGrid.size() )
			return;
		if (pRole->getEnemy() || pRole->getOpposite())					//武将反向攻击
		{
			for (int i=0;i<pRole->getBaseData()->getRoleRow();i++)							//获取向后攻击的判断格子(我方武将反向)
				pValids.push_back(CountGrid.at(CountGrid.size()-i-1));
		}else{
			for (int i=0;i<pRole->getBaseData()->getRoleRow();i++)							//获取向后攻击的判断格子(我方武将反向)
				pValids.push_back(CountGrid.at(i));
		}
	}

	void AffectArea::commonHandle(AreaCountInfo& pInfo)
	{
		
		initArea(pInfo);
	}

	void AffectArea::setEffectData( skEffectData* pData )
	{
		mEffectData = pData;
		mAffectType = getAreaType();
	}

}