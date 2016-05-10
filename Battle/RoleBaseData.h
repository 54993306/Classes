/** 
 *
 *		Data : 2016.4.12
 *	
 *		Name : ServerRoleBaseData
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 服务器传输来的基础数据信息(敌我双方都需要的信息)
 *
 */
#include "cocos2d.h"
using namespace std;
using namespace cocos2d;

class RoleBaseData : public CCObject
{
public:
	RoleBaseData();
	CC_SYNTHESIZE(int		,m_RoleID			,RoleID);						//武将ID
	CC_SYNTHESIZE(int		,m_ServerID			,ServerID);						//服务器提供ID
	CC_SYNTHESIZE(int		,m_Model			,Model);						//模型ID
	CC_SYNTHESIZE(bool		,m_Enemy			,Enemy);						//阵营
	CC_SYNTHESIZE(float		,m_MoveSpeed		,MoveSpeed);					//移动速度(格/秒)
	CC_SYNTHESIZE(float		,m_AtkInterval		,AtkInterval);					//攻击速度(秒/次)
	CC_SYNTHESIZE(int		,m_Attribute		,Attribute);					//武将属性(火1,水2,木3)
	CC_SYNTHESIZE(int		,m_Row				,Row);							//武将所占行数
	CC_SYNTHESIZE(int		,m_Col				,Col);							//武将所占列数
	CC_SYNTHESIZE(int		,m_Grid				,Grid);							//武将站位
	CC_SYNTHESIZE(int		,m_Maxhp			,Maxhp);						//最大血量值
	CC_SYNTHESIZE(int		,m_Hp				,Hp);							//当前血量值
	CC_SYNTHESIZE(int		,m_Attack			,Attack);						//攻击
	CC_SYNTHESIZE(int		,m_Defense			,Defense);						//防御
	CC_SYNTHESIZE(int		,m_Crit				,Crit);							//暴击
	CC_SYNTHESIZE(int		,m_Hit				,Hit);							//命中
	CC_SYNTHESIZE(int		,m_Dodge			,Dodge);						//闪避
	CC_SYNTHESIZE(int		,m_Agility			,Agility);						//敏捷
	CC_SYNTHESIZE(int		,m_Revert			,Revert);						//回复
	CC_SYNTHESIZE(int		,m_Zoom				,Zoom);							//缩放比
	CC_SYNTHESIZE(int		,m_RoleType			,RoleType);						//武将类型(包含召唤和怪物两种类型)
	CC_SYNTHESIZE(bool		,m_Summon			,Summon);						//是否为召唤类武将


	//int CallType;				//召唤类型(Control)
	//int MstType;				//怪物类型
	//vector<CBuff> buffList;	//武将进入战场时带进来的buf
	//int hasitem;				//掉落物品类型1、蓝2、金3、彩4、道具5、金币
	//int	level;				//武将等级
	//string name;				//名称
private:

};