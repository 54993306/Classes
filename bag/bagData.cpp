#include "bagData.h"
#include "Battle/AnimationManager.h"
#include "common/CommonFunction.h"
using namespace BattleSpace;
CPrize::CPrize():type(0), id(0), num(0), quality(0), thumb(0), color(0)
{

}


void CPrize::read( const Prize& prize )
{
	this->type = prize.type();
	this->id = prize.id();
	this->num = prize.num();
	this->quality = prize.quality();
	this->thumb = prize.thumb();
	this->color = prize.color();
}



void CItem::read(const Item& item)
{
	this->id = item.id();
	this->itemId = item.itemid();
	this->itemName = item.itemname();
	this->itemType = item.itemtype();
	this->useLevel = item.uselevel();
	this->moneyType = item.moneytype();
	this->buyPrice = item.buyprice();
	this->sellPrice = item.sellprice();
	this->quality = item.quality();
	this->useTime = item.usetime();
	this->loseTime = item.losetime();
	this->itemNum = item.itemnum();
	this->gridNum = item.gridnum();

	this->itemLevel = item.itemlevel();
	this->itparam = item.param();
	this->iconId = item.thumb();
	this->canUse = item.canuse();

	this->iStar = item.star();

// 	memset(&armor,0,sizeof(CArmor));
// 	memset(&prop,0,sizeof(CProp));
	//memset(&monst,0,sizeof(CMonster));
	
	if (item.has_armor())
	{
		this->armor.read(item.armor());
	}
	if (item.has_prop())
	{
		this->prop.read(item.prop());
	}
	if (item.has_soul())
	{
	//	this->soul.read(item.soul());
	}
	
	for (int i = 0; i < item.drops_size(); i++)
	{
		CDropStage stage;
		stage.read(item.drops(i));
		stageList.push_back(stage);
	}
	this->stren = item.stren();
}

CItem::CItem()
:id(0)		//玩家道具ID
,itemId(0)	//道具标识ID
,itemName("")	//道具名称
,itemType(0) 	//道具类型(1 常规道具，只显示名称和描述说明，不具有特殊属性， 2 装备，3 消耗类道具 比如药品 ……)
,useLevel(0)	//使用等级
,moneyType(0)	//购买道具花费的货币类型
,buyPrice(0)	//购买价格
,sellPrice(0)	//出售价格
,quality(0)	//装备品阶(1 白色， 2 绿色， 3 蓝色，4 紫色， 5 橙色)
,itemLevel(0)//道具等级
,itemNum(0)	//拥有道具的数量
,gridNum(0)	//物品背包位置
,itparam(0)
,iconId(0)		//道具图标ID
,stren(false)
{

}


CArmor::CArmor()
	:armorType(0),baseHp(0),baseDef(0),baseAtk(0),strenLv(0),addHp(0),addDef(0),addAtk(0),baseDex(0),
	baseCrit(0),baseRenew(0),baseDodge(0),dodgeRange(""),critRange(""),renewRange(""),IntRange(""),armor_skill(0)
	,nextAtk(0),nextHp(0),nextDef(0)
{

}

void CArmor::read(const Armor& armor)
{
	this->armorType = armor.armortype();
	this->baseHp = armor.basehp();
	this->baseDef = armor.basedef();

	this->baseAtk = armor.baseatk();
	this->strenLv = armor.strenlv();
// 	this->baptize = armor.baptize();
	this->addHp = armor.addhp();
	this->addDef = armor.adddef();
	this->addAtk = armor.addatk();
	this->hero = armor.hero();
// 	for (int i = 0; i < armor.attr_size(); i++)
// 	{
// 		const ExtAttr& attr = armor.attr(i);
// 		CExtAttr ext;
// 		ext.read(attr);
// 		this->attrList.push_back(ext);
// 	}

	this->baseDex = armor.basedex();
	this->baseHit = armor.basehit();
	this->baseCrit = armor.basecrit();
	this->baseRenew = armor.baserenew();
	this->baseDodge = armor.basedodge();

	this->addDex = armor.adddex();
	this->addHit = armor.addhit();
	this->addCrit = armor.addcrit();
	this->addRenew = armor.addrenew();
	this->addDodge = armor.adddodge();

	this->atkRange = armor.atkrange();
	this->hpRange = armor.hprange();
	this->defRange = armor.defrange();
	this->IntRange = armor.dexrange();
	this->hitRange = armor.hitrange();
	this->critRange = armor.critrange();
	this->renewRange = armor.renewrange();
	this->dodgeRange = armor.dodgerange();

	this->nextAtk = armor.nextatk();
	this->nextHp = armor.nexthp();
	this->nextDef = armor.nextdef();
	this->nextDex = armor.nextdex();
	this->nextHit = armor.nexthit();
	this->nextCrit = armor.nextcrit();
	this->nextRenew = armor.nextrenew();
	this->nextDodge = armor.nextdodge();

	this->combat = armor.combat();

	this->armor_skill = armor.armor_skill();
}

// void CExtAttr::read(const ExtAttr& attr)
// {
// 	this->attrType = attr.attrtype();
// 	this->attrVal = attr.attrval();
// 	this->attrMax = attr.attrmax();
// }

void CProp::read(const Prop& prop)
{
	this->AddType = prop.addtype();
	this->AddVal = prop.addval();
	this->param =  prop.param();
}

void CBagData::read(const WareHouseResponse& res)
{
	this->maxNum = res.maxnum();
	
	for (int i = 0; i < res.itemlist_size(); i++)
	{
		const Item& item = res.itemlist(i);
		CItem it;
		it.read(item);
		this->itemList.push_back(it);
	}
}

CCTexture2D* setQualityTexture(int quality)
{
	std::string texture;
	switch (quality)
	{
	case 1:
		texture="public/whtie__mask.png";
		break;
	case 2:
		texture="public/green_mask.png";
		break;
	case 3:
		texture="public/blue_mask.png";
		break;
	case 4:
		texture="public/purple_mask.png";
		break;
	case 5:
		texture="public/orange_mask.png";
		break;
	default:
		texture="public/whtie__mask.png";
		break;
	}

    CCTexture2D *pTexture = CCTextureCache::sharedTextureCache()->addImage(texture.c_str());
	return pTexture;
}

CCTexture2D* setItemQualityTexture(int quality) 
{
	std::string texture;
	switch (quality)
	{ 
	case 1:
		texture="common/whtie__mask.png";
		break;
	case 2:
		texture="common/green_mask.png";
		break;
	case 3:
		texture="common/blue_mask.png";
		break;
	case 4:
		texture="common/purple_mask.png";
		break;
	case 5:
		texture="common/orange_mask.png";
		break;
	case 10:
		texture="common/box_purple_3.png";
		break;
	case 11:
		texture="common/box_white_2.png";
		break;
	case 12:
		texture="common/box_green_2.png";
		break;
	case 13:
		texture="common/box_blue_2.png";
		break;
	case 14:
		texture="common/box_purple_2.png";
		break;
	case 15:
		texture="common/box_yellow_2.png";
		break;
	default:
		texture="common/orange_mask.png";
		break;
	}
	CCTexture2D *pTexture = CCTextureCache::sharedTextureCache()->addImage(texture.c_str());
	return pTexture;
}

CImageView * getPrizeSprite(const CPrize *prize)
{
	CImageView *prop = CImageView::create(GetImageName(prize->type, prize->color, prize->thumb).c_str());
	if (!prop)
	{
		prop = CImageView::create("headImg/101.png");
		CCLOG("ERROR CImageView * getPrizeSprite  %d", prize->thumb);
	}
	return prop;
}


CImageView * getItemSprite(const CItem *item)
{
	CImageView *prop = CImageView::create(GetImageName(1, item->quality, item->iconId).c_str());
	if (!prop)
	{
		prop = CImageView::create("headImg/101.png");
		CCLOG("ERROR CImageView * getItemSprite %d", item->iconId);
	}
	return prop;
}

CCSprite * addItemLight(int quality, CCNode *parent, CCPoint pos)
{
	CCAnimation *lightAnim= AnimationManager::sharedAction()->getAnimation("7030");
	lightAnim->setDelayPerUnit(0.15f);
	if (pos.x==0&&pos.y==0)
	{	
		pos = ccp(parent->getContentSize().width/2,parent->getContentSize().height/2);//ccpAdd(parent->getPosition(),);
	}
	else
	{
		pos = ccpAdd(pos,ccp(parent->getContentSize().width/2,parent->getContentSize().height/2));
	}
	CCSprite *light = createAnimationSprite("skill/7030.png",pos,lightAnim,true);
	light->setTag(7030);
	parent->addChild(light,2);
	return light;
}

// void CSoul::read(const Soul& sou)
// {
// 
// }

void CStrengthenResponse::read(const StrengthenResponse& res)
{
	this->result = res.result();

	if (res.has_armor())
	{
		this->armor.read(res.armor());
	}

	this->coin = res.coin();
	this->gold = 0;
	this->maxLevel = res.maxlevel();
}

CDropStage::CDropStage()
	:stageId(0),open(false)
{

}

void CDropStage::read(const DropStage& drpSt)
{
	this->stageId = drpSt.stageid();
	this->open = drpSt.open();
}
