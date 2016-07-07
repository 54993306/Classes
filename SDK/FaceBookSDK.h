
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
	static void	purge();//��Ϸ����ʱ�ͷ�ȫ�ֶ���

	//������Ȩ��¼
	void openAuthor();
	//�ر���Ȩ��¼
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