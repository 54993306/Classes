/************************************************************************/
/*     
	定义骨骼动画相关内容
*/
/************************************************************************/
static const char* Stand_Action					= "standby";			/*站立			 1*/
static const char* Walk_Action					= "move";				/*行走			 2*/
static const char* Hit_Action					= "hit";				/*受击			 3*/
static const char* Dizzy_Action					= "dizzy";				/*晕眩			 4*/
static const char* Attack_Action				= "attack01";			/*攻击			 5*/
static const char* SpAttack_Action				= "attack02";			/*特殊攻击		 6*/
static const char* Skill_Action					= "skill";				/*主动技能		 7*/
static const char* Win_Action					= "win";				/*胜利动作		 8*/
static const char* Die_Action					= "die";				/*死亡			 9*/
static const char* Start_Action					= "start";				/*入场			 10*/


static const char* EVE_CONTINUOUS				= "continuous";			/*连击判断			*/
static const char* EVE_ATKBEGIN					= "AtkBegin";			/*攻击开始			*/
static const char* EVE_ATKEND					= "AtkEnd";				/*攻击结束			*/
static const char* EVE_LOSTHP					= "LostHp";				/*掉血显示			*/ 
static const char* EVE_REPEAT					= "Repeat";				/*重复标记			*/ 
static const char* EVE_SHAKE					= "Shake";				/*震屏标记			*/
static const char* EVE_ANIMAT					= "Animat";				/*播放动画			*/
static const char* EVE_SOUND					= "Sound";				/*播放声效			*/
static const char* EVE_MASK						= "mask";				/*技能遮罩			*/