/******************************************************************************
文件名: scene.h
编写者: csdn_viva@foxmail.com ccbase1.0
编写日期: 2013-5-2
功能描述: 场景基类，用于协助CSceneManager管理场景
******************************************************************************/
#ifndef __CCBASE_SCENE_H__
#define __CCBASE_SCENE_H__

/******************************************************************************
类    名: CScene
功能描述: 场景基类，用于协助CSceneManager管理场景
******************************************************************************/
class CScene : public CCScene
{
public:
	///构造函数，初始化默认值
	CScene();
	///析构函数
	virtual ~CScene();

public:
	//当场景创建时调用
	virtual void onCreate() {};
	//当场景接收到消息时调用
	virtual void onMessage(int nMsg, ARRAY wParam, PARAM lParam) {};

public:
	///设置场景是否永久缓存
	void setCached(bool bCon) { m_bIsCached = bCon; }
	///获取场景是否永久缓存
	bool isCached() { return m_bIsCached; }

	///设置场景对应的类名
	void setClassName(const char* pClassName) { m_strClassName = pClassName; }
	///获取场景的类名
	string getClassName() { return m_strClassName; }

protected:
	//场景名
	string	m_strClassName;
	//场景是否缓存
	bool	m_bIsCached;
};



#endif //__CCBASE_SCENE_H__
