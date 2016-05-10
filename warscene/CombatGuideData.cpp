#include "CombatGuideData.h"

CombatGuideStep::CombatGuideStep()
	:m_id(0),m_type(0),m_backID(0),m_nextID(0),m_begingrid(0),m_AddCost(0),
	m_Describe(""),m_Despoint(-100,-100),m_finishgrid(0),m_Captain(false),
	m_RoleImage(0),m_RolePoint(0,0),m_uiType(0),m_Name(""),m_NamePoint(0,0)
	,m_Finish(false),m_LockLayer(false),m_BlackLayer(false),m_size(27),m_bRoleFlipX(false)
	,m_fRoleScale(1.0f), m_BlackBottom(true),m_DescribeFontColor(255),m_DescribeAligment(0)
	,m_DescribeAnchor(ccp(0, 1)),m_DescribeDimensions(CCSizeMake(1138, 640))
{}