#ifndef __VIP_DATA_
#define __VIP_DATA_

#include "bag/bagData.h"
#include "protos/protocol.h"
#include "cocos2d.h"
USING_NS_CC;

//VIP特权信息
struct CVipData{
	int level ;		//VIP等级
	int target ;		//达到该VIP等级需累计充值元宝数
	int buyAction ;		//额外购买体力次数
	int buyCoin ;		//额外购买铜钱次数
	int actionLimit;		//体力上限增加数
	vector<CPrize> prizeList;	//特权奖励列表
	CVipData();
	void read(const protos::VipData& vd);
};
//获取玩家VIP等级信息 (63)
struct CVipInfoRes 
{
	int level ;	//VIP等级
	int exp ;		//当前VIP经验
	int nextExp;	//下一VIP等级经验
	CVipData data;	//特权信息

	CVipInfoRes();
	void read(const protos::VipInfoRes& vr);
};

struct VipPriv
{
	string priv1;
	string priv2;
	string priv3;
};

struct CGoldCard{
	int id;			//ID
	string name;	//名称
	int price;		//价格
	bool recommend;	//是否推荐
	bool canBy;		//是否可购买
	string note;	//描述
	CGoldCard();
	void read(const GoldCard& gc);
};

typedef map<int,vector<VipPriv>> VipPrivMap;
class CVipDesc
{
private:
	VipPrivMap  m_privMap;
public:
	CVipDesc();
	void parseXml(const char* xml);
	const VipPrivMap& getPriv();
	const vector<VipPriv>& getPrivByLevel(int level);
protected:

};

#endif