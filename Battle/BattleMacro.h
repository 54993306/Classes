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

#if CC_PLATFORM_WIN32  == CC_TARGET_PLATFORM

#ifndef BATTLE_TEST
#define BATTLE_TEST 1
#endif

#endif

#define PROPERTY_CONSTREAD(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void) const { return varName; }\
public: virtual void set##funName(varType var);

#define UNINITCREATE(__TYPE__) \
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


#define DECLARE_ENUM(E) \
struct E \
{ \
public: \
	E(int value = 0) : _value((__Enum)value) { \
	} \
	E& operator=(int value) { \
	this->_value = (__Enum)value; \
	return *this; \
	} \
	operator int() const { \
	return this->_value; \
	} \
	\
	enum __Enum {
#define END_ENUM() \
	}; \
	\
private: \
	__Enum _value; \
};

//DECLARE_ENUM(FileAccess)
//Read = 0x1,
//Write = 0x2,
//END_ENUM()

#define ROLE_READONLY(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void) const;




#endif