/******************************************************************************
文件名: scenemanager.cpp
编写者: csdn_viva@foxmail.com ccbase1.0
编写日期: 2013-5-2
功能描述: 场景管理器，提供对场景生命周期进行操作的API
******************************************************************************/
#include <vector>
#include <map>
#include <list>
using namespace std;

#include "base.h"
#include "scenemanager.h"
#include "guide/GuideManager.h"
#include "common/CTransitionDelay.h"


CSceneManager* CSceneManager::m_pSceneManager = nullptr;
///单例 共享对象
CSceneManager* CSceneManager::sharedSceneManager()
{
	if( m_pSceneManager == NULL ) 
	{
		m_pSceneManager = new CSceneManager();
	}
	return m_pSceneManager;
}

///构造方法，开启帧函数。
CSceneManager::CSceneManager(void)
{
	CCDirector::sharedDirector()->setNotificationNode(this);
	this->pRunningBoxNode = new CCNode();
	this->pRunningBoxNode->init();
	this->pRunningBoxNode->setAnchorPoint(CCPointZero);
	this->pRunningBoxNode->setPosition(CCPointZero);
	this->pRunningBoxNode->setContentSize(CCDirector::sharedDirector()->getWinSize());
	this->pRunningBoxNode->onEnter();
}
///析构，关闭帧函数，释放资源
CSceneManager::~CSceneManager(void)
{
	//移除帧定时器内所有事件
	this->removeAllLooper();
	//删除所有不在运行栈中，但是在缓存里的场景
	this->removeAllCachedScene();
	//释放所有不在运行弹出框组里 但是在缓存里的弹出框
	this->removeAllCachedPopupBox();

	m_MessageQueue.clear();
	m_mSceneCacheMap.clear();
	m_mSceneFunMap.clear();
	m_mPopupBoxCacheMap.clear();
	m_mPopupBoxFunMap.clear();

	if(this->pRunningBoxNode) {
		this->pRunningBoxNode->onExit();
		delete this->pRunningBoxNode;
	}
}
///帧回调函数
void CSceneManager::visit()
{
	//消息队列的循环
	while(!m_MessageQueue.empty())
	{
		TMessage& tMessage = m_MessageQueue.front();
		//给指定场景发消息
		if(tMessage.pScene != NULL)
		{
			//tMessage.pScene->onMessage(tMessage.nMsg, tMessage.wParam, tMessage.lParam);
			msgHandlerMap::iterator iter = m_msgHandlerMap.find(tMessage.name);
			if (iter!=m_msgHandlerMap.end())
			{
 				//(iter->second.first->*(iter->second.second))(tMessage);
				vector<std::pair<CCObject*,handlerMsg>>  &handlerVec = iter->second;
				for (int i=0; i<handlerVec.size();i++)
					(handlerVec.at(i).first->*(handlerVec.at(i).second))(tMessage);
				//数组容器用完要释放
				if(tMessage.wParam != NULL)
					delete tMessage.wParam;
				//if (tMessage.lParam !=NULL)
				//	delete tMessage.lParam;
			}
			m_MessageQueue.pop_front();
		}
	}
	//循环帧事件处理器
	if(!m_vLooperList.empty())
	{
		//查找所有的事件处理器，并分发事件
		for(vector<CLooper*>::size_type idx = 0; idx < m_vLooperList.size(); idx++)
			m_vLooperList[idx]->loop();
	}
	//绘制弹出框的根节点
	if(pRunningBoxNode)
		pRunningBoxNode->visit();
}
///给栈顶的场景发消息，lParam指针要手动释放
void CSceneManager::PostMessageA(const char *name,int nMsg, ARRAY wParam, PARAM lParam,int intParam)
{
	CScene *pRunningScene = (CScene*) CSceneManager::sharedSceneManager()->getCurrScene();
	TMessage tMessage;
	tMessage.name = name;
	tMessage.nMsg   = nMsg;
	tMessage.wParam = wParam;
	tMessage.lParam = lParam;
	tMessage.intParam = intParam;
	tMessage.pScene = pRunningScene;
	m_MessageQueue.push_back(tMessage);
}

void CSceneManager::PostMessageB(const char *name,char *strArg , ARRAY wParam, PARAM lParam,int intParam)
{
	CScene *pRunningScene = (CScene*) CSceneManager::sharedSceneManager()->getCurrScene();
	TMessage tMessage;
	tMessage.name = name;
	tMessage.strMsg = strArg;
	tMessage.wParam = wParam;
	tMessage.lParam = lParam;
	tMessage.intParam = intParam;
	tMessage.pScene = pRunningScene;
	m_MessageQueue.push_back(tMessage);
}
//消息处理对象注册
void CSceneManager::addMsgObserver(const char *msgName, CCObject* obj, handlerMsg func)
{
	std::pair<CCObject*,handlerMsg> handMsg(obj,func);
	/*m_msgHandlerMap[msgName] = handMsg;*/

	msgHandlerMap::iterator iter = m_msgHandlerMap.find(msgName);
	if (iter!=m_msgHandlerMap.end())								//一个消息可以处理几个函数
	{
		iter->second.push_back(handMsg);
	}else{
		vector<std::pair<CCObject*,handlerMsg>> handlerVec;
		handlerVec.push_back(handMsg);
		m_msgHandlerMap[msgName] = handlerVec;
	}
}
//移除消息处理
void CSceneManager::removeMsgObserver(const char *msgName,CCObject *handler)
{
	msgHandlerMap::iterator iter = m_msgHandlerMap.find(msgName);

	if (iter!=m_msgHandlerMap.end())
	{
		vector<std::pair<CCObject*,handlerMsg>>::iterator it = iter->second.begin();
		for (;it!=iter->second.end(); it++)
		{
			if (it->first == handler)
			{
				iter->second.erase(it);
				break;
			}
		}
	}
}
///pLooper - 帧事件接收器，继承自CLooper,增加一个帧回调事件
void CSceneManager::addLooper(CLooper* pLooper)
{
	if(!m_vLooperList.empty())
	{
		for(vector<CLooper*>::iterator iter = m_vLooperList.begin(); iter != m_vLooperList.end(); iter++)
		{
			if((*iter) == pLooper)
				return;
		}
	}
	m_vLooperList.push_back(pLooper);
}
///pLooper - 帧事件接收器，继承自CLooper,移除一个帧回调事件
void CSceneManager::removeLooper(CLooper* pLooper)
{
	for(vector<CLooper*>::iterator iter = m_vLooperList.begin(); iter != m_vLooperList.end(); iter++)
	{
		if((*iter) == pLooper)
		{
			m_vLooperList.erase(iter);
			break;
		}
	}
}
///移除所有帧事件
void CSceneManager::removeAllLooper()
{
	m_vLooperList.clear();
}
///退出整个程序
void CSceneManager::exit()
{
	//移除帧定时器内所有事件
	this->removeAllLooper();
	//删除所有不在运行栈中，但是在缓存里的场景
	this->removeAllCachedScene();
	//释放所有不在运行弹出框组里 但是在缓存里的弹出框
	this->removeAllCachedPopupBox();

	m_MessageQueue.clear();
	m_mSceneCacheMap.clear();
    m_mSceneFunMap.clear();
	m_mPopupBoxCacheMap.clear();
	m_mPopupBoxFunMap.clear();

	CCDirector::sharedDirector()->setNotificationNode(NULL);
	//退出游戏
	CCDirector::sharedDirector()->end();
	//释放管理对象
	delete this;

#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	//exit();
#endif
}
///通过场景类名获得场景实例，从缓存池中取出，如果不存在于缓存池，就new
CScene* CSceneManager::getScene(const char* pClassName)
{
	map<string, CScene*>::iterator sciter = m_mSceneCacheMap.find(pClassName);
	//查看缓存列表，是否已经有场景实例
	if ( sciter != m_mSceneCacheMap.end() ) {
		m_pScene = sciter->second;
		return sciter->second;
	}
	//如果没有，就新new
	map<string, PFNCREATESCENE>::iterator fiter = m_mSceneFunMap.find(pClassName);
	if ( fiter != m_mSceneFunMap.end() ) 
	{
		//获得构造对象的函数指针
		PFNCREATESCENE pFunction = fiter->second;
		CScene *pScene = (*pFunction)();
		CCLOG("## [DEBUG] Create Scene [%s]", pClassName);	
		pScene->setClassName(pClassName);
		pScene->onCreate();
		pScene->autorelease();
		if(pScene->isCached()) 
		{
			m_mSceneCacheMap[pClassName] = pScene;
			pScene->retain();
		}
		m_pScene = pScene;
		return pScene;
	} else {
		CCLOG("## [ERROR] getScene cound't not find Scene by className=%s", pClassName);
		return NULL;
	}
}
///通过对话框类名获得对话框实例，从缓存池中取出，如果不存在于缓存池，就new
CPopupBox* CSceneManager::getPopupBox(const char* pClassName)
{
	map<string, CPopupBox*>::iterator pbiter = m_mPopupBoxCacheMap.find(pClassName);
	//查看缓存的弹出框组里，是否有
	if ( pbiter != m_mPopupBoxCacheMap.end() ) {
		return pbiter->second;
	}
	map<string, PFNCREATEPOPUPBOX>::iterator fiter = m_mPopupBoxFunMap.find(pClassName);
	if ( fiter != m_mPopupBoxFunMap.end() ) {
		PFNCREATEPOPUPBOX pFunction = fiter->second;
		CPopupBox *pBox = (*pFunction)();
		CCLOG("## [DEBUG] Create PopupBox [%s]", pClassName);
		pBox->setClassName(pClassName);
		pBox->onCreate();
		pBox->autorelease();
		if( pBox->isCached() ) {
			m_mPopupBoxCacheMap[pClassName] = pBox;
			pBox->retain();
		}
		return pBox;
	} else {
		CCLOG("## [ERROR] getPopupBox cound't not find Box by className=%s", pClassName);
		return NULL;
	}
}
///运行场景，只能调用一次
void CSceneManager::runScene(CCScene *pScene)
{
	CCDirector::sharedDirector()->runWithScene(pScene);
}
///替换当前栈顶的场景 出栈并压栈
void CSceneManager::replaceScene(CCScene *pScene, float fDelay)
{
	if (CGuideManager::getInstance()->getIsRunGuide())
		CGuideManager::getInstance()->exitGuide();
	m_pScene = dynamic_cast<CScene*>(pScene);

	if(fDelay==0)
	{
		CCDirector::sharedDirector()->replaceScene(pScene);
	}else{
		CCDirector::sharedDirector()->replaceScene(CTransitionDelay::create(fDelay, pScene));
	}
}
///增加一个场景到栈顶 压栈
void CSceneManager::pushScene(CCScene *pScene)
{
	CCDirector::sharedDirector()->pushScene(pScene);
}
///移除栈顶的场景 出栈
void CSceneManager::popScene()
{
	CCDirector::sharedDirector()->popScene();
}
///回到栈底的场景，连续出栈
void CSceneManager::popToRootScene()
{
	CCDirector::sharedDirector()->popToRootScene();
}
///打开对话框，可以同时打开多个
void CSceneManager::openPopupBox(const char* pClassName, const PARAM param)
{
	//弹出框
	CPopupBox *pRunningBox = NULL;
	//是否这个弹出框已经显示在节点上了
	if( pRunningBoxNode->getChildrenCount() > 0 ) {
		CCArray  *pArrs = pRunningBoxNode->getChildren();
		CCObject *pObject = NULL;
		CCARRAY_FOREACH( pArrs, pObject ) {
			CPopupBox *pBox = (CPopupBox*) pObject;
			if( pBox->getClassName() == pClassName ) {
				pRunningBox = pBox;
				break;
			}
		}
	}
	if( pRunningBox ) {
		pRunningBox->onOpen(param);
		return;
	} 
	CPopupBox *pBox = this->getPopupBox(pClassName);
	pBox->onOpen(param);
	this->pRunningBoxNode->addChild(pBox);
}
///关闭对话框
void CSceneManager::closePopupBox(const char* pClassName)
{
	//弹出框
	CPopupBox *pRunningBox = NULL;
	//这个要删除的弹出框是否在节点上
	if( pRunningBoxNode->getChildrenCount() > 0 ) {
		CCArray  *pArrs = pRunningBoxNode->getChildren();
		CCObject *pObject = NULL;
		CCARRAY_FOREACH( pArrs, pObject ) {
			CPopupBox *pBox = (CPopupBox*) pObject;
			if( pBox->getClassName() == pClassName ) {
				pRunningBox = pBox;
				break;
			}
		}
	}
	if( pRunningBox ) {
		this->pRunningBoxNode->removeChild(pRunningBox, true);
	}
}
///注册场景实例，通过类名映射
void CSceneManager::registeSceneClass(const char* pClassName, PFNCREATESCENE pFunction)
{
	if(pClassName != NULL && pFunction != NULL)
		m_mSceneFunMap[pClassName] = pFunction;
}
///注册对话框实例，通过类名映射
void CSceneManager::registePopupBoxClass(const char* pClassName, PFNCREATEPOPUPBOX pFunction)
{
	if(pClassName != NULL && pFunction != NULL)
		m_mPopupBoxFunMap[pClassName] = pFunction;
}
///释放所有不在运行场景栈 但是在场景缓存列表里的场景
void CSceneManager::removeAllCachedScene()
{
	//所有计数全部-1，没有在场景栈里的场景就都被释放掉了
	map<string, CScene*>::iterator iter = m_mSceneCacheMap.begin();
	for(; iter != m_mSceneCacheMap.end(); iter++)
		iter->second->release();
	m_mSceneCacheMap.clear();
}
///释放所有不在运行弹出框组里 但是在缓存里的弹出框
void CSceneManager::removeAllCachedPopupBox()
{
	//所有计数全部-1，没有加在根节点上的弹出框就被释放掉了
	map<string, CPopupBox*>::iterator iter = m_mPopupBoxCacheMap.begin();
	for(; iter != m_mPopupBoxCacheMap.end(); iter++)
		iter->second->release();
	m_mPopupBoxCacheMap.clear();
}
//当前运行场景
CScene *CSceneManager::getRunningScene()
{
	//return dynamic_cast<CScene*>(CCDirector::sharedDirector()->getRunningScene());
	return m_pScene;
}

CScene* CSceneManager::getCurrScene()
{
	return m_pScene;
}