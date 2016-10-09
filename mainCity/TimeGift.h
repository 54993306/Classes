/******************************************************
*文件名称:	TimeGift.h
*编写日期:	2016-8-29-12:48
*编写作者:	YPF
*功能描述:	在线礼包
*******************************************************/

#include "scene/layer/LayerManager.h"
#include "AppUI.h"
#include "net/CNetClient.h"
#include <spine/spine-cocos2dx.h>

using namespace spine;

//请求在线礼包
inline void AskForOnlinePrize()
{
	GetTcpNet->sendDataType(OnlinePrizeMsg);
}

class CTimeGift : public BaseLayer
{
public:
	CTimeGift();
	~CTimeGift();
	
	bool init();
	CREATE_LAYER(CTimeGift);
	void onEnter();
	void onExit();
	
	void setTime( int iTime );

private:
	void ProcessMsg(int type, google::protobuf::Message *msg);

	//接收广播时间差
	void timeInBackground(CCObject* pObj);

	void updateForTime( float dt );

	void updateTime();
	void updateString();

	void onGift( CCObject *pSender );
	void onClose(CCObject*ob);

private:
	CLayout								*m_pUI;
	CLabel									*m_pText;
	int										m_iTime;
	SkeletonAnimation				*m_pSkeletonAnimation;
};