#include "RankData.h"
#include "support/tinyxml2/tinyxml2.h"



CTeams::CTeams()
	:id(0)
	,thumb(0)
	,quality(0)
	,color(0)
{
}

void CTeams::read(const Teams& data)
{
	this->id = data.id();
	this->thumb = data.thumb();
	this->quality = data.quality();
	this->color = data.color();
}

CRankData::CRankData()
	:rank(0)
	,hurt(0)
{
    roleData = new UserData;
}

CRankData::~CRankData()
{
}

void CRankData::read(const RankData& data)
{
	this->rank = data.rank();
	this->hurt = data.hurt();
	this->roleData->read(data.roledata());
	for (int i = 0; i < data.teamlist_size(); i++)
	{
		CTeams team;
		team.read(data.teamlist(i));
		this->teamList.push_back(team);
	}
}

void CRankResponse::read(const RankResponse& data)
{
	this->myRank.read(data.myrank());
	for (int i = 0; i < data.ranklist_size(); i++)
	{
		CRankData rank;
		rank.read(data.ranklist(i));
		this->rankList.push_back(rank);
	}
}

CRankResponse::~CRankResponse()
{
// 	vector<CRankData>::iterator iter = rankList.begin();
// 	for (; iter!= rankList.end(); iter++)
// 	{
// 		CC_SAFE_DELETE(iter->roleData);
// 		iter->roleData = nullptr;
// 	}
}
