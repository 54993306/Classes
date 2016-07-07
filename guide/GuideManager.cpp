#include "GuideManager.h"
#include "model/DataCenter.h"
#include "mainCity/mainScene.h"
#include "scene/CPopTip.h"
#include "mainCity/mainCityBuild.h"

CGuideManager* CGuideManager::m_instance = nullptr;

CGuideManager* CGuideManager::getInstance()
{
	if (!m_instance)
	{
		m_instance = new CGuideManager;
	}
	return m_instance;
}

CGuideManager::CGuideManager():
	m_guideTask(0),m_guideStep(1),m_isRunGuiding(false),m_guideLayer(nullptr),m_isBeginGuide(true)
{
	m_openFuncMap[4] = 12;
	m_openFuncMap[5] = 4;
	m_openFuncMap[3] = 22;
	m_openFuncMap[6] = 21;
	m_openFuncMap[7] = 6;
	m_openFuncMap[8] = 15;
	m_openFuncMap[18] = 24;
	m_openFuncMap[19] = 16;
}

void CGuideManager::setTaskStep(int task, int step)
{
	if (!m_isRunGuiding)
	{
		m_guideTask = task;
		m_guideStep = step;
	}
}

const CCPoint& CGuideManager::geStepWidgetPosition()
{
	return m_stepPos;
}

void CGuideManager::enterGuide()
{
	GuideTask *task = m_guideData.getTaskByStep(m_guideTask,m_guideStep);
	if (m_guideTask>25||!task||m_isRunGuiding)
	{
		return;
	}
	if (m_guideTask==2&&LayerManager::instance()->getLayer("CTollgateLayer"))
	{
// 		m_guideStep+=3;
// 		task = m_guideData.getTaskByStep(m_guideTask,m_guideStep);
	}
// 	while (task&&task->isFinish)
// 	{
// 		m_guideStep++;
// 		task = m_guideData.getTaskByStep(m_guideTask,m_guideStep);
// 	}
	CCLOG("enter %d, %d",m_guideTask,m_guideStep);

	if (m_guideStep>m_guideData.getGuideTasks(m_guideTask).size())
	{
		return;
	}
	m_guideLayer = CGuideLayer::create();
	CCScene *mainScene = dynamic_cast<CMainScene*>(CSceneManager::sharedSceneManager()->getCurrScene());
	if (mainScene)
	{
		mainScene->addChild(m_guideLayer,100,100001);
	}
// 	CMainScene *mainScene = (CMainScene*)CSceneManager::sharedSceneManager()->getScene("CMainScene");
	
	/*CCDirector::sharedDirector()->getRunningScene()->addChild();*/
	if (task->type=="story")
	{
		m_isRunGuiding = true;
		m_guideLayer->addStoryGuide(task->text1.c_str(),task->text2.c_str(),task->tag);
	}
	else
	{
		CCNode *widget = getStepNode(*task);
		if (!widget)
		{
// 			if (task->className=="CTollgateLayer"&&task->widgetId=="close"&&m_isBeginGuide)
// 			{
// 				task->isFinish = true;
// 				m_isRunGuiding = true;
// 				nextStep();
// 				return;
// 			}
// 			else
			{
				exitGuide();
				return;
			}
		}
		CScrollView *scroll = dynamic_cast<CScrollView*>(widget->getParent()->getParent());
		CGridPageView *pageView = dynamic_cast<CGridPageView*>(widget->getParent()->getParent()->getParent()->getParent());
	
		if (scroll)
		{
			m_stepPos = widget->getParent()->convertToWorldSpace(ccp(widget->getPositionX()+widget->getContentSize().width/2
				,widget->getPositionY()+widget->getContentSize().height/2));
		}
		if (pageView)
		{
			m_stepPos = widget->getParent()->convertToWorldSpace(ccp(widget->getPositionX()+widget->getContentSize().width/2
				,widget->getPositionY()+widget->getContentSize().height/2));
		}
		else
		{
			m_stepPos = widget->getParent()->convertToWorldSpace(widget->getPosition());
		}
		m_isRunGuiding = true;
		if ((m_guideTask==7||m_guideTask==8)&&m_guideStep==2)
		{
			CMainScene *scene = (CMainScene*)(CSceneManager::sharedSceneManager()->getRunningScene());
			scene->getCityBuildLayer()->moveFrontByPos(task->offset);
			m_stepPos = widget->getParent()->convertToWorldSpace(widget->getPosition());
			m_guideLayer->setTipMoveToPosition(widget,*task,m_stepPos,widget->boundingBox());
		}
		else if (task->type=="open")
		{
			m_guideLayer->addOpenNewFunction(widget,*task,ccpAdd(m_stepPos,task->offset),widget->boundingBox());
		}
		else
		{
			m_guideLayer->setTipMoveToPosition(widget,*task,ccpAdd(m_stepPos,task->offset),widget->boundingBox());
		}
	}
}

void CGuideManager::nextStep()
{
	GuideTask *task = CGuideManager::getInstance()->getGuideTaskData();
	if (m_guideTask==2&&m_guideStep==8)
	{
	//	m_guideLayer->runDelayStep();
// 		m_guideLayer->setVisible(false);
// 		exitGuide();
//  		return;
	}
    if (!task->isFinish) return;
	m_guideStep++;

	CCLOG("enter %d, %d",m_guideTask,m_guideStep);
	
	if (m_guideStep<=m_guideData.getGuideTasks(m_guideTask).size()&&m_guideLayer)
	{
		m_guideLayer->setVisible(true);
		GuideTask *task = m_guideData.getTaskByStep(m_guideTask,m_guideStep);

		if (task->type=="story")
		{
			m_guideLayer->addStoryGuide(task->text1.c_str(),task->text2.c_str(),task->tag);
		}
		else if (m_guideTask==16&&(/*m_guideStep==3||*/m_guideStep==3))
		{
			CPopTip  *popTip = dynamic_cast<CPopTip *>(LayerManager::instance()->getLayer("CPopTip"));
			if (popTip)
			{
				CCNode *widget = popTip->getConfirmButton();
				m_stepPos = widget->getParent()->convertToWorldSpace(widget->getPosition());
				m_guideLayer->setTipMoveToPosition(widget,*task,ccpAdd(m_stepPos,task->offset),widget->boundingBox());
			}
		}
		else
		{
			CCNode *widget = getStepNode(*task);
			if (!widget)
			{
				if (task->className=="CTollgateLayer"&&task->widgetId=="close")
				{
					task->isFinish = true;
					nextStep();
					return;
				}
				else
				{
					exitGuide();
					return;
				}
				
			}
	// 		m_stepPos = widget->getParent()->getParent()->convertToWorldSpace(widget->getPosition());
			CListView *scroll = dynamic_cast<CListView*>(widget->getParent()->getParent());
			if (scroll)
			{
				m_stepPos = widget->getParent()->convertToWorldSpace(ccp(widget->getPositionX()+widget->getContentSize().width/2
					,widget->getPositionY()+widget->getContentSize().height/2));
			}
			else
			{
				m_stepPos = widget->getParent()->convertToWorldSpace(widget->getPosition());
			}

			if ((m_guideTask==7&&m_guideStep==2)||((m_guideTask==19)&&m_guideStep==3))
			{
				CMainScene *scene = (CMainScene*)(CSceneManager::sharedSceneManager()->getRunningScene());
				scene->getCityBuildLayer()->moveFrontByPos(task->offset);
				m_stepPos = widget->getParent()->convertToWorldSpace(widget->getPosition());
				if (task->type=="open")
				{
					m_guideLayer->addOpenNewFunction(widget,*task,m_stepPos,widget->boundingBox());
				}
				else
				{
					m_guideLayer->setTipMoveToPosition(widget,*task,m_stepPos,widget->boundingBox());
				}

			}
			else if (task->type=="open")
			{
				m_guideLayer->addOpenNewFunction(widget,*task,ccpAdd(m_stepPos,task->offset),widget->boundingBox());
			}
			else
			{
				m_guideLayer->setTipMoveToPosition(widget,*task,ccpAdd(m_stepPos,task->offset),widget->boundingBox());
			}
		}

	}
	else
	{
		exitGuide();
	}

}

void CGuideManager::exitGuide()
{
	m_isRunGuiding = false;
    CMainScene *mainScene = dynamic_cast<CMainScene*>(CSceneManager::sharedSceneManager()->getCurrScene());
	if (mainScene)
	{	
		mainScene->removeChildByTag(100001);
	}
	m_guideLayer = nullptr;
	UserData *user = DataCenter::sharedData()->getUser()->getUserData();
	if (m_guideTask>0&&user->getNewStep()!=m_guideTask)
	{
// 		user->setNewStep(user->getNewStep);
		CGuideManager::getInstance()->setTaskStep(user->getNewStep(),1);
		CGuideManager::getInstance()->enterGuide();
	}
	else
	{
		//user->setNewStep(0);
	}
	setJumpStep(false);

}

CCNode* CGuideManager::getStepNode(const GuideTask &task)
{
	BaseLayer *layer = dynamic_cast<BaseLayer*>(LayerManager::instance()->getLayer(task.className.c_str()));
	if (layer)
	{
		if (task.className1!="")
		{
			BaseLayer* pWidget = nullptr;
			for (int i = 0; i < layer->getChildrenCount(); i++)
			{
				CCNode * pNode = dynamic_cast<CCNode*>(layer->getChildren()->objectAtIndex(i));
				pWidget = dynamic_cast<BaseLayer*>(pNode);
				if (pWidget&&pWidget->getName() == task.className1)
				{
					return findStepWidget(pWidget,task);
				}
			}
		}
		return findStepWidget(layer, task);
	}
	else
	{
		CScene *scene = (CScene*)CSceneManager::sharedSceneManager()->getCurrScene();
		BaseLayer* pWidget = nullptr;
		for (int i = 0; i < scene->getChildrenCount(); i++)
		{
			CCNode * pNode = dynamic_cast<CCNode*>(scene->getChildren()->objectAtIndex(i));
			pWidget = dynamic_cast<BaseLayer*>(pNode);
			if (pWidget&&pWidget->getName() == task.className)
			{
				break;
			}
		}
		if (pWidget&&pWidget->findWidgetById("UI"))
		{
			CCNode *res= findStepWidget(pWidget, task);
			if (res)
			{
				return res;
			}
		}
		if (task.className=="CMainCityUI")
		{
			CMainCityUI *mainCity = (CMainCityUI*)((CMainScene*)scene)->getCityBuildLayer()->getChildByTag(20010);
			return findStepWidget(mainCity,task);
		}
		CWidgetWindow *widwin = (CWidgetWindow*)((CMainScene*)scene)->getCityBuildLayer()->getChildByTag(10000);
		if (widwin)
		{
			return findStepWidget((BaseLayer*)widwin,task);
		}
		return nullptr;
	}
}

CCNode* CGuideManager::findStepWidget(BaseLayer * layer, const GuideTask &task)
{
	CLayout *ui = dynamic_cast<CLayout*>(layer->findWidgetById("UI"));
	if (task.widgetId!= "")
	{
		if (task.type=="btn"||task.type=="radio"||task.type=="open")
		{
			if (task.tag>0&&ui->findWidgetById(task.widgetId.c_str()))
			{
				return ((CCNode*)ui->findWidgetById(task.widgetId.c_str()))->getChildByTag(task.tag);
			}
			return (CCNode*)ui->findWidgetById(task.widgetId.c_str());
		}
		else if (task.type=="table")
		{
			CTableView *table = (CTableView*)ui->findWidgetById(task.widgetId.c_str());
			if (!table)
			{
				return nullptr;
			}
			CCNode *child = table->cellAtIndex(task.tag);
			if (table->getChildrenCount()>0&&task.tag1==0)
			{
				return child;
			}
			if (child)
			{
				return table->cellAtIndex(task.tag)->getChildByTag(task.tag1);
			}
			return nullptr;
		}
		else if (task.type == "grid")
		{
			CGridView *gridView = (CGridView*)ui->findWidgetById(task.widgetId.c_str());
			if (!gridView)
			{
				return nullptr;
			}
			CCNode *child =  gridView->cellAtIndex(task.tag);
			if (gridView->getChildrenCount()>0&&task.tag1==0)
			{
				return child;
			}
			if (child)
			{
				return gridView->cellAtIndex(task.tag)->getChildByTag(task.tag1);
			}
			return nullptr;
		}
		else if (task.type =="page")
		{
			CGridPageView *pageView = (CGridPageView*)ui->findWidgetById(task.widgetId.c_str());
			if (!pageView)
			{
				return nullptr;
			}
			CCNode *page = pageView->cellAtIndex(0);
			CCNode *child = page->getChildByTag(task.tag);
			if (pageView->getChildrenCount()>0&&task.tag1==0)
			{
				return child;
			}
			if (child)
			{
				return child->getChildByTag(task.tag1);
			}
			return nullptr;
		}
		else if (task.type == "list")
		{
			CListView *listView = (CListView*)ui->findWidgetById(task.widgetId.c_str());
			if (!listView)
			{
				return nullptr;
			}
			CCNode *child =  listView->getNodeAtIndex(task.tag);
			if (listView&&listView->getChildrenCount()>0&&task.tag1==0)
			{
				return child;
			}
			if (child)
			{
				return (CCNode*)((CLayout*)child)->findWidgetById("bgBtn");
			}
			return nullptr;
		}
	}
	else if (task.tag>0)
	{
		if (task.type=="btn"||task.type=="radio"||task.type=="open")
		{
			if (task.tag1>0&&ui->getChildByTag(task.tag))
			{
				return (CCNode*)ui->getChildByTag(task.tag)->getChildByTag(task.tag1);
			}
			return (CCNode*)ui->getChildByTag(task.tag);
		}
// 		else if (task.type == "table")
// 		{
// 			CTableView *table = NULL;
// 			if (task.tag1>0)
// 			{
// 				table = (CTableView*)ui->getChildByTag(task.tag)->getChildByTag(task.tag1);
// 			}
// 			else
// 			{
// 				table = (CTableView*)ui->getChildByTag(task.tag);
// 			}
// 			if (table->getChildrenCount()>0)
// 			{
// 				return table->cellAtIndex(task.tag);
// 			}
// 			return table;
// 		}
// 		else if (task.type == "grid")
// 		{
// 			CGridView *gridView = nullptr;
// 			if (task.tag1>0)
// 			{
// 				gridView = (CGridView*)ui->getChildByTag(task.tag)->getChildByTag(task.tag1);
// 			}
// 			else
// 			{
// 				gridView = (CGridView*)ui->getChildByTag(task.tag);
// 			}
// 			if (gridView->getChildrenCount()>0)
// 			{
// 				return gridView->cellAtIndex(task.tag);
// 			}
// 			return gridView;
// 		}
// 		else if (task.type == "list")
// 		{
// 			CListView *listView = nullptr;
// 			if (task.tag1>0)
// 			{
// 				listView = (CListView*)ui->getChildByTag(task.tag)->getChildByTag(task.tag1);
// 			}
// 			else
// 			{
// 				listView = (CListView*)ui->getChildByTag(task.tag);
// 			}
// 			if (listView->getChildrenCount()>0)
// 			{
// 				return listView->getNodeAtIndex(task.tag);
// 			}
// 		}
	}
	return nullptr;
}

bool CGuideManager::getIsRunGuide()
{
	return m_isRunGuiding;
}

bool CGuideManager::isAutoNextStep()
{
	GuideTask *task = m_guideData.getTaskByStep(m_guideTask,m_guideStep);
	if (task->autoJump==1)
	{
		return true;
	}
	return false;
}

int CGuideManager::getCurrTask()
{
	return m_guideTask;
}

int CGuideManager::getCurrStep()
{
	return m_guideStep;
}

GuideTask* CGuideManager::getGuideTaskData()
{
	GuideTask *task = m_guideData.getTaskByStep(m_guideTask,m_guideStep);
	return task;
}

int CGuideManager::getOpenFunction()
{
	map<int,int>::iterator iter = m_openFuncMap.find(m_guideTask);
	if (iter!= m_openFuncMap.end())
	{
		return iter->second;
	}
	return 0;
}

void CGuideManager::setJumpStep(bool isJump)
{
	m_isBeginGuide = isJump;
}


