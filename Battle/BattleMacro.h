#ifndef __BATTLEMACRO__
#define __BATTLEMACRO__

/** 
 *
 *		Data : 2016.4.12
 *	
 *		Name : BattleMacro
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 定义常用宏
 *
 */

#define CC_PROPERTY_CONSTREAD(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void) const { return varName; }\
public: virtual void set##funName(varType var);




#endif