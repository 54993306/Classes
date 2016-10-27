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
//实现一个战斗专用的消息监听机制   引擎提供的效率非常低

const char B_SKILL_MASK[]					=	"B_SKILL_MASK";									//技能遮罩颜色
const char B_WorldBoss_HurtUpdate[]			=	"B_WroldBoss_HurtUpdate";						//刷新时间boss伤害值
const char B_ChangeMoveState[]				=	"B_ChangeMoveState";
const char B_RecordContinuousSkill[]		=	"B_RecordContinuousSkill";
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
const char MsgMonsterTips[]					=	"MsgMonsterTips";
const char B_DrpItem[]						=	"Msg_DropItem";
const char MsgRoleDie[]						=	"ALIVEDIE";						//一个武将死亡
const char B_ActObjectRemove[]				=	"ACTREMOVE";					//武将死亡移除
const char MsgAddBuff[]						=	"ADDBUFF";						//武将添加buff表现
const char B_RemoveBuff[]					=	"REMOVEBUFF";					//武将移除buff时的表现
const char B_UpdateBuffEffect[]				=	"UPBUFFEFFECT";					//刷新武将buff效果
const char B_PlayBeginAnimation[]			=	"PLAYERBEGINANIMATION";			//播放出场动画
const char MsgStorOver[]					=	"WAR_STORY_OVER";				//战斗剧情对话结束
const char MsgReturnLayer[]					=	"MsgReturnLayer";				//显示返回层
const char B_CritEnd[]						=	"SKILLEND";						//武将技能释放完毕
const char B_IntoBattle[]					=	"B_IntoBattle";					//延时完成进入战场
const char B_RoleSkill[]					=   "B_RoleSkill";					//我方武将释放技能
const char B_SkillCreateRole[]				=	"B_SkillCreateRole";			//技能创建武将
const char B_AttactNull[]					=	"B_AttactNull";					//空效果处理
const char B_AttackResult[]					=	"B_AttackResult";				//攻击结果表现
const char B_EntranceBattle[]				=	"B_EntranceBattle";				//武将触摸进入战斗
const char MsgCreateRoleObject[]			=	"MsgCreateRoleObject";			//创建显示武将
const char MsgBattleOver[]					=	"MsgBattleOver";				//战斗结束
const char MsgAutoBattle[]					=	"MsgAutoBattle";				//自动战斗
const char MsgBattleStateChange[]			=	"MsgBattleStateChange";			//上阵状态改变
const char MsgGetButtonState[]				=	"MsgGetButtonState";			//获取按钮状态
const char MsgSendButtonState[]				=	"MsgSendButtonState";			//发送按钮状态
const char MsgButtonStateUpdate[]			=	"MsgButtonStateUpdate";			//按钮状态刷新
const char MsgRoleEnterBattle[]				=	"MsgRoleInToBattle";			//角色进入战区
const char MsgRoleGridChange[]				=	"MsgRoleGridChange";			//角色位置改变
const char MsgUpAutoSkillState[]			=	"MsgUpAutoSkillState";			//刷新自动释放技能状态
const char MsgRoleStand[]					=	"MsgRoleStand";					//所有角色站立
const char MsgRoleBattleState[]				=	"MsgRoleBattleState";			//进入战场状态变化
const char MsgReleaseTouch[]				=	"MsgReleaseTouch";				//释放角色层触摸
const char MsgStarActionEnd[]				=	"MsgStarActionEnd";				//入场动作播放结束
const char MsgControlRemove[]				=	"MsgControlRemove";				//移除控制面板操作
const char MsgCreateStory[]					=	"MsgCreateStory";				//创建剧情
const char MsgMonsterDie[]					=	"MsgMonsterDie";				//怪物死亡
const char MsgHideControlUp[]				=	"MsgHideControlUp";				//隐藏控制面板上部分
const char MsgNextBatchEnemy[]				=	"MsgNextBatchEnemy";			//创建下一批怪物
const char MsgUpBatchNumber[]				=	"MsgUpBatchNumber";				//刷新关卡批次
const char MsgPlayWinEffect[]				=	"MsgPlayWinEffect";				//播放胜利特效
const char MsgShowStageWarning[]			=	"MsgShowStageWarning";			//显示关卡警告
const char MsgChangeLayerLight[]			=	"MsgChangeLayerLight";			//改变角色层亮度
const char MsgRoleVariant[]					=	"MsgRoleVariant";				//角色变身
const char MsgCreateSpineFloor[]			=	"MsgCreateSpineFloor";			//创建spine地板特效
const char MsgDelayAttack[]					=	"MsgDelayAttack";				//延迟攻击效果
const char MsgInitBossBar[]					=	"MsgInitBossBar";				//初始化boss的显示条
const char MsgUpdateBossHp[]				=	"MsgUpdateBossHp";				//刷新boss血量值
#endif