#ifndef _MESSAGE_H_
#define _MESSAGE_H_
const char Msg_DropItem[]				= "Msg_DropItem";
const char Draw_Skill_Effect[]			= "Draw_Skill_Effect";			//武将效果区域
const char Drop_Item_NumDispose[]		= "Drop_Item_NumDispose";		//物品掉落数量处理
const char Trap_Effect[]				= "Trap_Effect";				//关卡陷阱效果
const char ComBatMove_MSG[]				= "ComBatMove_MSG";				//战斗中移动
const char Draw_SKILL_Area[]			= "Draw_SKILL_Area";			//绘制技能伤害区域
const char Draw_BtmSkill_Area[]			= "Draw_BtmSkill_Area";			//技能按钮绘制区域
const char Remove_SkillTips[]			= "Remove_SkillTips";			//移除技能提示
const char DispalyGridByType[]			= "DispalyGridByType";			//根据类型显示格子
const char hideGridByType[]				= "hideGridByType";				//根据类型显示格子
const char SHAKE_BYEFFECT[]				= "SHAKE_BYEFFECT";				//震屏消息
const char BEATBACK[]					= "BEATBACK";					//击退效果处理
const char BEATFRONT[]					= "BEATFRONT";					//前移效果处理
const char ALIVEDIE[]					= "ALIVEDIE";					//一个武将死亡
const char ACTREMOVE[]					= "ACTREMOVE";					//武将死亡移除
const char ALIVEBATTLETOUCH[]			= "ALIVEBATTLETOUCH";			//武将上阵触摸
const char ALIVEBATTLET[]				= "ALIVEBATTLE";				//武将上阵
const char ALIVEATTACK[]				= "ALIVEATTACK";				//武将攻击
const char CANCELATTACKAREA[]			= "CANCELATTACKAREA";			//取消回执武将攻击范围
const char CHANGEBATTLESPEED[]			= "CHANGEBATTLESPEED";			//改变战斗速度
const char TOGGLE_GAME_EDITOR[]			= "TOGGLE_GAME_EDITOR";
const char LAYERMOVEEND[]				= "LAYERMOVEEND";				//开场层移动完毕
const char TIPSEFFECT[]					= "TIPSEFFECT";					//怪物提示特效
const char CHANGECOST[]					= "CHANGECOST";					//改变当前怒气值消息
const char ADDBUFF[]					= "ADDBUFF";					//武将添加buff表现
const char REMOVEBUFF[]					= "REMOVEBUFF";					//武将移除buff时的表现
const char UPBUFFEFFECT[]				= "UPBUFFEFFECT";				//刷新武将buff效果
const char CAPTAINHIT[]					= "CAPTAINHIT";					//主帅受击效果
const char UPSKILLBUTTON[]				= "UPSKILLBUTTON";				//刷新技能按钮状态
const char PLAYERBEGINANIMATION[]		= "PLAYERBEGINANIMATION";		//播放出场动画
const char RECORDBATTERNUM[]			= "RECORDBATTERNUM";			//记录必杀技连击信息

const char WAR_ROUND_START[]			= "WAR_ROUND_START";			//回合开始
const char BATCH_FOR_BAR[]				= "BATCH_FOR_BAR";				//回合开始
const char NEXT_ROUND_FOR_BAR[]			= "NEXT_ROUND_FOR_BAR";
const char NEXT_ROUND_FADEIN[]			= "NEXT_ROUND_FADEIN";
const char SHOW_SKILL_CONTROL[]			= "SHOW_SKILL_CONTROL";			//显示主动技能控制面板
const char CHANGE_HERO_STATE[]			= "CHANGE_HERO_STATE";			//改变人物状态为释放必杀技状态
const char WARSCENE_EXIT[]				= "WARSCENE_EXIT";				//回合开始
const char WAR_DRAW_DRAG_AREA[]			= "WAR_DRAW_DRAG_AREA";			//开始绘制可拖拽区域
const char ALIVE_EFFECT_AREA[]			= "ALIVE_EFFECT_AREA";			//武将效果区域
const char WAR_CANCEL_DRAW_DRAG[]		= "WAR_CANCEL_DRAW_DRAG";		//取消绘制拖拽区域
const char TOUCH_ALIVE[]				= "TOUCH_ALIVE";				//点击武将
const char TOUCH_ALIVE_CANCEL[]			= "TOUCH_ALIVE_CANCEL";			//取消点击武将
const char BAR_ATK_HANDLE[]				= "BAR_ATK_HANDLE";				//BAR关于武将攻击处理
const char BAR_ADD_ICOM[]				= "BAR_ADD_ICOM";				//BAR添加新的Icon
const char SEND_ALIVE_ID[]				= "SEND_ALIVE_ID";				//发送武将ID
const char WAR_STORY_OVER[]				= "WAR_STORY_OVER";				//战斗剧情对话结束
const char COST_AREA[]					= "STORY_OVER";					//COST变化区域
const char EMBATTLE_MAIN_CHANGE[]		= "EMBATTLE_MAIN_CHANGE";		//布阵主帅位武将变化
const char EMBATTLE_SUB_CHANGE[]		= "EMBATTLE_SUB_CHANGE";		//布阵其它武将变化
const char EMBATTLE_SELECT_WUJIANG[]	= "EMBATTLE_SELECT_WUJIANG";	//选中武将
const char UP_Rage[]					= "UP_Rage";					//更新武将UI怒气值
const char NIGHT_CONTROL[]				= "NIGHT_CONTROL";				//夜视模式
const char MOVE_ATK_AREA[]				= "MOVE_ATK_AREA";				//移动区域受击判断
const char ALIVE_OBJECT_CLICKED[]		= "ALIVE_OBJECT_CLICKED";		//角色或怪物被按中
const char CHANGE_MOVE_STATE[]			= "CHANGE_MOVE_STATE";			//改变移动对象状态
const char SHOW_PLAN_PANEL[]			= "SHOW_PLAN_PANEL";			//显示当前关卡进度
const char REMOVE_SETPANEL[]			= "REMOVE_SETPANEL";			//插播主帅技动画
const char DISPLAY_AGILITY[]			= "DISPLAY_AGILITY";			//显示隐藏出手顺序
const char DISPLAY_ALIVELAYERBG[]		= "DISPLAY_ALIVELAYERBG";		//显示隐藏人物层黑色底
const char SKILLEND[]					= "SKILLEND";					//武将技能释放完毕
const char CHANGE_SKILL[]				= "CHANGESKILL";				//
const char CHANGE_HERO[]				= "CHANGEHERO";
const char UPDATE_HERO[]				= "UPDATEROLE";
const char CHANGE_EQUIPMENT[]			= "CHANGEEQUIPMENT";
const char GARRSION_HERO[]				= "GARRSION_HERO";

const char TASK_NOTICE[]				= "TASK_NOTICE";
const char MAIL_NOTICE[]				= "MAIL_NOTICE";

const char UPDATE_BUILD[]				= "UPDATE_BUILD";

const char UPDATE_RECRUIT[]				= "UPDATE_RECRUIT";   //招募

const char DELETE_TASK[]				= "DELETE_TASK";

const char UPDATE_WEIRD[]				= "UPDATE_WEIRD";

const char UPDATE_HEROINFO[]			= "UPDATE_HEROINFO";      //更新hero属性
const char UPDATE_HEROEXP[]				= "UPDATE_HEROEXP";      //更新hero exp
 
const char CULTIVATE_GET_MONEY[]				= "CULTIVATE_GET_MONEY";      //培植区收获
const char UPDATE_VIPINFO[]				= "UPDATE_VIPINFO";
const char UPDATE_FUNCTIONOPEN[]		= "UPDATE_FUNCTIONOPEN";

const char HIDE_MAIN_SCENE[]				= "HIDE_MAIN_SCENE";				//隐藏主场景背景
const char SHOW_MAIN_SCENE[]				= "SHOW_MAIN_SCENE";				//显示主场景背景

const char HIDE_RECRUIT[]				= "HIDE_RECRUIT";				//隐藏酒馆层
const char SHOW_RECRUIT[]				= "SHOW_RECRUIT";				//显示酒馆层

const char OPENE_SELECT_ARMY[]				= "OPENE_SELECT_ARMY";				//队伍选择界面打开消息
const char CLOSE_SELECT_ARMY[]				= "CLOSE_SELECT_ARMY";				//队伍选择界面关闭消息


const char SHOW_TOP_LAYER[]				= "SHOW_TOP_LAYER";				//显示toplayer
const char HIDE_TOP_LAYER[]				= "HIDE_TOP_LAYER";				//隐藏toplayer
const char UPDATE_GAMETIP[]				= "UPDATE_GAMETIP";				

const char WARPREVIEW_EXIT[]				= "WARPREVIEW_EXIT";					

const char COMPOSE_SUCCESS[]				= "COMPOSE_SUCCESS";			//调和成功
const char SHOW_HEAD[]				= "SHOW_HEAD";

const char RECEIVE_BATTLE_INFO[]				= "RECEIVE_BATTLE_INFO";	//接收到了战斗数据

const char UPDATE_EQUIP_STOCK_COUNT[]				= "UPDATE_EQUIP_STOCK_COUNT";		//更新库存

const char SAME_EQUIP_STARUP_LEVELUP[]				= "SAME_EQUIP_STARUP_LEVELUP";		//升级和进阶相同的装备提示

const char UPDATE_ITEM_DATA[]				= "UPDATE_ITEM_DATA";		//更新装备信息

const char HIDE_ROLE_INFO[]				= "HIDE_ROLE_INFO";					//隐藏角色信息界面
const char SHOW_ROLE_INFO[]				= "SHOW_ROLE_INFO";			//显示角色信息界面

#endif // !_MESSAGE_H_
