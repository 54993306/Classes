﻿#pragma once

/******************************************************
*文件名称:	CheckMoney.h
*编写日期:	2016-6-14-11:38
*编写作者:	YPF
*功能描述:	检查货币是否足够
*******************************************************/

#include "model/DataCenter.h"

/************************************************************************/
/*												检查本地货币                                                       */
/************************************************************************/

//检查行动力
bool inline CheckActionPower(int iNeed)
{
	UserData *user = DataCenter::sharedData()->getUser()->getUserData();
	bool bAnswer = user->getRoleAction()>=iNeed;
	return bAnswer;
}

//检查行动力上限
bool inline CheckActionPowerLimit(int iNeed)
{
	UserData *user = DataCenter::sharedData()->getUser()->getUserData();
	bool bAnswer = user->getActionLimit()>=iNeed;
	return bAnswer;
}

//检查金币
bool inline CheckCoin(int iNeed)
{
	UserData *user = DataCenter::sharedData()->getUser()->getUserData();
	bool bAnswer = user->getCoin()>=iNeed;
	return bAnswer;
}

//检查勾玉
bool inline CheckGold(int iNeed)
{
	UserData *user = DataCenter::sharedData()->getUser()->getUserData();
	bool bAnswer = user->getRoleGold()>=iNeed;
	return bAnswer;
}

//检查友情点
bool inline CheckFriendShip(int iNeed)
{
	UserData *user = DataCenter::sharedData()->getUser()->getUserData();
	bool bAnswer = user->getFriends()>=iNeed;
	return bAnswer;
}

//检查灵魂点
bool inline CheckGhost(int iNeed)
{
	UserData *user = DataCenter::sharedData()->getUser()->getUserData();
	bool bAnswer = user->getRoleFood()>=iNeed;
	return bAnswer;
}

//检查积分
bool inline CheckRolePoint(int iNeed)
{
	UserData *user = DataCenter::sharedData()->getUser()->getUserData();
	bool bAnswer = user->getRolePoints()>=iNeed;
	return bAnswer;
}