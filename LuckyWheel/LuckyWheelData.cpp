#include "LuckyWheelData.h"


CNotice::CNotice():player(""),type(0),num(0),id(0)
{

}

void CNotice::read(const protos::Notice& notice)
{
	this->player = notice.player();
	this->type = notice.prize_type();
	this->num = notice.prize_num();
	this->id = notice.prize_id();
}