#ifndef __BATTLEMESSAGE__
#define __BATTLEMESSAGE__

/** 
 *
 *		@Data : 2016.4.12
 *	
 *		@Name : BattleMessage
 *
 *		@Author : Lin_Xiancheng
 *
 *		@Description : 定义战斗中使用到的消息
 *
 *		@brief		以 B 开头,在战斗模块进行消息传递
 *
 *		@purpose	将在主城中使用到的消息和战斗中的消息区分开,减少编译时间
 *
 */

const char B_SKILL_MASK[]					=	"B_SKILL_MASK";									//技能遮罩颜色
const char B_WorldBoss_HurtUpdate[]			=	"B_WroldBoss_HurtUpdate";						//刷新时间boss伤害值
const char B_ChangeMoveState[]				=	"B_ChangeMoveState";
const char B_RecordContinuousSkill[]		=	"B_RecordContinuousSkill";
const char B_ChangeCostNumber[]				=	"B_ChangeCostNumber";
const char B_DrawDynamicSkillArea[]			=	"B_DrawDynamicSkillArea";
const char B_CaptainHurt[]					=	"B_CaptainHurt";
const char B_CostArea[]						=	"B_CostArea";
const char B_Shark[]						=	"B_Shark";
const char B_CancelCostArea[]				=	"B_CancelCostArea";
const char B_RoleAttackCostArea[]			=	"B_RoleAttackCostArea";
const char B_SkilEffectInMap[]				=	"B_SkilEffectInMap";
const char B_CancelDrawAttackArea[]			=	"B_CancelDrawAttackArea";
const char B_DrawSkillArea[]				=	"B_DrawSkillArea";
const char B_RemoveSkillTipsLayer[]			=	"B_RemoveSkillTipsLayer";
const char B_DropItem[]						=	"B_DropItem";		
const char B_ContinuousNumber[]				=	"B_ContinuousNumber";
const char B_RolrLogInBattlefield[]			=	"B_RolrLogInBattlefield";
const char B_MonsterTips[]					=	"B_MonsterTips";



#endif