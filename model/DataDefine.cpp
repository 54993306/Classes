#include "DataDefine.h"
#include "roleImg/RoleUpdate.h"

BaseObject::BaseObject()
	:m_AliveID(0)
	,m_Model(0)
{}

void UserData::read(const protos::common::Role& role)
{
	if (this->m_level<role.rolelv()&&role.roleid()==this->getRoleID())
	{
		CRoleUpdate *roleUpdate = CRoleUpdate::create();
		roleUpdate->setZOrder(10);
		LayerManager::instance()->push(roleUpdate);
		roleUpdate->updateRole(this,&role);
	}

	if (role.has_rolename())
	{
		this->m_roleName = role.rolename();
	}
	if (role.has_roleid())
	{
		this->m_roleId = role.roleid();
	}
	if (role.has_rolelv())
	{
		this->m_level = role.rolelv();
	}
	if (role.has_rolecoin())
	{
		this->m_coin = role.rolecoin();
	}
	if (role.has_rolegold())
	{
		this->m_roleGold = role.rolegold();
	}
	if (role.has_roleexp())
	{
		this->m_exp = role.roleexp();
	}
	if (role.has_nextexp())
	{
		this->m_nextExp = role.nextexp();
	}
	if (role.has_roleaction())
	{
		this->m_roleAction = role.roleaction();
	}
	if (role.has_rolefood())
	{
		this->m_roleFood = role.rolefood();
	}
	if (role.has_rolefcoin())
		this->m_Friends = role.rolefcoin();

	if (role.has_actionlimit())
	{
		this->m_actionLimit = role.actionlimit();
	}

	if (role.has_actiontime())
	{
		this->m_actionTime = role.actiontime();
	}
	if (role.has_interval())
	{
		this->m_interval = role.interval();
	}

	if (role.has_vip())
	{
		this->m_vip = role.vip();
	}
	if (role.has_thumb())
	{
		this->m_thumb = role.thumb();
	}
	if (role.has_combat())
	{
		this->m_combat = role.combat();
	}
	if (role.has_newerstep())
	{
		this->m_newStep = role.newerstep();
	}
	else
	{
		this->m_newStep = 0;
	}

	if (role.has_mobileshop())
	{
		this->m_mobileShop = role.mobileshop();
	}
// 	else
// 	{
// 		this->m_mobileShop = false;
// 	}

	if (role.has_invitefriend())
	{
		this->m_inviteFriend = role.invitefriend();
	}
	if (role.has_fbid())
	{
		this->m_fbId = role.fbid();
	}
	if(role.has_firstlogin())
	{
		this->m_bFirstLogin = role.firstlogin();
	}
	if (role.has_rolemoney())
	{
		this->m_roleMoney = role.rolemoney();
	}
	if (role.has_rolepoints())
	{
		this->m_rolePoints = role.rolepoints();
	}
	if(role.has_goolebilling())
	{
		m_bGoogleBilling = role.goolebilling();
	}
// 	this->m_thumb = 0;
// 	this->m_fbId = "677158805720606";
}

UserData::UserData()
	:m_raceType(0),m_level(0),m_roleFood(0),m_Friends(0),m_newStep(0),m_roleId(0)
	,m_coin(0),m_exp(0),m_nextExp(0),m_roleAction(0),m_mapId(0),m_roleGold(0),m_roleName("")
	,m_actionLimit(0),m_interval(0),m_vip(0),m_combat(0),m_thumb(0),m_inviteFriend(0)
	,m_isFBLogin(false),m_mobileShop(false),m_bFirstLogin(false),m_roleMoney(0),m_rolePoints(0),m_bGoogleBilling(false)
{}