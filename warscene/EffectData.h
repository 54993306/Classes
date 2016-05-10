#ifndef EFFECTLOAD_H_
#define EFFECTLOAD_H_
#include "AppUI.h"

/************************************************************************/
/*         
							效果信息管理
						//  [10/16/2014 xc_lin]
*/
/************************************************************************/
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
	CCArray* getEffIdList();
private:

};

typedef map<unsigned int,EffectInfo*> WarEffect;	

class EffectData : public CCObject
{
public:
	~EffectData();
	virtual bool init();
	CREATE_FUNC(EffectData);
	EffectInfo* getEffectInfo(int EffectID);
	CCArray* getEffs();
protected:
	WarEffect warEffect;
private:

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
private:

};


typedef map<unsigned int , SpecialEffectInfo*> SpEff;

class SpecialEffectData : public CCObject
{
public:
	~SpecialEffectData();
	virtual bool init();
	CREATE_FUNC(SpecialEffectData);
	SpecialEffectInfo* getspEff(int spId);
	int getSpcialEfectId(int spid);
	bool JudgeSpcialEffect(int id);
	CCArray* getspEffs();
protected:
	SpEff spEff;

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

class BuffData : public CCObject
{
public: 
	~BuffData();
	virtual bool init ();
	CREATE_FUNC(BuffData);
	void ClearData();
	void AddBuffEffect(BuffEffect* info);
	BuffEffect* getBuffEffect(int bufftype,bool Dbuf);			//根据buff类型得到相应的buff特效
private:
	JsonEffectMap m_JsonEffectMap;
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

enum struct StoryType
{
	beginStory = 1,
	MoveEndStory,
	OverStory,
};			//剧情类型,根据不同类型会有不同结束处理

class StoryData : public CCObject
{
public:
	StoryData();
	~StoryData();
	virtual bool init();
	CREATE_FUNC(StoryData);
	void initStoryData(int stageid);
	void LoadFile(const char* FileName,int type);
	vector<StoryStep*>* getStory(StoryType storytype);
	map<StoryType,vector<StoryStep*>>* getStoryMap();
private:
	map<StoryType,vector<StoryStep*>> m_MapStoryData;
	map<int,vector<TypeAndStoryID>> m_MapConfig;
	vector<int> m_SpineData;
	//vector
};

#endif