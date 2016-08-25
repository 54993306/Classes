#include "LayerManager.h"
#include "CCTK/scenemanager.h"

BaseLayer::BaseLayer()
{
	static int id = 0;
	char str[20];
	sprintf(str,"%d",id);
	m_name = str;
	++id;
}

bool BaseLayer::init()
{
	CColorView *colorView = CColorView::create(ccc4(0,0,0,80));
	colorView->setContentSize(CCSizeMake(1138,640));
	colorView->setPosition(VCENTER);
	colorView->setTag(1000);
	this->addChild(colorView);
	this->setCoverScreen(false);
	this->setVisible(false);
	return true;
}

void BaseLayer::setIsShowBlack(bool isVisable)
{
	this->getChildByTag(1000)->setVisible(isVisable);
}

void BaseLayer::setOpacity(float color)
{
	CColorView *colorView = (CColorView*)this->getChildByTag(1000);
	colorView->setOpacity(color);
}

CColorView *BaseLayer::getCColorView()
{
	return (CColorView *)this->getChildByTag(1000);
}
bool BaseLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	bool res = CWidgetWindow::ccTouchBegan(pTouch,pEvent);
	if( !res )
	{
		if( this->boundingBox().containsPoint(pTouch->getLocation()))
		{
			res = true;
		}
	}
	return res;
}

MaskLayer::MaskLayer(const char* name)
	:BaseLayer(name)
{

}

bool MaskLayer::init()
{
	if(!BaseLayer::init()) return false;
	this->setContentSize(CCSizeMake(1138,640));
	//addChild(sp);
	this->setVisible(true);
	return true;
}

LayerManager::LayerManager()
	:m_Priority(0)
	,m_list(nullptr)
{
	m_list = CCArray::create();
	m_list->retain();
}
LayerManager::~LayerManager()
{
	closeAll();
	m_list->release();
	m_list = nullptr;
}
void LayerManager::close(BaseLayer* layer)
{
	if(!layer) return;
	CCObject* object = nullptr;
	CCARRAY_FOREACH(m_list,object)
	{ 
		if(object == layer)
		{
			layer->removeFromParent();
			m_list->removeObject(object);
			break;
		}
	}
}
void LayerManager::close(const char* name)
{
	close(getLayer(name));
}
bool LayerManager::push(BaseLayer* layer, bool bForce)
{
	CCScene* scene = CSceneManager::sharedSceneManager()->getCurrScene();
	if(!scene) return false;
	if(!layer || (getLayer(layer->getName()) && !bForce))
	{
		return false;
	}
	layer->setTouchPriority(getPriority() - m_list->count());
	scene->addChild(layer);
	m_list->addObject(layer);
	return true;
}
void LayerManager::pop()
{
	if(m_list->count()<=0)
	{
		return;
	}
	BaseLayer* layer = (BaseLayer*)m_list->lastObject();
	layer->removeFromParent();
	m_list->removeObject(layer);
}


void LayerManager::removeLayer( BaseLayer* layer )
{
	if(m_list->count()<=0)
	{
		return;
	}
	layer->removeFromParent();
	m_list->removeObject(layer);
}

BaseLayer* LayerManager::getCurrLayer()
{
	if(m_list->count()<=0)
	{
		return nullptr;
	}
	return (BaseLayer*)m_list->lastObject();
}
BaseLayer* LayerManager::getLayer(const char* name)
{
	CCObject* object = nullptr;
	CCARRAY_FOREACH(m_list,object)
	{
		BaseLayer* layer = (BaseLayer*)object;
		if(strcmp(name,layer->getName()) == 0)
		{
			return layer;
		}
	}
	return nullptr;
}
void LayerManager::setPriority(int priority)
{
	if(m_Priority != priority)
	{
		m_Priority = priority;
		updatePriority();
	}
}
void LayerManager::release()
{
	delete this;
}
LayerManager* LayerManager::instance()
{
	static LayerManager* m_layerManager = nullptr;
	if(!m_layerManager)
	{
		m_layerManager = new LayerManager();
	}
	return m_layerManager;
}
void LayerManager::updatePriority()
{
	int index = m_list->count() - 1;
	CCObject* object = nullptr;
	CCARRAY_FOREACH(m_list,object)
	{
		BaseLayer* layer = (BaseLayer*)object;
		layer->setTouchPriority(getPriority()-index);
		--index;
	}
}
////////////////////////////////////////////////////////////////////////////////until
void LayerManager::closeAll()
{
	do 
	{
		CC_BREAK_IF(m_list->count()<=0);
		pop();
	} while (m_list->count()>0);
}

int LayerManager::getLayerCount()
{
	return m_list->count();
}

void LayerManager::hideLayerBeCovered( BaseLayer* pLayer )
{
	int iIndexLayer = m_list->indexOfObject(pLayer);
	
	if(iIndexLayer==0) return;

	for(int i=iIndexLayer-1; i>=0; i--)
	{
		BaseLayer* pCoveredLayer = (BaseLayer*)m_list->objectAtIndex(i);
		if(!pCoveredLayer->getCoverScreen())
		{
			pCoveredLayer->setVisible(false);
		}
		else
		{
			break;
		}
	}
}

void LayerManager::showLayerUnBeCovered( BaseLayer* pLayer )
{
	int iIndexLayer = m_list->indexOfObject(pLayer);

	if(iIndexLayer==0) return;

	for(int i=iIndexLayer-1; i>=0; i--)
	{
		BaseLayer* pCoveredLayer = (BaseLayer*)m_list->objectAtIndex(i);
		if(!pCoveredLayer->getCoverScreen())
		{
			pCoveredLayer->setVisible(true);
		}
		else
		{
			break;
		}
	}
}

void LayerManager::removeInvalidPopLayer()
{
	//倒数第一个弹框隐藏，网络数据没返回，主动pop掉这两个
	if(m_list->count()<2)
	{
		return;
	}
	CCLayer* pLayer1 = (CCLayer*)m_list->objectAtIndex(m_list->count()-1);
	if(!pLayer1->isVisible())
	{
		pop();
		pop();
	}
}

int LayerManager::getLayerManagerMinPriority()
{
	return m_Priority-m_list->count();
}



