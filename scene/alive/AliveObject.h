#ifndef _ALIVEOBJECT_H_
#define _ALIVEOBJECT_H_

/************************************************************* 
 *
 *
 *		Data : 2016.5.12
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 用于做武将显示对象处理的地方,和数据对象waralive构成一个完整的ActObject
 *
 *
 *************************************************************/
#include "ActionDef.h"
#include "AppUI.h"
#include <string>
#include <spine/spine-cocos2dx.h>

class HPObject;
class EffectObject;
class WarAlive;
class MoveObject;


using namespace std;
using namespace spine;

enum AliveDirection	//武将朝向有可能有8个方向,若仅是2个方向可以使用布尔值替代
{
	Ditection_Left,		//左
	Ditection_Right,	//右		(武将默认朝向、怪物朝向)
};

//用于显示的武将信息
class AliveObject : public CCNode,public CCTargetedTouchDelegate
{
public:
	AliveObject();
	virtual ~AliveObject();
	CREATE_FUNC(AliveObject);	
	virtual bool init();
	void AliveDie();
	void AtkEnd_Event();
	void AtkBegin_Event();
	void HpChange_Event();
	void PlayAnimat_Event(string sData);
	void initAliveTypeIcon();
	void playerNum(int num,int type);
	void initTypeIconPath( char* pPath );
	void lostHpDispose();
public:
	CC_SYNTHESIZE(WarAlive*,m_Alive,Alive);				//设置逻辑对象
	CC_SYNTHESIZE(MoveObject*,m_MoveObj,MoveObject);			//设置移动对象
	CC_SYNTHESIZE(int,m_ID,ID);							//显示对象id
	CC_SYNTHESIZE(bool,m_Enemy,Enemy);					//阵营
	CC_SYNTHESIZE(int,m_Model,Model);					//角色模型ID( 子类重写 )
	CC_SYNTHESIZE(string,m_ActionKey,ActionKey);		//武将当前动作
	CC_SYNTHESIZE(int,m_DropItem,DropItem);				//掉落物品类型
	CC_SYNTHESIZE(CCNodeRGBA*,m_Armature,Armature);		//动画对象	( 子类实现 )
	CC_SYNTHESIZE(SkeletonAnimation*,m_Skeleton,Skeleton);//动画对象	( 子类实现 )
	CC_SYNTHESIZE(int,m_AtkEffect,AtkEffect);			//攻击特效id
	CC_SYNTHESIZE(int,m_PlayerEffect,PlayerEffect);		//播放空效果
	CC_SYNTHESIZE(int,m_EffectMusic,EffectMusic);		//空效果音效
	CC_SYNTHESIZE(bool,m_UpdateState,UpdateState);		//是否在攻击结束刷新武将状态
	CC_PROPERTY(string,m_Name,NickName);				//设置武将显示名称
	CC_PROPERTY(CCSprite*,m_Body,Body);					//显示搭载
	CC_PROPERTY(int,m_Direction,RoleDirection);			//武将方向
	CC_PROPERTY(HPObject*,m_HpObject,Hp);				//血量对象
	CC_SYNTHESIZE(CCPoint,m_offs,offs);					//实际位置相对于
	CC_SYNTHESIZE(int,m_MoveState,MoveState);			//刷新位置执行的动作
	CC_SYNTHESIZE(CCPoint,m_Speed,Speed);				//移动速度
	CC_SYNTHESIZE(bool,m_IsSpine,IsSpine);				//spine动画
private:
	void testLabel();
protected:									
	CCLabelTTF* m_NameLabel;							//武将显示名称
};
#endif // !_ALIVEOBJECT_H_
