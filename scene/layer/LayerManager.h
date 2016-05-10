#ifndef LAYER_MANAGER_H_
#define LAYER_MANAGER_H_
#include "AppUI.h"
#include <string>
#include <vector>
using namespace std;
#define CREATE_LAYER(_TYPE_)             \
static _TYPE_* create()					 \
{                                        \
	_TYPE_* pRet = new _TYPE_();		 \
	if(pRet && pRet->init())		     \
	{									 \
		pRet->setName(#_TYPE_);			 \
		pRet->autorelease();			 \
		return pRet;                     \
	}                                    \
	delete pRet;                         \
	pRet = nullptr;                      \
	return nullptr;                      \
}                                        
#define CREATE_LAYER_BY_NAME(_TYPE_)     \
static _TYPE_* create(const char* name)  \
{                                        \
	_TYPE_* pRet = new _TYPE_(name);     \
	if(pRet && pRet->init())             \
	{                                    \
		pRet->autorelease();             \
		return pRet;                     \
	}                                    \
	delete pRet;                         \
	pRet = nullptr;                      \
	return nullptr;                      \
}
//层基类，场景所有层需继承她
class BaseLayer:public CWidgetWindow
{
public:
	BaseLayer();
	explicit BaseLayer(const char* name):m_name(name),m_bIsCoverScreen(false){};
	virtual ~BaseLayer(){};
	virtual bool init();
	void setIsShowBlack(bool isVisable);
	void setOpacity(float color);
	CColorView* getCColorView();
	inline const char* getName(){return m_name.c_str();}
	inline void setName(const char* pName){ m_name = pName;};
	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	CC_SYNTHESIZE(bool, m_bIsCoverScreen, CoverScreen);
protected:
	string m_name;
};
class MaskLayer:public BaseLayer
{
public:
	MaskLayer(){};
	explicit MaskLayer(const char* name);
	bool init();
	CREATE_LAYER(MaskLayer);
	CREATE_LAYER_BY_NAME(MaskLayer);
};
class LayerManager
{
public:
	LayerManager();
	~LayerManager();
	static LayerManager* instance();

	bool push(BaseLayer* layer, bool bForce=false);
	void pop();
	void removeLayer(BaseLayer* layer);
	void close(BaseLayer* layer);
	void close(const char* name);
	void closeAll();
	BaseLayer* getCurrLayer();
	BaseLayer* getLayer(const char* name);
	void setPriority(int priority);
	inline int getPriority(){return m_Priority;}
	void release();

	int getLayerCount();

	void hideLayerBeCovered(BaseLayer* pLayer);
	void showLayerUnBeCovered(BaseLayer* pLayer);

	void removeInvalidPopLayer();

protected:
	void updatePriority();
protected:
	CCArray* m_list;
	int m_Priority;//最下层的事件权限
};
#endif // !LAYER_MANAGER_H_
