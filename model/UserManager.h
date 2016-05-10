#ifndef _USER_MANAGER_H_
#define _USER_MANAGER_H_
#include "cocos2d.h"
#include "DataDefine.h"
#include <map>
#include "protos/common/role_common.pb.h"
USING_NS_CC;
using namespace protos::common;
using namespace std;
class UserManager : public CCObject
{
public:
	UserManager();
	virtual ~UserManager();
	CREATE_FUNC(UserManager);
public:
	void addAlive(CityAlive* alive);
	CCArray* getAlives();
	CityAlive* getAlive(unsigned int aliveID);
	inline UserData* getUserData(){return m_userData;};

	void addRoleAlive(Role *role);
	void removeRoleAlive(int roleId);

	void test();
	void clear();

	void setoldLevel();
	bool upLevel();
protected:
	virtual bool init();
protected:
	UserData* m_userData;
	map<unsigned int,CityAlive*> m_list;
private:
	int m_oldLevel;
};
#endif // !_USER_MANAGER_H_
