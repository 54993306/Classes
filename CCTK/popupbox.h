/******************************************************************************
文件名: popupbox.h
编写者: csdn_viva@foxmail.com ccbase1.0
编写日期: 2013-5-2
功能描述: 弹出框定义，触摸优先级 POPUPBOX_PRIORITY
******************************************************************************/
#ifndef __CCBASE_POPUPBOX_H__
#define __CCBASE_POPUPBOX_H__

/******************************************************************************
类    名: CPopupBox
功能描述: 弹出框定义
******************************************************************************/
class CPopupBox : public CCLayerColor
{
public:
	///构造函数，初始化默认值
	CPopupBox();
	///析构函数
	virtual ~CPopupBox();

public:
	///设置弹出框是否永久缓存
	void setCached(bool bCon) { m_bIsCached = bCon; }
	///获取弹出框是否永久缓存
	bool isCached() { return m_bIsCached; }

	///设置弹出框对应的类名
	void setClassName(const char* pClassName) { m_strClassName = pClassName; }
	///获取弹出框的类名
	string getClassName() { return m_strClassName; }

public:
	///当场景创建时调用
	virtual void onCreate() {};
	///当场景被打开的时候调用
	virtual void onOpen(const PARAM param) {};
	///当进入显示时调用
	virtual void onEnter();
	///当退出隐藏时调用
	virtual void onExit();
	///关闭本对话框
	virtual void closeSelf();

public:
	///重写事件分发函数
	virtual void registerWithTouchDispatcher();
	///重写当触摸按下时回调
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent) { return false; };
	///重写当移动时回调
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent) {};
	///重写当触摸释放时调用
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent) {};
	///重写当触摸中断时调用
	virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent) {};

protected:
	//弹出框名称
	string	m_strClassName;
	//弹出框是否缓存
	bool	m_bIsCached;

};

#endif //__CCBASE_POPUPBOX_H__
