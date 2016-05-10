#ifndef _ALIVEOBJECT_H_
#define _ALIVEOBJECT_H_
#include "ActionDef.h"
#include "AppUI.h"
#include <string>
#include <spine/spine-cocos2dx.h>
class HPObject;
class EffectObject;
class WarAlive;
class MoveObj;


using namespace std;
using namespace spine;

enum AliveDirection	//武将朝向
{
	Ditection_Right,	//右		(武将默认朝向、怪物朝向)
	Ditection_Left,		//左
	FLOOR_TAG = 70,		//武将脚下格子
	ROLEEFFECT,			//热能效果
	CritHpBmg_Sp,		//暴击掉血背景图
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
public:
	CC_SYNTHESIZE(WarAlive*,m_Alive,Alive);				//设置逻辑对象
	CC_SYNTHESIZE(MoveObj*,m_MoveObj,MoveObj);			//设置移动对象
	CC_SYNTHESIZE(int,m_ID,ID);							//显示对象id
	CC_SYNTHESIZE(bool,m_Enemy,Enemy);					//阵营
	CC_SYNTHESIZE(int,m_Model,Model);					//角色模型ID( 子类重写 )
	CC_SYNTHESIZE(string,m_ActionKey,ActionKey);		//武将当前动作
	CC_SYNTHESIZE(int,m_DropItem,DropItem);				//掉落物品类型
	CC_SYNTHESIZE(CCNode*,m_Armature,Armature);			//动画对象	( 子类实现 )
	CC_SYNTHESIZE(SkeletonAnimation*,m_Skeleton,Skeleton);//动画对象	( 子类实现 )
	CC_SYNTHESIZE(int,m_AtkEffect,AtkEffect);			//攻击特效id
	CC_SYNTHESIZE(int,m_PlayerEffect,PlayerEffect);		//播放空效果
	CC_SYNTHESIZE(int,m_EffectMusic,EffectMusic);		//空效果音效
	CC_SYNTHESIZE(bool,m_Front,Front);					//让武将单独高亮
	CC_SYNTHESIZE(bool,m_UpdateState,UpdateState);		//是否在攻击结束刷新武将状态
	CC_PROPERTY(string,m_Name,NickName);				//设置武将显示名称
	CC_PROPERTY(CCSprite*,m_Body,Body);					//显示搭载
	CC_PROPERTY(int,m_Direction,Direction);				//武将方向
	CC_PROPERTY(HPObject*,m_Hp,Hp);						//血量对象
	CC_SYNTHESIZE(CCPoint,m_offs,offs);					//实际位置相对于
	CC_SYNTHESIZE(int,m_MoveState,MoveState);			//刷新位置执行的动作
	CC_SYNTHESIZE(CCPoint,m_Speed,Speed);				//移动速度
	CC_SYNTHESIZE(bool,m_DieState,DieState);			//是否已死亡
	CC_SYNTHESIZE(bool,m_IsSpine,IsSpine);				//spine动画
protected:									
	CCLabelTTF* m_NameLabel;							//武将显示名称
};
#endif // !_ALIVEOBJECT_H_
