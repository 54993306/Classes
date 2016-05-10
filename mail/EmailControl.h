#ifndef __EMAIL_CONTROL_
#define __EMAIL_CONTROL_

#include "protos/protocol.h"
#include "AppUI.h"
#include "EmailData.h"
#include "EmailLayer.h"

class CMailLayer;

class CMailDelegate
{
public:
	//收件箱
	virtual void checkEMailCallBack(CMail *mail) = 0;			//点击查看邮件				
	virtual void delEMailCallBack(CMail *mail) = 0;								//点击删除按钮
	virtual void delAllEMailCallBack() = 0;								//点击删除全部邮件按钮
	virtual void extractAllCallBack() = 0;								//一键全部提取
	virtual void sendEMailCallBack(int friendId,char *title, char* content) = 0;		//发送邮件
	virtual void replyEMailCallBack(const char* name) = 0;									//回复邮件
	virtual void extractMailCallBack(int mailId) = 0;						//提取附件
	virtual void closeMailView()=0;
private:
};


class CEmailControl: public CCNode, public CMailDelegate
{
public:
	CREATE_FUNC(CEmailControl);
	CEmailControl();
	~CEmailControl();
	virtual bool init();
	void onEnter();
	void onExit();
	void sendToMail(int friendId,char *title, char* content);  //请求发送邮件
	void sendMailReq(int mailId, int reqType);    //请求邮件操作 1 删除邮件， 2 提取附件，3 读取邮件
	void sendMailList();      //邮件列表
	void ProcessMsg(int type, google::protobuf::Message *msg);
	void setEmailLayer(CMailLayer *layer);
	virtual void checkEMailCallBack(CMail *mail);						
	virtual void delEMailCallBack(CMail *mail);							
	virtual void delAllEMailCallBack();								
	virtual void extractAllCallBack();								
	virtual void extractMailCallBack(int mailId);
	virtual void sendEMailCallBack(int friendId,char *title, char* content);		//发送邮件
	virtual void replyEMailCallBack(const char* name);	
	virtual void closeMailView();
private:
	vector<CMail> m_mailList;
	CMailLayer *m_emailLayer;

	int m_opType;   //操作类型
	CMail *m_selectMail;
};


#endif