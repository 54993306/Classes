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
const char B_DrpItem[]						=	"Msg_DropItem";
const char B_AliveDie[]						=	"ALIVEDIE";						//一个武将死亡
const char B_ActObjectRemove[]				=	"ACTREMOVE";					//武将死亡移除
const char B_ChangeSceneSpeed[]				=	"CHANGEBATTLESPEED";			//改变战斗速度
const char B_LayerMoveEnd[]					=	"LAYERMOVEEND";					//开场层移动完毕
const char B_AddBuff[]						=	"ADDBUFF";						//武将添加buff表现
const char B_RemoveBuff[]					=	"REMOVEBUFF";					//武将移除buff时的表现
const char B_UpdateBuffEffect[]				=	"UPBUFFEFFECT";					//刷新武将buff效果
const char B_PlayBeginAnimation[]			=	"PLAYERBEGINANIMATION";			//播放出场动画
const char B_RoundStart[]					=	"WAR_ROUND_START";				//回合开始
const char B_StoryOver[]					=	"WAR_STORY_OVER";				//战斗剧情对话结束
const char B_ReturnLayer[]					=	"SHOW_PLAN_PANEL";				//显示当前关卡进度
const char B_CritEnd[]						=	"SKILLEND";						//武将技能释放完毕
const char B_IntoBattle[]					=	"B_IntoBattle";					//延时完成进入战场
const char B_RoleSkill[]					=   "B_RoleSkill";					//我方武将释放技能
const char B_SkillCreateRole[]				=	"B_SkillCreateRole";			//技能创建武将
const char B_AttactNull[]					=	"B_AttactNull";					//空效果处理
const char B_AttackResult[]					=	"B_AttackResult";				//攻击结果表现
const char B_EntranceBattle[]				=	"B_EntranceBattle";				//武将触摸进入战斗
#endif