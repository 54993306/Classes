/************************************************************************/
/*      
用于存储战斗常使用到但是不经常修改的数值
开头用C_ 表示位常量
*/
/************************************************************************/

//帧率
const int		C_FrameNum			=5;					//每5帧执行一次逻辑计算
/************************************************************************/
//武将的动作帧率是24，游戏的帧率是60，武将的掉血最少间隔是2帧。
//从而将武将的最快遍历的时间应该是1/24*2 = 1/12，我们当前的帧率是1/60，而要求是帧率的5倍。
//即为刚好可以触发的情况是每5帧遍历一次所有武将的信息。
/************************************************************************/


//大怪物站位相关
const int		C_two_grid			=2;					//2个格子大怪物
const int		C_fore_grid			=4;					//4个格子大怪物

//动画相关
const float		walk_time_min		=0.5f;				//武将行走最小时间
const float		walk_time_max		=3.0f;				//武将行走最大时间

//伤害计算相关
const float		C_BUFFTIME			= 1.0f;				//BUFF触发时间
const int		C_VainNum			= 2;				//表示无效
const float     C_Erange			= 0.05f;			//伤害浮动值
const float		LoseHp_Interval		= 0.2f;				//每次掉血间隔时间
const float		C_Knum				= 1.0f;				//伤害所使用到的系数K
const float		C_Anum				= 1.0f;				//系数α
const float		C_Bnum				= 1.0f;				//系数β
const float		C_Rnum				= 1.0f;				//系数γ

const int		C_BaseHit			= 95;				//基础命中概率
const int		C_HitMin			= 70;				//命中Min
const int		C_HitMax			= 100;				//命中Max

const int		C_BaseCrit			= 2;				//基础暴击概率
const int		C_CritMin			= 0;				//暴击概率Min
const int		C_CritMax			= 30;				//暴击概率Max

const int		C_BaseCritHurt		= 150;				//基础暴伤
const int		C_CritHurtMin		= 150;				//暴伤Min
const int		C_CritHurtMax		= 220;				//暴伤Max

const int		BossBattle			= 100000;			//世界boss关卡id

//怒气值相关
const float		C_KillAddAnger		= 20/*50*/;			//杀死每个敌方增加的怒气值
const float		C_AtkAddAnger		= 3/*20*/;			//攻击敌方增加的怒气值
const int		C_HitAddAnger		= 10;				//受击回复的怒气值 每超过受击者最大血量5%则额外恢复5点最多一次恢复50点
const float		C_AddAngerPe		= 0.05f;			//伤害值超过最大血量百分之5
const int		C_AddPe				= 1/*5*/;			//每百分之5回复的怒气值
const int		C_EffNumMax			= 4;				//一个技能最多4个技能效果
const int		MainAlive			= 50;				//主帅位武将回复怒气值

//地图相关常量
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
const CCPoint	INVALID_POINT		= CCPoint(50,50);		//测试用可以看得见
#else
const CCPoint	INVALID_POINT		= CCPoint(-200,-200);
#endif
const int		C_GRIDOFFSET_X		= 4;				//格子间隙
const int		C_GRIDOFFSET_Y		= 2;				//格子间隙
const int		C_GRID_ROW			= 4;
const int		C_GRID_COL_MIN		= 16;				//可处理列数
const int		C_STAND_COL			= 24;				//增cost列数
const int		C_GRID_COL			= 32;
const int		C_MOSTERATKGRID		= 60;				//开始计算怪物攻击格子
const int		C_BEGINGRID			= 64;				//前面的格子为不可显示范围
const int		C_GRIDSHOWRECT		= 92;				//格子显示区域的变化(从地图转UI层)
const int		C_STANDGRID			= 96;				//武将怒气值增加的半屏
const int		C_CAPTAINGRID		= 124;				//主帅可移动范围格子
const int		C_CAPTAINSTAND		= 125;				//主帅初始站立格子
const int		C_ENDGRID			= 127;					

const CCRect	NULL_RECT			= CCRect(0,0,0,0);
const int		GRID_WIDTH			= 118;				//格子大小108*92
const int		GRID_HEIGHT			= 105;
const int		GRID_VGAP			= 4;
const int		GRID_HGAP			= 4;
const char		INVALID_GRID		= 0;
const int		TOPHEIGHT			= 162;
const int		BOTTOMHEIGHT		= 110;

//武将相关
const int		C_CallHero			= 500;				//我方英雄召唤武将
const int		C_Monst				= 1000;				//区分敌方(我方从1开始)
const int		C_BatchMonst		= 2000;				//伏兵ID
const int		C_ALIVE_OB			= 3000;				//显示的武将ID
const int		C_SceneTrap			= 4000;				//场景效果武将
const int		C_CallMonst			= 5000;				//敌方召唤怪物
const int		C_MODEID			= 10000;			//模型id

const int		C_PAUSE_TAG			= 20000;


//const ccColor4F WHITE_COLOR = {255, 255, 255, 1.0f};			//
//const ccColor4F GREEN_COLOR = {0,156,0,0.5f};					//
//const ccColor4F BLUE_COLOR = {0,0,255,0.2f};					//我方脚下颜色
//const ccColor4F ENEMY_GRID_COLOR = {255, 0, 0, 0.15f};		//敌方脚下颜色	
//const ccColor4F HERO_GRID_COLOR = {0, 255, 0, 0.15f};			//可移动区域
//const ccColor4F SKILL_AREA = {255 , 0 , 0 ,0.15f};			//技能施放区域颜色
//const ccColor4F ATTACK_AREA = {255 , 0 , 255 ,0.25f};			//技能施放区域颜色
//const ccColor4F CURE_AREA = {255 , 255 , 0 ,0.25f};			//技能施放区域颜色
//const ccColor4F WOOD_GRID_COLOR = {204, 0, 204, 0.15f};		//伏兵颜色