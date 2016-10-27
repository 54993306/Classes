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
 *		@Description : ����ս����ʹ�õ�����Ϣ
 *
 *		@brief		�� B ��ͷ,��ս��ģ�������Ϣ����
 *
 *		@purpose	����������ʹ�õ�����Ϣ��ս���е���Ϣ���ֿ�,���ٱ���ʱ��
 *
 */
//ʵ��һ��ս��ר�õ���Ϣ��������   �����ṩ��Ч�ʷǳ���

const char B_SKILL_MASK[]					=	"B_SKILL_MASK";									//����������ɫ
const char B_WorldBoss_HurtUpdate[]			=	"B_WroldBoss_HurtUpdate";						//ˢ��ʱ��boss�˺�ֵ
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
const char MsgRoleDie[]						=	"ALIVEDIE";						//һ���佫����
const char B_ActObjectRemove[]				=	"ACTREMOVE";					//�佫�����Ƴ�
const char MsgAddBuff[]						=	"ADDBUFF";						//�佫���buff����
const char B_RemoveBuff[]					=	"REMOVEBUFF";					//�佫�Ƴ�buffʱ�ı���
const char B_UpdateBuffEffect[]				=	"UPBUFFEFFECT";					//ˢ���佫buffЧ��
const char B_PlayBeginAnimation[]			=	"PLAYERBEGINANIMATION";			//���ų�������
const char MsgStorOver[]					=	"WAR_STORY_OVER";				//ս������Ի�����
const char MsgReturnLayer[]					=	"MsgReturnLayer";				//��ʾ���ز�
const char B_CritEnd[]						=	"SKILLEND";						//�佫�����ͷ����
const char B_IntoBattle[]					=	"B_IntoBattle";					//��ʱ��ɽ���ս��
const char B_RoleSkill[]					=   "B_RoleSkill";					//�ҷ��佫�ͷż���
const char B_SkillCreateRole[]				=	"B_SkillCreateRole";			//���ܴ����佫
const char B_AttactNull[]					=	"B_AttactNull";					//��Ч������
const char B_AttackResult[]					=	"B_AttackResult";				//�����������
const char B_EntranceBattle[]				=	"B_EntranceBattle";				//�佫��������ս��
const char MsgCreateRoleObject[]			=	"MsgCreateRoleObject";			//������ʾ�佫
const char MsgBattleOver[]					=	"MsgBattleOver";				//ս������
const char MsgAutoBattle[]					=	"MsgAutoBattle";				//�Զ�ս��
const char MsgBattleStateChange[]			=	"MsgBattleStateChange";			//����״̬�ı�
const char MsgGetButtonState[]				=	"MsgGetButtonState";			//��ȡ��ť״̬
const char MsgSendButtonState[]				=	"MsgSendButtonState";			//���Ͱ�ť״̬
const char MsgButtonStateUpdate[]			=	"MsgButtonStateUpdate";			//��ť״̬ˢ��
const char MsgRoleEnterBattle[]				=	"MsgRoleInToBattle";			//��ɫ����ս��
const char MsgRoleGridChange[]				=	"MsgRoleGridChange";			//��ɫλ�øı�
const char MsgUpAutoSkillState[]			=	"MsgUpAutoSkillState";			//ˢ���Զ��ͷż���״̬
const char MsgRoleStand[]					=	"MsgRoleStand";					//���н�ɫվ��
const char MsgRoleBattleState[]				=	"MsgRoleBattleState";			//����ս��״̬�仯
const char MsgReleaseTouch[]				=	"MsgReleaseTouch";				//�ͷŽ�ɫ�㴥��
const char MsgStarActionEnd[]				=	"MsgStarActionEnd";				//�볡�������Ž���
const char MsgControlRemove[]				=	"MsgControlRemove";				//�Ƴ�����������
const char MsgCreateStory[]					=	"MsgCreateStory";				//��������
const char MsgMonsterDie[]					=	"MsgMonsterDie";				//��������
const char MsgHideControlUp[]				=	"MsgHideControlUp";				//���ؿ�������ϲ���
const char MsgNextBatchEnemy[]				=	"MsgNextBatchEnemy";			//������һ������
const char MsgUpBatchNumber[]				=	"MsgUpBatchNumber";				//ˢ�¹ؿ�����
const char MsgPlayWinEffect[]				=	"MsgPlayWinEffect";				//����ʤ����Ч
const char MsgShowStageWarning[]			=	"MsgShowStageWarning";			//��ʾ�ؿ�����
const char MsgChangeLayerLight[]			=	"MsgChangeLayerLight";			//�ı��ɫ������
const char MsgRoleVariant[]					=	"MsgRoleVariant";				//��ɫ����
const char MsgCreateSpineFloor[]			=	"MsgCreateSpineFloor";			//����spine�ذ���Ч
const char MsgDelayAttack[]					=	"MsgDelayAttack";				//�ӳٹ���Ч��
const char MsgInitBossBar[]					=	"MsgInitBossBar";				//��ʼ��boss����ʾ��
const char MsgUpdateBossHp[]				=	"MsgUpdateBossHp";				//ˢ��bossѪ��ֵ
#endif