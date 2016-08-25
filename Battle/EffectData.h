#ifndef EFFECTLOAD_H_
#define EFFECTLOAD_H_
/************************************************************* 
 *
 *
 *		Data : 2016.8.17
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : [10/16/2014 xc_lin] 效果信息管理
 *
 *
 *************************************************************/
#include "AppUI.h"
#include "Battle/BattleConfigMacro.h"
namespace BattleSpace
{
	class EffectInfo : public CCObject
	{
	public:
		EffectInfo();
		virtual bool init(){return true;}
		CREATE_FUNC(EffectInfo);
		CC_SYNTHESIZE(int,m_EFid,EFid);					//效果id
		CC_SYNTHESIZE(int,m_ActionID,ActionID);			//人物动作ID
		CC_SYNTHESIZE(int,usEft,usEft);					//施放特效ID
		CC_SYNTHESIZE(int,usMusicId,usMusicId);			//施放音效
		CC_SYNTHESIZE(int,foeEft,foeEft);				//受击特效ID
		CC_SYNTHESIZE(int,foeMusicId,foeMusicId);		//受击音效
		CC_SYNTHESIZE(int,FloorEf,FloorEf);				//地板特效ID 
		CC_SYNTHESIZE(int,isblack,isblack);				//是否需要黑屏
		CC_SYNTHESIZE(int,isShake,isShake);				//是否震屏
		void initEffectList(vector<int>& pVec) const;
	};

	class EffectData : public CCObject
	{
	public:
		EffectData();
		~EffectData();
		virtual bool init();
		CREATE_FUNC(EffectData);
		const EffectInfo* getEffectInfo(int EffectID) const;
	protected:
		map<int,EffectInfo*> warEffect;
		EffectInfo* mNullEffect;
	};


	/****************************************************************/
	class SpecialEffectInfo : public CCObject
	{
	public:
		SpecialEffectInfo();
		virtual bool init(){return true;}
		CREATE_FUNC(SpecialEffectInfo);
		CC_SYNTHESIZE(int,m_EffID,EffID);			//特效id
		CC_SYNTHESIZE(float,p_x,p_x);
		CC_SYNTHESIZE(float,p_y,p_y);
		CC_SYNTHESIZE(float,Acp_x,Acp_x);
		CC_SYNTHESIZE(float,Acp_y,Acp_y);
		CC_SYNTHESIZE(float,scale,scale);
		CC_SYNTHESIZE(float,speed,speed);
	};

	class SpecialEffectData : public CCObject
	{
	public:
		SpecialEffectData();
		~SpecialEffectData();
		virtual bool init();
		CREATE_FUNC(SpecialEffectData);
		SpecialEffectInfo* getSpecialEffect(int spId);
		bool JudgeSpcialEffect(int id);
	protected:
		map<int , SpecialEffectInfo*> mEffectMap;
	private:
		SpecialEffectInfo* mNullInfo;

	};

	/************************************************************************/

	class BuffEffect : public CCObject
	{
	public:
		BuffEffect();
		virtual bool init(){return true;}
		CREATE_FUNC(BuffEffect);
		CC_SYNTHESIZE(int,m_BuffType,BuffType);		//同一类型的buff使用同一类型特效(有是否为Debuf)
		CC_SYNTHESIZE(bool,m_Dbuf,Dbuf);			
		CC_SYNTHESIZE(int,small_icon,Small);
		CC_SYNTHESIZE(int,big_icon,Big);
		CC_SYNTHESIZE(int,effect_up,Effect_up);
		CC_SYNTHESIZE(bool,up_Loop,up_Loop);
		CC_SYNTHESIZE(int,effect_down,Effect_down);
		CC_SYNTHESIZE(bool,down_Loop,down_Loop);
	};
	typedef map<int,BuffEffect*> BuffEffMap;
	typedef map<bool,BuffEffMap> JsonEffectMap;

	class BuffConfig : public CCObject
	{
	public: 
		BuffConfig();
		~BuffConfig();
		virtual bool init ();
		CREATE_FUNC(BuffConfig);
		void ClearData();
		void AddBuffEffect(BuffEffect* info);
		const BuffEffect* getBuffEffect(int bufftype,bool Dbuf) const;			//根据buff类型得到相应的buff特效
	private:
		JsonEffectMap m_JsonEffectMap;
		BuffEffect* mNullEffect;
	};

	/************************************************************* 
	*
	*
	*		Data : 2016.4.14
	*	
	*		Name : 
	*
	*		Author : Lin_Xiancheng
	*
	*		Description : 
	*
	*
	*************************************************************/

	typedef std::pair<int,int> TypeAndStoryID;

	class StoryStep : public CCObject
	{
	public:
		StoryStep();
		virtual ~StoryStep(){};
		CREATE_FUNC(StoryStep);
	protected:
		inline bool init(){return true;}
	public:
		CC_SYNTHESIZE(int,m_storyId,storyId);
		CC_SYNTHESIZE(int,m_Texture,Texture);				//背景图
		CC_SYNTHESIZE(int,m_BGM,BGM);						//背景音乐
		CC_SYNTHESIZE(int,m_SFX,SFX);						//音效
		CC_SYNTHESIZE(int,m_RoleID,RoleID);					//角色图片
		CC_SYNTHESIZE(string,m_Action,Action);				//spine动作
		CC_SYNTHESIZE(bool,m_Spine,Spine);					//是否为spine			默认都是spine动画
		CC_SYNTHESIZE(bool,m_RoleShark,RoleShark);			//角色震动
		CC_SYNTHESIZE(int,m_Rolex,Rolex);					//角色偏移x
		CC_SYNTHESIZE(int,m_Roley,Roley);	
		CC_SYNTHESIZE(bool,m_right,right);					//右向
		CC_SYNTHESIZE(bool,m_Turn,Turn);					//翻转图片
		CC_SYNTHESIZE(string,m_RoleName,RoleName);			//角色名称
		CC_SYNTHESIZE(bool,m_Aside,Aside);					//是否为旁白
		CC_SYNTHESIZE(bool,m_Black,Black);					//是否变黑背景
		CC_SYNTHESIZE(float,m_time,time);					//旁白时间
		CC_SYNTHESIZE(string,m_Content,Conten);				
		CC_SYNTHESIZE(int,m_Cx,Cx);							//内容偏移x
		CC_SYNTHESIZE(int,m_Cy,Cy);
		CC_SYNTHESIZE(bool,m_ShowTalkBg,ShowTalkBg);		//是否显示谈话底框背景
		CC_SYNTHESIZE(bool,m_Shark,Shark);					//震动
	};

	class StoryData : public CCObject
	{
	public:
		~StoryData();
		virtual bool init();
		CREATE_FUNC(StoryData);
		void initStoryData(int stageid);
		void LoadFile(const char* FileName,int type);
		const vector<StoryStep*>* getStory(StoryType storytype) const;
		const map<StoryType,vector<StoryStep*>>& getStoryMap() const;
	private:
		map<StoryType,vector<StoryStep*>> m_MapStoryData;
		map<int,vector<TypeAndStoryID>> m_MapConfig;
		vector<int> m_SpineData;
	};
};
#endif