#include "ActivityData.h"

void CActivity::read(const Activity& activity)
{
	this->iActId = activity.actid();
	this->iType = activity.type();
	this->sName = activity.name();
	this->iStatus = activity.status();
	this->sIconFile = activity.icon();
	this->sInfoFile = activity.pic();
	this->iExtraParam = activity.param();
}