/******************************************************
*�ļ�����:	GoogleLoginSDK.h
*��д����:	2016-8-19-19:30
*��д����:	YPF
*��������:	Google��¼
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
	static void	purge();//��Ϸ����ʱ�ͷ�ȫ�ֶ���

	//������Ȩ��¼
	void openAuthor();

	void setCallbackForUserInfo(CCObject *target,SEL_CallFuncO func);

private:
	static GoogleLoginSDK *m_pInstance;

};