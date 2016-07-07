
#ifndef __FACEBOOK_SDK_
#define __FACEBOOK_SDK_

#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;


class FaceBookSDK:public CCObject 
{
public:
	static FaceBookSDK* sharedInstance();
	static void	purge();//游戏结束时释放全局对象

	//申请授权登录
	void openAuthor();
	//关闭授权登录
	void closeAuthor();
	void onOpenPickFriends();
	void setCallbackForAuthor(CCObject *target,SEL_CallFuncO func);
	void setCallbackForUserInfo(CCObject *target,SEL_CallFuncO func);
	void setCallbackForFriendList(CCObject *target,SEL_CallFuncO func);
	void onShareToFb(const char* desc, const char* imgUrl);

	void openFriendList();
private:
	FaceBookSDK(void);
	~FaceBookSDK(void);

	static FaceBookSDK *g_Instance;

	void onUpdate(float dt);

};

#endif