#ifndef ITEM_GM_H_
#define ITEM_GM_H_
#include "DataDefine.h"

struct ItemData
{
	int itemId;
	string itemName;
	string itemDesc;
	string itemDesc1;
	string skillName;
	string skillDesc;
	int iItemType;
};


class ItemGM
{
public:
	typedef map<int,ItemData*> ItemCSV;
	ItemGM();
	~ItemGM();
	const ItemData* getCfg(int id);
	void parseItem();
protected:
	ItemCSV m_cfg;     
};
#endif // !SKILL_GM_H_
