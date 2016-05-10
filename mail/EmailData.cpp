#include "EmailData.h"


void CMail::read(const Mail& mail)
{
	this->mailId = mail.mailid();
	this->title = mail.title();
	this->content = mail.content();
	if (mail.has_addtime())
	{
		this->addTime = mail.addtime();
	}
	this->senderId = mail.senderid();
	this->sender = mail.sender();
	this->readed = mail.readed();

	for (int i = 0; i < mail.attrlist_size(); i++)
	{
		CPrize prizeTemp;
		prizeTemp.read(mail.attrlist(i));
		prize.prizeList.push_back(prizeTemp);
	}

	this->icon = mail.icon();
}
