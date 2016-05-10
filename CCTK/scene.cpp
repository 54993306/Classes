/******************************************************************************
文件名: scene.cpp
编写者: csdn_viva@foxmail.com ccbase1.0
编写日期: 2013-5-2
功能描述: 场景基类，用于协助CSceneManager管理场景
******************************************************************************/

#include "base.h"
#include "scene.h"

///构造函数，初始化默认值
CScene::CScene(): m_bIsCached(false)
{

}

///析构函数
CScene::~CScene() 
{
	CCLOG("## [DEBUG] Release Scene [%s]", m_strClassName.c_str());
}