/******************************************************
*文件名称:	HandBookControl.h
*编写日期:	2016-8-30-16:46
*编写作者:	YPF
*功能描述:	图鉴相关
*******************************************************/

#include "net/CNetClient.h"

enum HandBookTabType
{
	HandBookTabNull = -1,				//null
	HandBookTabHero,						//英雄
	HandBookTabMonster,					//怪物
	HandBookTabEquip,						//装备
	HandBookTabMax					
};


//请求图鉴
void inline AskForHandBook(HandBookTabType type)
{
	AtlasRequest *pReq = new AtlasRequest;
	pReq->set_atlas_type(type+1);
	GetTcpNet->sendData(pReq, AtlasRequestMsg);
	delete pReq;
}


//图鉴中完成的数量
int inline CompleteInHandBook( AtlasResponse *pRes )
{
	int iN = 0;
	for ( auto atlas : pRes->atlas_list())
	{
		//奖励领取标志 0未领取(不可领取)，1未领取(可领取状态)，2 已领取
		if ( atlas.get_prize() > 0)
		{
			iN++;
		}
	}

	return iN;
}