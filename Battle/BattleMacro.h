#ifndef __BATTLEMACRO__
#define __BATTLEMACRO__

/** 
 *
 *		Data : 2016.4.12
 *	
 *		Name : CC_PROPERTY_CONSTREAD
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : get函数const类型,需要实现set函数
 *
 */

#define PROPERTY_CONSTREAD(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void) const { return varName; }\
public: virtual void set##funName(varType var);

#define CREATE_UNINIT(__TYPE__) \
	static __TYPE__* create() \
{ \
	__TYPE__ *pRet = new __TYPE__(); \
	if (pRet) \
	{ \
	pRet->autorelease(); \
	return pRet; \
	} \
	else \
	{ \
	delete pRet; \
	pRet = NULL; \
	return NULL; \
	} \
}

#endif