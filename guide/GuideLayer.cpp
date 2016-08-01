#include "GuideLayer.h"
#include "net/CNetClient.h"
#include "net/CNetClient.h"
#include "GuideManager.h"
#include "mainCity/RecruitResult.h"
#include "Battle/AnimationManager.h"
#include "CCTK/scenemanager.h"
#include "GMessage.h"
#include "tools/UICloneMgr.h"
using namespace BattleSpace;
CGuideLayer::CGuideLayer()
{

}

bool CGuideLayer::init()
{
	if (BaseLayer::init())
	{
		CColorView *colorView = CColorView::create(ccc4(0,0,0, 0));
		colorView->setPosition(VCENTER);
		colorView->setContentSize(CCSizeMake(1138,640));

		setIsShowBlack(false);

		m_ui = LoadComponent("Guide.xaml"); 
		m_ui->setPosition(VCENTER);
		m_ui->setVisible(false);
		this->addChild(m_ui);

		CCClippingNode *clipNode = CCClippingNode::create();
		clipNode->setInverted(true);
		clipNode->setTag(100);
		clipNode->addChild(colorView);
		m_stencil = CCSprite::create("public/guide/blue.png");
		m_stencil->setPosition(VCENTER);
		clipNode->setStencil(m_stencil);
		this->addChild(clipNode);
		m_pClipNode = clipNode;

		setIsShowBlack(true);
		clipNode->setVisible(false);

		m_selectSpr = CCSprite::create("public/guide/a_0002.png");
		m_selectSpr->setPosition(VCENTER);
		m_selectSpr->setOpacity(0);
		CCActionInterval *action = CCSequence::createWithTwoActions(CCScaleTo::create(0.35f,0.96f),CCScaleTo::create(0.4f,1.0f));
		CCAction *repeatAct = CCRepeatForever::create(action);		
		repeatAct->setTag(1);
		m_selectSpr->runAction(repeatAct);
 		this->addChild(m_selectSpr);
		m_selectSpr->setVisible(false);

		m_finger = CCSprite::create("public/guide/shou_2.png");
		m_finger->setAnchorPoint(ccp(0,1));	
		m_finger->setPosition(ccp(VCENTER.x+10,VCENTER.y-65));
		m_finger->setVisible(false);
		this->addChild(m_finger,2);
		action = CCSequence::createWithTwoActions(CCScaleTo::create(0.35f,0.8f),CCScaleTo::create(0.4f,1.0f));
		repeatAct = CCRepeatForever::create(action);		
		repeatAct->setTag(1);
		m_finger->runAction(repeatAct);
		this->setVisible(true);

		m_openLay = (CLayout *)(m_ui->findWidgetById("open"));
		m_ui->removeChild(m_openLay);
		this->addChild(m_openLay);
		m_openLay->setVisible(false);

		scheduleUpdate();
		CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("BoneAnimation/8021.ExportJson");
		m_Armature = CCArmature::create("8021");
		m_Armature->setPosition(ccp(300, 150));
		m_Armature->setScale(0.6f);
		m_Armature->getAnimation()->playWithIndex(0,-1,-1,1);
	    CLayout *one =(CLayout*)(m_ui->findWidgetById("one"));
		one->addChild(m_Armature);
		return true;
	}
	return false;
}

void CGuideLayer::onEnter()
{	
	this->setTouchPriority(-1000);
	BaseLayer::onEnter();
}

void CGuideLayer::onExit()
{
	BaseLayer::onExit();
}

void CGuideLayer::onClose(CCObject* pSender)
{

}

void CGuideLayer::setTipMoveToPosition(CCNode *node, GuideTask task, const CCPoint& pos, CCRect rect /*= CCRectZero*/)
{
	m_ui->setVisible(false);
	m_rect = rect;	
	m_clickNode = node;
	if (m_selectSpr->getActionByTag(2)&&!m_selectSpr->getActionByTag(2)->isDone())
	{
		return;
	}
	addStencil(task, rect, pos);
	if (CGuideManager::getInstance()->getCurrTask()==1&&CGuideManager::getInstance()->getCurrStep()==7)
	{
		m_finger->setVisible(false);
	}
	else  m_finger->setVisible(true);
	//m_selectSpr->setVisible(true);
	m_selectSpr->stopActionByTag(1);
	CCPoint point = this->convertToNodeSpace(pos);	
	CCPoint fingerPos = this->convertToNodeSpace(ccpAdd(m_finger->getPosition(),ccpSub(point, m_selectSpr->getPosition())));

	m_selectSpr->setPosition(point);
	CCActionInterval *action = nullptr;
// 	if (task.step==4&&CGuideManager::getInstance()->getCurrTask()==2)
// 	{
// 		action = CCSequence::createWithTwoActions(CCScaleTo::create(0.25f,0.5f),CCScaleTo::create(0.35f,0.7f));
// 	}
// 	else

	action = CCSequence::createWithTwoActions(CCScaleTo::create(0.25f,0.96f),CCScaleTo::create(0.35f,1.0f));

	
	CCAction *repeatAct = CCRepeatForever::create(action);	
	repeatAct->setTag(1);
	m_selectSpr->runAction(repeatAct);
// 	CCSequence *seque = CCSequence::createWithTwoActions(move, CCCallFuncN::create(this,callfuncN_selector(CGuideLayer::recoverAction)));
// 	seque->setTag(2);
// 	m_selectSpr->runAction(seque);

	/*m_stencil->runAction(CCMoveTo::create(0.0015*distance, point));*/
	//m_stencil->setPosition(point);
	m_finger->stopActionByTag(1);
	float fdistance = ccpDistance(m_finger->getPosition(),fingerPos);
	CCMoveTo* move = CCMoveTo::create(0.001*fdistance, point);
	CCSequence *seque = CCSequence::createWithTwoActions(move, CCCallFuncN::create(this,callfuncN_selector(CGuideLayer::recoverFingerAction)));
	seque->setTag(2);
	m_finger->runAction(seque);
}

void CGuideLayer::setTipPosition(const CCPoint& pos)
{
	CCPoint point = this->convertToNodeSpace(pos);
	m_selectSpr->setPosition(point);
}

void CGuideLayer::recoverAction(CCNode *node)
{
// 	CCActionInterval *action = CCSequence::createWithTwoActions(CCScaleTo::create(0.25f,0.98f),CCScaleTo::create(0.35f,1.0f));
// 	CCAction *repeatAct = CCRepeatForever::create(action);	
// 	repeatAct->setTag(1);
// 	m_selectSpr->runAction(repeatAct);
}

void CGuideLayer::recoverFingerAction(CCNode *node)
{
	CCActionInterval *action = CCSequence::createWithTwoActions(CCScaleTo::create(0.35f,0.8f),CCScaleTo::create(0.4f,1.0f));
	CCAction *repeatAct = CCRepeatForever::create(action);		
	repeatAct->setTag(1);
	m_finger->runAction(repeatAct);
}

bool CGuideLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	if (this->isVisible())
	{	
		//if(m_rect.containsPoint(this->convertToWorldSpace(pTouch->getLocation())))
		if (m_ui->isVisible())
		{
			return true;
		}
 		else
		{
			CCNode* pNode = getClickBoundNode();
			if(pNode)
			{
				if(pNode->boundingBox().containsPoint(this->convertToWorldSpace(pTouch->getLocation())))
				{
					return true;
				}
			}
		}
	}
	return true;
}

void CGuideLayer::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
	if (m_ui->isVisible())
	{
		GuideTask *task = CGuideManager::getInstance()->getGuideTaskData();
		task->isFinish = true;
		CGuideManager::getInstance()->nextStep();
	}
	else if((m_finger&&m_finger->isVisible())||(CGuideManager::getInstance()->getCurrTask()==1&&CGuideManager::getInstance()->getCurrStep()==7))
	{
		CCNode* pNode = getClickBoundNode();
		if(pNode)
		{
			if(pNode->boundingBox().containsPoint(this->convertToWorldSpace(pTouch->getLocation())))
			{
				CButton *btn = dynamic_cast<CButton*>(m_clickNode);
				GuideTask *task = CGuideManager::getInstance()->getGuideTaskData();

				// 		if (CGuideManager::getInstance()->getCurrTask()==1&&CGuideManager::getInstance()->getCurrStep()>=3)
				// 		{
				// 			RecruitResult *result = (RecruitResult*)(LayerManager::instance()->getLayer("RecruitResult"));
				// 			if (result)
				// 			{
				// 				result->guideres(btn);   
				// 			}
				// 		}
				// 		else if (CGuideManager::getInstance()->getCurrTask()==2&&CGuideManager::getInstance()->getCurrStep()==3)
				// 		{
				// 			CSelectHero *selectHero = (CSelectHero *)(LayerManager::instance()->getLayer("CSelectHero"));
				// 			CGridPageView *pageView = (CGridPageView*)selectHero->findWidgetById("scroll");
				// 			CGridPageViewCell *cell = (CGridPageViewCell *)(pageView->cellAtIndex(0)->getChildByTag(1));
				// 			CButton *btn = (CButton*)cell->getChildByTag(1);
				// 			btn->executeClickHandler(btn);
				// 			/*selectHero->guide();*/
				// 		}
				// 		if (CGuideManager::getInstance()->getCurrTask()==2&&CGuideManager::getInstance()->getCurrStep()==2)
				// 		{
				// // 			CArmyLayer *armyLayer = (CArmyLayer *)(LayerManager::instance()->getLayer("CArmyLayer"));
				// // 			armyLayer->guide();
				// 		}
				// 		else 
				if (m_clickNode)
				{
					CImageView* image = dynamic_cast<CImageView*>(m_clickNode);
					if (btn)
					{
						btn->executeClickHandler(btn);
					}
					else if (image)
					{
						image->executeClickHandler(image);
					}
					else if (task->type=="radio")
					{
						CRadioButton *btn = dynamic_cast<CRadioButton*>(m_clickNode);
						if (btn)
						{
							btn->executeCheckHandler(btn,true);
						}
					}
					else
					{
						CImageViewScale9 * imgScale = dynamic_cast<CImageViewScale9*>(m_clickNode);
						if (imgScale)
						{			
							imgScale->executeClickHandler(imgScale);
						}
					}
				}

				if (CGuideManager::getInstance()->isAutoNextStep())
				{
					this->scheduleOnce(schedule_selector(CGuideLayer::delayNextStep),0.1f);
				}
				else if (task->autoJump>0)
				{
					//this->scheduleOnce(schedule_selector(CGuideLayer::delayNextStep),task->autoJump);
					this->setVisible(false);
				}
				else if (CGuideManager::getInstance()->getCurrTask()==1&&(task->step==6||task->step==7))
				{
					this->setVisible(false);
					task->isFinish = false;
				}
				else
				{
					task->isFinish = true;
				}
				m_clickNode = nullptr;
				m_finger->setVisible(false);
				m_selectSpr->setVisible(false);
				m_pClipNode->removeChildByTag(ClickNodeShadowTag);
				m_pClipNode->getStencil()->setVisible(false);
			}
		}
	}
}

void CGuideLayer::runDelayStep()
{
	GuideTask *task = CGuideManager::getInstance()->getGuideTaskData();
	this->scheduleOnce(schedule_selector(CGuideLayer::delayNextStep),task->autoJump);
}

void CGuideLayer::delayNextStep(float dt)
{
	this->setVisible(true);
	GuideTask *task = CGuideManager::getInstance()->getGuideTaskData();
	task->isFinish = true;
	CGuideManager::getInstance()->nextStep();
	this->unschedule(schedule_selector(CGuideLayer::delayNextStep));
}

void CGuideLayer::addStencil(GuideTask &task, CCRect &rect, const CCPoint& pos)
{
	CCPoint po = m_selectSpr->getPosition();
	this->removeChild(m_selectSpr);
	m_selectSpr = CCSprite::create(task.name.c_str());
	m_selectSpr->setPosition(po);
	m_selectSpr->setOpacity(0);
	this->addChild(m_selectSpr);

	setIsShowBlack(false);
	m_pClipNode->setVisible(true);

	CCNode* pStencil = nullptr;
	if (m_clickNode)
	{
		if (dynamic_cast<CImageView*>(m_clickNode))
		{
			pStencil = UICloneMgr::cloneImageView((CImageView*)m_clickNode);
		}
		else if(dynamic_cast<CButton*>(m_clickNode))
		{
			pStencil = UICloneMgr::cloneButton((CButton*)m_clickNode);
		}
		else if(dynamic_cast<CRadioButton*>(m_clickNode))
		{
			pStencil = UICloneMgr::cloneRadioBtn((CRadioButton*)m_clickNode);
		}
		else if(dynamic_cast<CImageViewScale9*>(m_clickNode))
		{
			pStencil = UICloneMgr::cloneImageViewScale9((CImageViewScale9*)m_clickNode);
		}
		else if(dynamic_cast<CLayout*>(m_clickNode))
		{
			pStencil = UICloneMgr::cloneLayout((CLayout*)m_clickNode);
		}
		else if(dynamic_cast<CCheckBox*>(m_clickNode))
		{
			pStencil = UICloneMgr::cloneCheckBox((CCheckBox*)m_clickNode);
		}
	}
	if(pStencil)
	{
		switch (task.nodeClickType)
		{
		case GuideNodeClipWithStencil:
			{
				CCClippingNode *clipNode = (CCClippingNode*)this->getChildByTag(100);
				clipNode->removeChildByTag(ClickNodeShadowTag);
				clipNode->setAlphaThreshold(0.05f);	//设置以后按非透明像素裁切，不设置就是矩形区域
				clipNode->setStencil(pStencil);
				pStencil->setVisible(true);
			}
			break;
		case GuideNodeClipWithZorder:
			{
				CCClippingNode *clipNode = (CCClippingNode*)this->getChildByTag(100);
				clipNode->removeChildByTag(ClickNodeShadowTag);
				pStencil->setTag(ClickNodeShadowTag);
				clipNode->addChild(pStencil);
				clipNode->getStencil()->setVisible(false);
			}
			break;
		case GuideNodeClipWithStencilNoAlpha:
			{
				CCClippingNode *clipNode = (CCClippingNode*)this->getChildByTag(100);
				clipNode->removeChildByTag(ClickNodeShadowTag);
				clipNode->setAlphaThreshold(1);
				clipNode->setStencil(pStencil);
				pStencil->setVisible(true);
			}
			break;
		default:
			break;
		}
	}
}

void CGuideLayer::addStoryGuide(const char* text1, const char* text2, int tag/*=0*/)
{
     m_ui->setVisible(true);
	 m_finger->setVisible(false);
	 m_selectSpr->setVisible(false);
	 setIsShowBlack(true);
	 m_pClipNode->setVisible(false);

	 CLayout *one =(CLayout*)(m_ui->findWidgetById("one"));
	 CLayout *two =(CLayout*)(m_ui->findWidgetById("two"));
	 CLabel *textLab1 = (CLabel*)m_ui->findWidgetById("text1");
	 CLabel *textLab2 = (CLabel*)m_ui->findWidgetById("text2");
	 CLabel *textLab3 = (CLabel*)m_ui->findWidgetById("text3");
	 one->setVisible(tag==0);
	 two->setVisible(tag==1);
	 if (tag==0)
	 {
		 if (strcmp(text1,"")!=0)
		 {
			 textLab1->setString(text1);
		 }
	 }
	 else
	 {
		 textLab3->setString(text1);
	 }
	 textLab2->setString(text2);
// 	 CCNode *npc = (CCNode*)(m_ui->findWidgetById("npc"));
// 	 if (!npc->getActionByTag(100))
// 	 { 
// 		 CCAction *action = CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.4f,ccp(0,25)),CCMoveBy::create(0.4f,ccp(0,-25))));
// 		 action->setTag(100);
// 		 npc->runAction(action);
// 	 }
}

void CGuideLayer::addOpenNewFunction(CCNode *node, GuideTask task, const CCPoint& pos, CCRect rect)
{
	m_ui->setVisible(false);
	m_rect = rect;	
	m_clickNode = node;
	m_openLay->setVisible(true);
	m_destPos = this->convertToNodeSpace(pos);	
	CCSprite *roll = (CCSprite*)(m_openLay->findWidgetById("roll"));
	CCAction *action = CCSequence::create(CCRepeat::create(CCRotateBy::create(0.5f,360),2),CCCallFuncN::create(this,callfuncN_selector(CGuideLayer::rollAnimationCall)),CCRemoveSelf::create(),nullptr);
	roll->runAction(action);
}

void CGuideLayer::rollAnimationCall(CCNode *node)
{
	CCSprite *active = (CCSprite*)(m_openLay->findWidgetById("active"));
	active->removeFromParent();
	CCSprite *key = (CCSprite*)(m_openLay->findWidgetById("key"));
	CCMoveTo *move = CCMoveTo::create(0.4f,m_destPos);
	key->runAction(CCSequence::create(move,CCCallFuncN::create(this,callfuncN_selector(CGuideLayer::runSelectAnim)),CCRemoveSelf::create(),nullptr));
}

void CGuideLayer::runSelectAnim(CCNode *node)
{
	CCAnimation *anim = AnimationManager::sharedAction()->getAnimation("7032");
	anim->setDelayPerUnit(0.2f);
	CCSprite *spr = CCSprite::create("skill/7032.png");
	spr->setPosition(m_destPos);
	m_openLay->addChild(spr);
	spr->runAction(CCSequence::create(CCAnimate::create(anim),CCCallFuncN::create(this,callfuncN_selector(CGuideLayer::callNextGuide)),CCRemoveSelf::create(),nullptr));
}

void CGuideLayer::callNextGuide(CCNode *node)
{
	addStencil(*CGuideManager::getInstance()->getGuideTaskData(), m_rect, m_destPos);
	m_finger->setVisible(true);
	//m_selectSpr->setVisible(true);
	m_selectSpr->stopActionByTag(1);
	CCPoint fingerPos = this->convertToNodeSpace(ccpAdd(m_finger->getPosition(),ccpSub(m_destPos, m_selectSpr->getPosition())));

	m_selectSpr->setPosition(m_destPos);
	CCActionInterval *action = nullptr;
	action = CCSequence::createWithTwoActions(CCScaleTo::create(0.25f,0.96f),CCScaleTo::create(0.35f,1.0f));
	CCAction *repeatAct = CCRepeatForever::create(action);	
	repeatAct->setTag(1);
	m_selectSpr->runAction(repeatAct);

	if (fingerPos.y<0)
	{
		fingerPos.y =90;
	}
	m_finger->setPosition(fingerPos);

	int open = CGuideManager::getInstance()->getOpenFunction();
	CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_FUNCTIONOPEN,open,nullptr,nullptr);

	if (CGuideManager::getInstance()->getCurrTask()==19)
	{
		CGuideManager::getInstance()->getGuideTaskData()->isFinish = true;
		CGuideManager::getInstance()->nextStep();
	}
}

void CGuideLayer::update( float dt )
{
	if(m_pClipNode->isVisible() && m_clickNode)
	{
		CCNode* pStencil = m_pClipNode->getStencil();
		if(pStencil->isVisible())
		{
			CCPoint pPos = m_clickNode->getParent()->convertToWorldSpace(m_clickNode->getPosition());
			pStencil->setPosition(pPos);
			pStencil->setScaleX(m_clickNode->getScaleX()-5.0f/m_clickNode->getContentSize().width);
			pStencil->setScaleY(m_clickNode->getScaleY()-5.0f/m_clickNode->getContentSize().height);
		}
		else
		{
			CCNode* pChild = m_pClipNode->getChildByTag(ClickNodeShadowTag);
			if(pChild)
			{
				CCPoint pPos = m_clickNode->getParent()->convertToWorldSpace(m_clickNode->getPosition());
				pChild->setPosition(pPos);
				pChild->setScaleX(m_clickNode->getScaleX()-5.0f/m_clickNode->getContentSize().width);
				pChild->setScaleY(m_clickNode->getScaleY()-5.0f/m_clickNode->getContentSize().height);
			}
		}
	}
}

CCNode* CGuideLayer::getClickBoundNode()
{
	if(m_pClipNode->isVisible() && m_clickNode)
	{
		CCNode* pChild = m_pClipNode->getChildByTag(ClickNodeShadowTag);
		if(pChild)
		{
			return pChild;
		}
		else
		{
			return m_pClipNode->getStencil();
		}
	}
	return nullptr;
}
