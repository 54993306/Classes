/******************************************************
*文件名称:	GoogleLoginSDK.h
*编写日期:	2016-8-19-19:30
*编写作者:	YPF
*功能描述:	Google登录
*******************************************************/
#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

class GoogleLoginSDK:public CCObject 
{
public:
	GoogleLoginSDK(void);
	~GoogleLoginSDK(void);

	static GoogleLoginSDK* getInstance();
	static void	purge();//游戏结束时释放全局对象

	//申请授权登录
	void openAuthor();

	void setCallbackForUserInfo(CCObject *target,SEL_CallFuncO func);

private:
	static GoogleLoginSDK *m_pInstance;

};