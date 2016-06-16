
#include "PanelTips.h"


CPanelTips::CPanelTips():m_ui(nullptr)
{
}


CPanelTips::~CPanelTips()
{
}

bool CPanelTips::init()
{
	if (BaseLayer::init())
	{

		m_ui = LoadComponent("Tips.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		m_ui->setScale(0.6f);

		setIsShowBlack(false);

		setVisible(true);

		return true;
	}
	return false;
}


void CPanelTips::onEnter()
{ 
	BaseLayer::onEnter();

	CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("BoneAnimation/8021.ExportJson");

	//Pos node
	CCNode* pNode = (CCNode*)m_ui->findWidgetById("pos");

	//猫头鹰
	CCArmature* pArmature = CCArmature::create("8021");
	pArmature->setPosition(pNode->getPosition());
	//pArmature->setScale(0.9f);
	pArmature->getAnimation()->playWithIndex(0);
	m_ui->addChild(pArmature, -1);
}


void CPanelTips::onExit()
{
	BaseLayer::onExit();

	CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo("BoneAnimation/8021.ExportJson");
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

void CPanelTips::setString( const char* sInfo )
{
	CLabel* pLabel = (CLabel*)m_ui->findWidgetById("info");
	pLabel->setString(sInfo);
}
