
/** 
 *
 *		Data : 2016.4.13
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 每个模型对应的配置数据信息
 *
 */
#ifndef __ROLECONFIGDATA__
#define __ROLECONFIGDATA__

#include "cocos2d.h"

//using namespace cocos2d;
using namespace std;

class RoleConfigData /*: public CCObject*/
{
public:
	RoleConfigData();
	~RoleConfigData();
	CC_SYNTHESIZE(int,m_RoleMode,RoleMode);
	CC_SYNTHESIZE(int,m_MoveSound,MoveSound);
	CC_SYNTHESIZE(bool,m_Turn,Turn);
protected:
	
private:

};

#endif