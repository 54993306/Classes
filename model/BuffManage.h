#ifndef _BUFF_MANAGER_H_
#define _BUFF_MANAGER_H_
#include "AppUI.h"
#include <map>

/************************************************************* 
 *
 *
 *		Data : 2016.5.27
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/
using namespace std;
class WarAlive;
class RoleBuffData;
class BuffInfo;
typedef map<unsigned int,BuffInfo*> BuffMap;	
typedef map<int,vector<CCNode*>>	BuffEffectMapList;	

class BuffManage : public CCObject
{
public:
	BuffManage();
	virtual ~BuffManage();
	CREATE_FUNC(BuffManage);
	CC_SYNTHESIZE(WarAlive*,m_alive,Alive);
	void Buffclear();
	void clearDbuf();											//清除减益buf
	BuffInfo* getbuff(int bufID);
	BuffInfo* getbuffbyType(int buftype);
	void removeBuf(int id);
	void AddBuff(RoleBuffData& buf);							//将buff对象添加到武将身上
	bool AddBuffLogic(RoleBuffData& buff);						//buf替换添加逻辑
	void upDateBuff(float dt);
	BuffMap* getBuffMap(){return &m_BuffMap;}
	vector<CCNode*>* getVecEffect(int buffid);					//判断是否已经存在效果数组
	void AddEffectVec(int buffid,vector<CCNode*>& Vec);
	BuffEffectMapList* getEffectMap(){return &m_EffectMap;}
	void ExcuteBuff(BuffInfo*bfinfo,bool handel = true);		//为false时清除buf对武将的影响/*buf处理类型分发函数*/

	int  bufCurrHpHandle(BuffInfo* bfinfo,bool handel);			//当前血量	/*可以存在当前的原因是buf移除不用恢复*/
	void bufAtkHandle(BuffInfo* bfinfo,bool handel);			//攻击		/*区分持续和非持续影响的属性*/
	void bufDefHandle(BuffInfo* bfinfo,bool handel);			//防御
	void bufCritHandle(BuffInfo* bfinfo,bool handel);			//暴击
	void bufDogeHandle(BuffInfo* bfinfo,bool handel);			//闪避
	void bufHitHandle(BuffInfo* bfinfo,bool handel);			//命中
	void bufMSpeedHandle(BuffInfo* bfinfo,bool handel);			//移动速度
	void bufAtkSpeedHandle(BuffInfo* bfinfo,bool handel);		//攻击速度

	int  bufMaxHpHandle(BuffInfo* bfinfo,bool handel);			//最大血量
	void bufHrtHandle(BuffInfo* bfinfo,bool handel);			//伤害
protected:
	BuffMap m_BuffMap; 
	BuffEffectMapList m_EffectMap;										//存储武将身上的表现效果。
	bool init(){return true;}			
};
#endif // _BUFFMANAGE_