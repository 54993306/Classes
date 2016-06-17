
#include "Battle/LoadSpineData.h"
#include <spine/spine-cocos2dx.h>
#include "model/DataCenter.h"
#include "model/WarManager.h"
#include "Battle/EffectData.h"
#include "Battle/BattleTools.h"
using namespace spine;
using namespace cocos2d;
using namespace std;
namespace BattleSpace{
	LoadSpineData::LoadSpineData()
		:m_LoadSucceed(false),m_LoadSpineNum(0),m_TotalSpineNum(0)
	{}

	LoadSpineData::~LoadSpineData()
	{}

	//������Ƿ�����һ����������������ȷ�������ڼ��غ͹���spine����Դ,ʵ�����Ѿ����Գ��ˡ�ʱ��������
	//���洢����ʹ�ù���ģ������̫������������ά��һ�����õ��佫ģ���б�����ģ�͸�����������Ϊ10����������ʱ���ͷ��������ټ����µġ�
	//ά��һ�����õ�ģ���б�
	bool LoadSpineData::init()
	{
		m_Manage = DataCenter::sharedData()->getWar();
		return true;
	}

	void LoadSpineData::AddRoleSpineID( int id )
	{
		//m_VecStep.assign(vec->begin(),vec->end());				//��ʹ�ô���һ��vector�ķ�����vector��ֵ
		m_VecRole.push_back(id);
	}

	void LoadSpineData::LoadSpineAnimation()
	{
		if (m_VecRole.empty() && m_VecStory.empty())
		{
			m_LoadSucceed = true;
			CCLOG("[ *TIPS ]LoadSpineData::LoadTexture is Null");
			return;
		}
		LoadVecRoleData();
		LoadVecStoryData();
	}
	//  id + ����  �õ������ļ���   ��ǰ��3�֣���ɫ����ɫ��Ч������spine
	void LoadSpineData::LoadVecRoleData()
	{
		VectorRemoveRepeat(m_VecRole);
		for (auto i : m_VecRole)
			LoadRoleData(i);
	}

	void LoadSpineData::LoadRoleData( int id )
	{
		char str[60] = {0};
		sprintf(str,"Spine/%d",id);
		RealLoadTexture(str);					//����ģ��ͼƬ
		sprintf(str,"Spine/%d_texiao",id);
		RealLoadTexture(str);					//������ЧͼƬ
	}

	void LoadSpineData::AddStorySpineID( int id )
	{
		m_VecStory.push_back(id);
	}

	void LoadSpineData::initStoryData()
	{
		map<StoryType,vector<StoryStep*>>* storymap = DataCenter::sharedData()->getWar()->getStoryData()->getStoryMap();

		for (auto i : *storymap)
			for (auto j:i.second)
				if (j->getSpine()&&j->getRoleID())
					m_VecStory.push_back(j->getRoleID());
		VectorRemoveRepeat(m_VecStory);
	}

	void LoadSpineData::LoadVecStoryData()
	{
		initStoryData();
		for (auto j : m_VecStory)
			LoadStoryData(j);
	}

	void LoadSpineData::LoadStoryData(int id)
	{
		char str[60] = {0};
		sprintf(str,"storyImage/Spine/%d",id);
		RealLoadTexture(str);					//����ģ��ͼƬ
	}

	void LoadSpineData::RealLoadTexture( const char* str )
	{
		char pStr[60] = {0};
		sprintf(pStr,"%s.png",str);
		if (!AsyncLoadTexture(pStr))
		{
			CCLOG("[ *ERROR ]LoadSpineData::RealLoadTxture %s Is NULL",pStr);
			return ;
		}
		int MaxImageNum = 10;						//����һ��spine�������10��ͼƬ
		for(int i=2;i < MaxImageNum;i++)
		{
			sprintf(pStr,"%s%d.png",str,i);
			if (!AsyncLoadTexture(pStr))
				break;
		}
	}

	bool LoadSpineData::AsyncLoadTexture(const char* str)
	{
		std::string strFullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(str);
		if(!CCFileUtils::sharedFileUtils()->isFileExist(strFullPath))
		{
			//CCLOG("[ *Tips ]LoadSpineData::AsyncLoadTexture %s Is NULL",str);							//��Ӧ�ô���ô��Log�ġ�
			return false;
		}
		if (!CCTextureCache::sharedTextureCache()->textureForKey(str))
		{
			m_TotalSpineNum++;
			CCTextureCache::sharedTextureCache()->addImageAsync(str,this,callfuncO_selector(LoadSpineData::LoadTextureCallBack));
			CCLOG("LoadSpineData::SpineThread Load %s",str);
		}
		return true;
	}

	void LoadSpineData::LoadTextureCallBack( CCObject* ob )
	{
		m_LoadSpineNum++;
		if (m_LoadSpineNum < m_TotalSpineNum )
			return;
		LoadStoryAction();
		LoadRoleAction();
		m_LoadSucceed = true;
		//std::function<void()> Fun = std::bind(&LoadSpineData::LoadSpineAction,this,m_VecSpine);
		//std::thread LoadThread(Fun);
		//LoadThread.detach();
		//std::function<bool(vector<int>&,bool)> Fun = std::bind(&LoadWar::SpineThread,this,std::placeholders::_1,std::placeholders::_2);
		//std::packaged_task<bool(vector<int>&,bool)>dTask(Fun);		//spine�첽���ش���
		//pFuture = dTask.get_future();
		//std::thread cthread(std::move(dTask),m_WarResouse.find(ResourceType::Load_Spine)->second,true);
		//cthread.detach();
		//m_InitFuture = true;
	}

	void LoadSpineData::LoadRoleAction()
	{
		for (auto i : m_VecRole)
		{
			char c_json[60]   = {0};
			char c_altlas[60] = {0};
			char c_effect[60] = {0};
			sprintf(c_json,"Spine/%d.json",i);
			sprintf(c_altlas,"Spine/%d.atlas",i);
			LoadSpineAction(c_json,c_altlas,ToString(i));
			sprintf(c_json,"Spine/%d_texiao.json",i);
			sprintf(c_altlas,"Spine/%d_texiao.atlas",i);
			sprintf(c_effect,"%d_texiao",i);
			LoadSpineAction(c_json,c_altlas,c_effect);
		}
	}

	void LoadSpineData::LoadStoryAction()
	{
		for (auto i: m_VecStory)
		{
			char c_json[60]   = {0};
			char c_altlas[60] = {0};
			sprintf(c_json,"storyImage/Spine/%d.json",i);
			sprintf(c_altlas,"storyImage/Spine/%d.atlas",i);
			LoadSpineAction(c_json,c_altlas,ToString(i));
		}
	}

	void LoadSpineData::LoadSpineAction( const char* c_json,const char* c_altlas,const char* key )
	{
		if (m_Manage->getSpineData(key))
		{
			CCLOG("[ *Tips ] LoadWar::RealLoadAction Load Spine Repeat =%s",key);
			return;
		}
		std::string strFullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(c_json);
		if(!CCFileUtils::sharedFileUtils()->isFileExist(strFullPath))
		{
			CCLOG(" [ *Tips ] LoadSpineData::RealLoadAction Json File No Find %s",c_json);
			return;
		}
		strFullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(c_altlas);
		if(!CCFileUtils::sharedFileUtils()->isFileExist(strFullPath))
		{
			CCLOG(" [ *Tips ] LoadSpineData::RealLoadAction altlas File No Find %s",c_altlas);
			return;
		}
		//CCLOG("1______%s", c_altlas);
		spAtlas*atlas = spAtlas_createFromFile(c_altlas, 0);
		//CCLOG("2______%s", c_altlas);
		CCAssert(atlas, "Error reading atlas file.");
		spSkeletonJson* atlas_json = spSkeletonJson_create(atlas);
		//CCLOG("3______%s", c_altlas);
		atlas_json->scale = 1.0f;//����������С
		spSkeletonData* skeletonData = spSkeletonJson_readSkeletonDataFile(atlas_json, c_json);
		CCAssert(skeletonData, atlas_json->error ? atlas_json->error : "Error reading skeleton data file.");
		//CCLOG("4______%s", c_altlas);
		spSkeletonJson_dispose(atlas_json);
		//CCLOG("5______%s", c_altlas);
		AddSpineDataInMap(key,skeletonData,atlas);
	}

	void LoadSpineData::AddSpineDataInMap( const char* name,spSkeletonData* skeletonData,spAtlas*atlas )
	{
		if (m_Manage->getSpineData(name))
		{
			CCLOG("[ *Tips ]LoadSpineData::AddSpineDataInMap Add Repead = %s",name);
			return;
		}
		m_Manage->getSpineMap()[name] = make_pair(skeletonData,atlas);
		CCLOG("LoadSpineData::AddSpineDataInMap  Load Succeed SpineName = %s",name);
	}

	spSkeletonData* LoadSpineData::loadSpineImmediately( const char* key,const char* path )
	{
		char c_json[60]   = {0};
		char c_altlas[60] = {0};
		char c_png[60] = {0};
		sprintf(c_json,"%s.json",path);
		sprintf(c_altlas,"%s.atlas",path);
		sprintf(c_png,"%s.png",path);
		std::string sFullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(c_png);
		if(!CCFileUtils::sharedFileUtils()->isFileExist(sFullPath))
		{
			sprintf(c_json,"storyImage/Spine/888756.json");
			sprintf(c_altlas,"storyImage/Spine/888756.atlas");
			CCLOG("[ *ERROR ]LoadSpineData::loadSpineImmediately not foud-%s", path);
		}
		LoadSpineAction(c_json,c_altlas,key);
		return m_Manage->getSpineData(key)->first;
	}
};
