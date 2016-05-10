/******************************************************************************
文件名: base.h
编写者: csdn_viva@foxmail.com ccbase1.0
编写日期: 2013-5-2
功能描述: 所有头文件的定义集合
******************************************************************************/
#ifndef __CCBASE_BASE_H__
#define __CCBASE_BASE_H__

#include "cocos2d.h"
using namespace cocos2d;

class CScene;
class CArray;
class CLooper;
class CPopupBox;
class CSceneManager;
NS_CC_BEGIN
class CCSceneExTransition;
NS_CC_END

typedef CScene*		(*PFNCREATESCENE)();	//构造场景的函数指针类型
typedef CPopupBox*	(*PFNCREATEPOPUPBOX)();	//构造弹出框的函数指针类型

#define PARAM void*
#define ARRAY CArray*

//场景的创建实例的反射方法
#define DEFINE_SCENE_CLASS(__TYPE__) \
static CScene* newInstance() { \
__TYPE__ *__cls__ = new __TYPE__(); \
if(__cls__ && __cls__->init()) \
{ \
	return __cls__; \
} \
CC_SAFE_DELETE(__cls__); \
return NULL; \
}
//弹出框的创建实例的反射方法
#define DEFINE_POPUPBOX_CLASS(__TYPE__) \
static CPopupBox* newInstance() { \
__TYPE__ *__cls__ = new __TYPE__(); \
if(__cls__ && __cls__->init()) \
{ \
	return __cls__; \
} \
CC_SAFE_DELETE(__cls__); \
return NULL; \
}
//注册场景
#define REGISTE_SCENE_CLASS(__TYPE__) \
CSceneManager::sharedSceneManager()->registeSceneClass(#__TYPE__, &__TYPE__::newInstance)
//获取场景
#define GETSCENE(__TYPE__) \
CSceneManager::sharedSceneManager()->getScene(#__TYPE__)

//获取当前场景
#define GETCURRSCENE()  CSceneManager::sharedSceneManager()->getCurrScene()

//注册弹出框
#define REGISTE_POPUPBOX_CLASS(__TYPE__) \
CSceneManager::sharedSceneManager()->registePopupBoxClass(#__TYPE__, &__TYPE__::newInstance)
//获取弹出框
/*
#define GETBOX(__TYPE__) \
#__TYPE__ */

#define GETBOX(__TYPE__) \
CSceneManager::sharedSceneManager()->getPopupBox(#__TYPE__)

//弹出框的触摸优先级
#define POPUPBOX_PRIORITY -1000000
//弹出框上的事件优先级
#define GET_PRIORITY(__V__) POPUPBOX_PRIORITY + __V__

#include "array.h"
#include "scene.h"
#include "popupbox.h"
#include "scenemanager.h"
//#include "CCSceneExTransition.h"

#endif //__CCBASE_BASE_H__
