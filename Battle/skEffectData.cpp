
#include "Battle/skEffectData.h"
#include "Battle/BuffData.h"
#include "Battle/AffectArea/AffectArea.h"
#include <protos/common/skill_common.pb.h>
namespace BattleSpace{
	skEffectData::skEffectData()
		:mEffectID(0),mEffectType(0),mUserRate(0),mImpactType(0),mImpactRate(0),mGroupPos(0)
		,mChangeCost(0),mDamageRate(0),mRealHurt(0),mTargetType(0),mAreaSize(0),mGroup(0)
		,mBatter(0),mRepel(0),mHurtRatio(0),mSpaceRange(0),mAffectArea(nullptr)
	{}

	skEffectData::~skEffectData()
	{
		clearBuffData();
		CC_SAFE_RELEASE(mAffectArea);
		mAffectArea = nullptr;
	}

	skEffectData* skEffectData::create()
	{
		skEffectData* tData = new skEffectData();
		if (tData)
		{
			tData->autorelease();
			return tData;
		}else{
			delete tData;
			tData = nullptr;
			return nullptr;
		}
	}

	void skEffectData::readData(const protos::common::Effect* pEffect)
	{
		this->setEffectID(pEffect->effectid());
		this->setGroup(pEffect->group());
		this->setGroupPos(pEffect->pos());
		this->setEffectType(pEffect->type());
		this->setDamageRate(pEffect->damage()*0.01f);
		this->setRealHurt(pEffect->hurt());
		this->setTargetType(pEffect->target());
		this->setBatter(pEffect->batter());
		this->setRepel(pEffect->repel());
		this->setHurtRatio(pEffect->erange()*0.01f);
		this->setSpaceRange(pEffect->distance());
		this->setAreaSize(pEffect->range());
		this->setImpactType(pEffect->pro_type());
		this->setImpactRate(pEffect->pro_rate());

		for (int i=0; i<pEffect->bufflist_size();i++)
			addBuffData(&pEffect->bufflist(i));
		this->setuserRate(pEffect->userrate());
		this->setChangeCost(pEffect->cost());

		this->initAffect(pEffect->mode());
	}

	const std::vector<BuffData*>& skEffectData::getBuffVector() const
	{
		return mBuffVector;
	}

	void skEffectData::addBuffData( const protos::common::Buff* pBuffData )
	{
		BuffData* tBuffData = BuffData::create();
		tBuffData->readData(pBuffData);
		tBuffData->retain();
		mBuffVector.push_back(tBuffData);
	}

	void skEffectData::clearBuffData()
	{
		for (auto tData : mBuffVector)
		{
			CC_SAFE_RELEASE(tData);
		}
		mBuffVector.clear();
	}

	void skEffectData::initAffect( int pType )
	{
		if (mAffectArea)
		{
			CC_SAFE_RELEASE(mAffectArea);
			mAffectArea = nullptr;
		}
		mAffectArea = AffectArea::create(pType);
		mAffectArea->setEffectData(this);
		mAffectArea->retain();
	}

	void skEffectData::initArea( AreaCountInfo& pInfo ) 
	{
		if (!mAreaSize)
			return;
		if (mTargetType == eEnemyType)
		{
			for (auto tGrid : pInfo.getAlive()->mStandGrids)
				pInfo.addGrid(tGrid);
		}
		mAffectArea->commonHandle();
		mAffectArea->initArea(pInfo);
	}

	//ÉýÐò
	bool EffectSort(const skEffectData* Effect1,const skEffectData* Effect2)
	{
		return Effect1->getGroupPos() < Effect2->getGroupPos();
	}
}
