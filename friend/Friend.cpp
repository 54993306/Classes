#include "Friend.h"

CFriend::CFriend():friendId(0),level(0),combat(0),loginTime(0),vip(0),thumb(0)
{

}

void CFriend::read(const protos::Friend& frd)
{
	this->friendId = frd.friendid();
	this->friendName = frd.friendname();
	this->level = frd.level();
	this->online = frd.online();
	this->combat = frd.combat();
	this->loginTime = frd.logintime();
	this->vip = frd.vip();
	this->thumb = frd.thumb();
	this->fcoin = frd.fcoin();
	this->fbId = frd.fbid();
}
