/** 
 *
 *
 *		Data : 2016.4.13
 *	
 *		Name : ParseRoleConfigFile
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 解析角色配置数据
 *
 *
 */

#ifndef __ParseRoleConfigFile__
#define __ParseRoleConfigFile__

#include "cocos2d.h"

using namespace cocos2d;
using namespace std;
class BaseRoleData;
class ParseRoleConfigFile : public CCObject
{
public:
	ParseRoleConfigFile();
	~ParseRoleConfigFile();
	virtual bool init(){return true;}
	CREATE_FUNC(ParseRoleConfigFile);
	void pauseData(vector<BaseRoleData*>& Vec);
protected:
	
private:
};
#endif