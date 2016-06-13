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
		switch ((AffectType)pAffectType) 
		{
		case AffectType::eFront:
			{
				return AffectFront::create();
			}break;
		case AffectType::eNearby:
			{
				return AffectNearby::create();
			}break;
		case AffectType::eVertical:
			{
				return AffectVertical::create();
			}break;
		case AffectType::eCross:
			{
				return AffectCross::create();
			}break;
		case AffectType::eFrontTow:
			{
				return AffectFrontTwo::create();
			}break;
		case AffectType::eNearbyUnself:
			{
				return AffectNearbyUnself::create();
			}break;
		case AffectType::eEnemy:
			{
				return AffectEnemy::create();
			}break;
		case AffectType::eOur:
			{
				return AffectOur::create();
			}break;
		case AffectType::eAllArea:
			{
				return AffectAllArea::create();
			}break;
		case AffectType::ePuncture:
			{
				return AffectPuncture::create();
			}break;
		case AffectType::eTArea:
			{
				return AffectTArea::create();
			}break;
		case AffectType::eFrontSingle:
			{
				return AffectFrontSingle::create();
			}break;
		case AffectType::eCenterSingle:
			{
				return AffectCenterSingle::create();
			}break;
		case AffectType::eBackSingle:
			{
				return AffectBackSingle::create();
			}break;
		case AffectType::eFrontDisperse:
			{
				return AffectFrontDisperse::create();
			}break;
		case AffectType::eCenterDisperse:
			{
				return AffectCenterDisperse::create();
			}break;
		case AffectType::eBackDisperse:
			{
				return AffectBackDisperse::create();
			}break;
		case AffectType::eFrontRow:
			{
				return AffectFrontRow::create();
			}break;
		case AffectType::eCenterRow:
			{
				return AffectCenterRow::create();
			}break;
		case AffectType::eBackRow:
			{
				return AffectBackRow::create();
			}break;
		case AffectType::eAnyFixGrid:
			{
				return AffectAnyFixGrid::create();
			}break;
		case AffectType::eAnyFixGridDisperse:
			{
				return AffectAnyFixGridDisperse::create();
			}break;
		case AffectType::eAnyFixAlive:
			{
				return AffectAnyAlive::create();
			}break;
		case AffectType::eAnyFixAliveDisperse:
			{
				return AffectAnyAliveDisperse::create();
			}break;
		case AffectType::eLowestHp:
			{
				return AffectLowesHp::create();
			}break;
		case AffectType::eMapCenterTwoLine:
			{
				return AffectMapCenterTwoLine::create();
			}break;
		case AffectType::eFrontFourGrid:
			{
				return AffectFrontFourGrid::create();
			}break;
		case AffectType::eLowestHpUnCaptain:
			{
				return AffectLowesHpUnCaptain::create();
			}break;
		case AffectType::eMapAnyDoubleLine:
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
		if ( pRole->getBaseData()->getRoleRow() == 1 && 
			 pRole->getBaseData()->getRoleCol() == 1 )
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

	void AffectArea::commonHandle()
	{

	}
}