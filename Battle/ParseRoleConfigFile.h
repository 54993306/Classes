/** 
 *
 *
 *		Data : 2016.4.13
 *	
 *		Name : ParseRoleConfigFile
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : ������ɫ��������
 *
 *
 */

#ifndef __ParseRoleConfigFile__
#define __ParseRoleConfigFile__

#include "cocos2d.h"

using namespace cocos2d;
using namespace std;
class RoleBaseData;
class ParseRoleConfigFile : public CCObject
{
public:
	ParseRoleConfigFile();
	~ParseRoleConfigFile();
	virtual bool init(){return true;}
	CREATE_FUNC(ParseRoleConfigFile);
	void pauseData(vector<RoleBaseData*>& Vec);
protected:
	
private:
};
#endif