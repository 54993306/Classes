#include "CoordsManage.h"
#include "Battle/ConstNum.h"
#include "Battle/Coords/Coords.h"
namespace BattleSpace
{
	CoordsManage::CoordsManage():mDefultData(nullptr),mCurrCoords(nullptr)
	{}

	CoordsManage::~CoordsManage()
	{
		CC_SAFE_RELEASE(mDefultData);
		mDefultData = nullptr;
		clearCoords();
	}

	void CoordsManage::clearCoords()
	{
		for (auto tPair : mCoordsMap)
		{
			CC_SAFE_RELEASE(tPair.second);
		}
		mCoordsMap.clear();
		mCurrCoords = nullptr;
	}

	bool CoordsManage::init()
	{
		mDefultData = Coords::createCoords(sCoordsType::eRectangle);
		mDefultData->retain();
		return true;
	}
	//目前，只有一种类型的地图
	void CoordsManage::initMap(int pStageID)
	{
		mCurrCoords = Coords::createCoords(sCoordsType::eRectangle);
		mCurrCoords->setMapID(pStageID);
		AddCoords(mCurrCoords);
	}

	void CoordsManage::AddCoords(Coords* pCoords)
	{
		Coords* tpMap = getCoords(pCoords->getMapID());
		if( tpMap->getMapID() )
		{
			CCLOG("MapManager has exists Map:%d",pCoords->getMapID());
			return;
		}
		pCoords->retain();
		mCoordsMap[pCoords->getMapID()] = pCoords;
	}
	
	Coords* CoordsManage::getCoords(int pCoordsID)
	{
		map<int,Coords*>::iterator iter = mCoordsMap.find(pCoordsID);
		if( iter != mCoordsMap.end() )
			return iter->second;
		CCLOG("[ *ERROR ] MapManager::getCoords %d",pCoordsID);
		return mDefultData;
	}

	int CoordsManage::getGridIndex( const CCPoint& pPoint ) const
	{
		if (mCurrCoords)
		{
			return mCurrCoords->getGridIndex(pPoint);
		}
		return INVALID_GRID;
	}

	const CCPoint& CoordsManage::getPoint( int pGrid ) const
	{
		if (mCurrCoords)
		{
			return mCurrCoords->getPoint(pGrid);
		}
		return CCPointZero;
	}

	float CoordsManage::CoordsMax() const
	{
		CCSize size = CCDirector::sharedDirector()->getWinSize();
		return size.width / 2 + fabs(size.width / 2 - mCurrCoords->getCoordsWidth() / 2);
	}

	float CoordsManage::CoordsMin() const
	{
		CCSize size = CCDirector::sharedDirector()->getWinSize();
		return size.width / 2 - fabs(( size.width - mCurrCoords->getCoordsWidth() ) / 2);
	}

	int CoordsManage::getCoordsRow() const
	{
		if (mCurrCoords)
			return mCurrCoords->getCoordsRow();
		return C_GRID_ROW;
	}

	int CoordsManage::getCoordsCol() const
	{
		if (mCurrCoords)
			return mCurrCoords->getCoordsCol();
		return C_GRID_COL;
	}

	int CoordsManage::getCoordsWith() const
	{
		if (mCurrCoords)
			return mCurrCoords->getCoordsWidth();
		return 0;
	}

	int CoordsManage::getCoordsHight() const
	{
		if (mCurrCoords)
			return mCurrCoords->getCoordsHeight();
		return 0;
	}
}