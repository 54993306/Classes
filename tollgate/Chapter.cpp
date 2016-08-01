#include "Chapter.h"

void CChapter::read(const Chapter& chap)
{
	this->id = chap.id();
	this->isOpen = chap.isopen();
	this->name = chap.name();
	this->star = chap.star();
	this->totalStar = chap.totalstar();
// 	if (chap.has_item())
// 	{	
// 		this->item.read(chap.item());
// 	}
	this->prize = chap.prize();
}

void CStage::read(const StageInfo& info)
{
	this->id = info.id();
	this->name = info.name();
	this->level = info.inlevel();
	this->isOpen = info.isopen();
	this->star = info.star();
	this->action = info.action();
	this->prize = info.prize();
	for (int i = 0; i < info.friends_size(); i++)
	{
		const string &frd = info.friends(i);
		this->friendList.push_back(frd);
	}
}

void CStageInfoRes::read(const StageInfoRes& info)
{
	this->action = info.action();
	for (int i = 0; i < info.monsterlist_size(); i++)
	{
		CMonster monst;
		monst.readData(info.monsterlist(i));
		monstList.push_back(monst);
	}
	for (int i = 0; i < info.itemlist_size(); i++)
	{
		CPrize prize;
		prize.read(info.itemlist(i));
		prizeList.push_back(prize);
	}
}
