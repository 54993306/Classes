#ifndef _COMBATGUIDEDATA_
#define _COMBATGUIDEDATA_

/************************************************************************/
//
//							处理json文件信息
//							
/************************************************************************/
#include "AppUI.h"
#include "CocoStudio/Json/rapidjson/rapidjson.h"
namespace BattleSpace{

	enum Sprite_Type
	{
		NULL_Type = 0,
		Hand_Type,				//手型图标
		Arraw_Type,				//箭型图标
	};
	enum CombatGridType
	{
		ImageText_Type= 1,		//图文类型
		UI_Type,				//释放技能
		AliveMove_Type,			//武将移位类型
		CallAalive_Type,		//召唤武将类型
		Batter_Type,			//连击引导
	};
	enum GridDirection			//引导的精灵指向
	{
		D_Default,
		D_Up = 1,
		D_Down,
		D_Left,
		D_Right,
		D_Left_Down,
		D_Right_Down,
		D_Left_Up,
		D_Right_Up,
	};
	enum UIGuideType
	{
		generalBtn= 1,				//主帅技能按钮
		skill2Btn,
		skill3Btn,
		skill4Btn,
		skill5Btn,
	};
	struct AliveCompel			//强制将武将设置到某个模型
	{
		int model;				//被强制武将模型ID	
		int grid;				//被强制到的目标位置
	};

	struct ImageData
	{
		std::string sFile;
		bool isSpine;
		int spineID;
		float fScale;
		CCPoint pos;
		bool bFlipX;
		bool bFlipY;
		int iRotation;
		int iDir;
		int iZorder;
		int SendMsg;
		ImageData():SendMsg(0),iZorder(0),iDir(0),iRotation(0),bFlipY(0),bFlipX(0),fScale(0),sFile(""),pos(CCPointZero),isSpine(false),spineID(0){}
	};

	//在移动结束的地方判断,当前是否为新手引导状态,是否为移位类型的引导
	class CombatGuideStep : public CCObject
	{
	public:
		CombatGuideStep();
		virtual bool init(){return true;}
		CREATE_FUNC(CombatGuideStep);
		CC_SYNTHESIZE(int,m_id,Id);						//引导ID
		CC_SYNTHESIZE(int,m_backID,BackID);				//上一步ID
		CC_SYNTHESIZE(int,m_nextID,NextID);				//下一步ID
		CC_SYNTHESIZE(int,m_type,Type);					//引导类型(1、移位类型，2、点击UI类型，3、)
		CC_SYNTHESIZE(int,m_uiType,UiType);				//确定是哪个UI
		CC_SYNTHESIZE(int,m_begingrid,Begingrid);		//移动开始格子(移位类型引导使用)//移动类引导使用
		CC_SYNTHESIZE(int,m_finishgrid,Finishgrid);		//移动结束格子(移位类型引导使用)
		CC_SYNTHESIZE(int,m_RoleImage,RoleImage);		//头像
		CC_SYNTHESIZE(bool, m_bRoleFlipX, RoleFlipX);		//头像翻转
		CC_SYNTHESIZE(float, m_fRoleScale, RoleScale);		//头像缩放
		CCPoint m_RolePoint;							//头像位置
		CC_SYNTHESIZE(string,m_Name,Name);				//名称
		CCPoint m_NamePoint;							//名称位置
		CC_SYNTHESIZE(string,m_Describe,Describe);		//内容(判断是否为“”,是否存在文字内容)
		CC_SYNTHESIZE(int, m_DescribeFontColor, DescribeFontColor);		//文字颜色
		CC_SYNTHESIZE(int, m_DescribeAligment, DescribeAligment);		//文字排版
		CCPoint m_DescribeAnchor;																		//文字锚点
		CCSize m_DescribeDimensions;																//文字换行
		CCPoint m_Despoint;								//内容位置		
		CC_SYNTHESIZE(int,m_size,size);					//文字大小
		CC_SYNTHESIZE(bool,m_Captain,Captain);			//主帅标记
		CC_SYNTHESIZE(bool,m_BlackLayer,BlackLayer);	//是否显示黑屏
		CC_SYNTHESIZE(bool,m_BlackBottom, BlackBottom);	//是否显示对话框黑底
		CC_SYNTHESIZE(bool,m_Reset,Reset);				//是否执行重置操作
		CC_SYNTHESIZE(int,m_AddCost,AddCost);			//添加cost值
		CC_SYNTHESIZE(bool,m_LockLayer,LockLayer);		//锁住移动层
		vector<AliveCompel> m_VecCompel;				//存储武将将要移动的位置信息(其他我方多余角色执行死亡操作)
		CC_SYNTHESIZE(bool,m_Finish,Finish);			//是否执行重置操作
		vector<ImageData> m_imageData;
	private:

	};

};
	//JSON格式的冒号前后不能多空格
	//{
	//	"guid": 
	//	[
	//		{
	//			"id": 1,
	//			"backid": 0,
	//			"nextid": 2,
	//			"type": 3,
	//			"Uitype":0,
	//			"beginGrid":0,
	//			"FnishGrid":0,
	//			"RoleImage": "2046",
	//			"RolePoint_x": 200,
	//			"RolePoint_y": 300,
	//			"Name":"老板娘",
	//			"Name_x":0,
	//			"Name_y":0,
	//			"describe": "小帥哥，這副生面孔應該是新來的吧?",
	//			"desc_x": 350,
	//			"desc_y": 148,
	//			"Captain":false,
	//			"Reset":false,
	//			"AddCost":0,
	//			"moveDispose": 
	//			[
	//				{
	//					"ModeID": 506,
	//					"grid": 100
	//				},
	//				{
	//					"ModeID": 516,
	//					"grid": 100
	//				}
	//			]
	//		}
	//	]
	//}
#endif