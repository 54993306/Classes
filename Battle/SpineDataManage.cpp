
#include "Battle/SpineDataManage.h"
#include <spine/spine-cocos2dx.h>
#include "Battle/BattleCenter.h"
#include "Battle/WarManager.h"
#include "Battle/EffectData.h"
#include "Battle/BattleTools.h"
#include "Battle/ParseFileData.h"
#include "Battle/Config/ConfigManage.h"
using namespace spine;
using namespace cocos2d;
using namespace std;
namespace BattleSpace
{
	SpineDataManage::SpineDataManage()
	:mLoadSucceed(false),mLoadNumber(0),mTotalNumber(0)
	{}

	SpineDataManage::~SpineDataManage()
	{
		ReleaseSpineData();
	}

	bool SpineDataManage::init()
	{
		//���洢����ʹ�ù���ģ������̫������������ά��һ�����õ��佫ģ���б�����ģ�͸�����������Ϊ10����������ʱ���ͷ��������ټ����µġ�
		//ά��һ�����õ�ģ���б�
		parseSpineModelFile(mSpineModels);
		return true;
	}

	void SpineDataManage::ReleaseSpineData()
	{
		CCLOG("/******************* Release SpineData ***************************/ \n");
		for (auto tPair:mMapData)
		{
			spSkeletonData_dispose(tPair.second.first);
			spAtlas_dispose(tPair.second.second);
			CCLOG("SpineDataManage::ReleaseData Release %s",tPair.first.c_str());
		}
		mMapData.clear();
		mVecRole.clear();
		mVecStory.clear();
		mTrapVec.clear();
		mLoadNumber = 0;
		mTotalNumber = 0;
		mLoadSucceed = false;
	}

	bool SpineDataManage::isSpineModel( int pModel )
	{
		vector<int>::iterator tSpine = mSpineModels.end();
		if (std::find(mSpineModels.begin(),tSpine,pModel) != tSpine)
			return true;
		return false;
	}

	SpineData* SpineDataManage::getSpineData( string pKey )
	{
		map<std::string,SpineData>::iterator iter = mMapData.find(pKey);
		if (iter != mMapData.end())
			return &iter->second;
		return nullptr;
	}

	spSkeletonData* SpineDataManage::loadSpineImmediately( const char* pKey,const char* pPath )
	{
		char tJson[60]   = {0};
		char tAtlas[60] = {0};
		char tPng[60] = {0};
		sprintf(tJson,"%s.json",pPath);
		sprintf(tAtlas,"%s.atlas",pPath);
		sprintf(tPng,"%s.png",pPath);
		std::string sFullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(tPng);
		if(!CCFileUtils::sharedFileUtils()->isFileExist(sFullPath))
		{
			sprintf(tJson,"storyImage/Spine/888756.json");
			sprintf(tAtlas,"storyImage/Spine/888756.atlas");
			CCLOG("[ *ERROR ]LoadSpineData::loadSpineImmediately not foud-%s", pPath);
		}
		LoadActionData(tJson,tAtlas,pKey);
		return getSpineData(pKey)->first;
	}

	void SpineDataManage::AddRoleSpineID( int pRoleID )
	{
		mVecRole.push_back(pRoleID);
	}

	void SpineDataManage::AddTrapID( int pTrapID )
	{
		mTrapVec.push_back(pTrapID);
	}

	void SpineDataManage::LoadSpineAnimation()
	{
		LoadVecRoleData();
		LoadVecStoryData();
		LoadVecTrap();
	}
	//  id + ����  �õ������ļ���   ��ǰ��3�֣���ɫ����ɫ��Ч������spine
	void SpineDataManage::LoadVecRoleData()
	{
		VectorUnique(mVecRole);
		for (auto tRole : mVecRole)
			LoadRoleData(tRole);
	}

	void SpineDataManage::LoadRoleData( int pRoleID )
	{
		char tPath[60] = {0};
		sprintf(tPath,"Spine/%d",pRoleID);
		LoadTexture(tPath);					//����ģ��ͼƬ
		sprintf(tPath,"Spine/%d_texiao",pRoleID);
		LoadTexture(tPath);					//������ЧͼƬ
	}
	//��ʼ��������Ҫ��spine����
	void SpineDataManage::LoadVecStoryData()
	{
		for (auto i : BattleConfig->getStoryData()->getStoryMap())
			for (auto tStoryRole:i.second)
				if (tStoryRole->getSpine()&&tStoryRole->getRoleID())
					mVecStory.push_back(tStoryRole->getRoleID());

		VectorUnique(mVecStory);
		for (auto tStoryRole : mVecStory)
			LoadStoryData(tStoryRole);
	}

	void SpineDataManage::LoadStoryData(int pStoryRole)
	{
		char path[60] = {0};
		sprintf(path,"storyImage/Spine/%d",pStoryRole);
		LoadTexture(path);					//����ģ��ͼƬ
	}

	void SpineDataManage::LoadVecTrap()
	{
		VectorUnique(mTrapVec);
		for (auto tTrap : mTrapVec)
			LoadTrapData(tTrap);
	}

	void SpineDataManage::LoadTrapData( int pTrap )
	{
		char path[60] = {0};
		sprintf(path,"Trap/%d",pTrap);
		LoadTexture(path);					//����ģ��ͼƬ
	}

	void SpineDataManage::LoadTexture( const char* pPath )
	{
		char tFileName[60] = {0};
		sprintf(tFileName,"%s.png",pPath);
		if (!AsyncLoadTexture(tFileName))
		{
			CCLOG("[ *Tips ]LoadSpineData::RealLoadTxture %s Is NULL",tFileName);
			return ;
		}
		int MaxImageNum = 10;						//����һ��spine�������10��ͼƬ
		for(int i=2;i < MaxImageNum;i++)
		{
			sprintf(tFileName,"%s%d.png",pPath,i);
			if (!AsyncLoadTexture(tFileName))
				break;
		}
	}

	bool SpineDataManage::AsyncLoadTexture(const char* pFilePath)
	{
		std::string tFullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(pFilePath);
		if(!CCFileUtils::sharedFileUtils()->isFileExist(tFullPath))			//�ж��ļ��Ƿ����
			return false;
		if (!CCTextureCache::sharedTextureCache()->textureForKey(pFilePath))
		{
			mTotalNumber++;
			CCTextureCache::sharedTextureCache()->addImageAsync(pFilePath,this,callfuncO_selector(SpineDataManage::AsyncLoadCallBack));
			CCLOG("LoadSpineData::SpineThread Load %s",pFilePath);
		}
		return true;
	}

	void SpineDataManage::AsyncLoadCallBack( CCObject* ob )
	{
		mLoadNumber++;
		if (mLoadNumber < mTotalNumber ) return;
		LoadStoryAction();
		LoadRoleAction();
		LoadTrapAction();
		mLoadSucceed = true;
		//std::function<void()> Fun = std::bind(&LoadSpineData::LoadSpineAction,this,m_VecSpine);
		//std::thread LoadThread(Fun);
		//LoadThread.detach();
		//std::function<bool(vector<int>&,bool)> Fun = std::bind(&LoadBattleResource::SpineThread,this,std::placeholders::_1,std::placeholders::_2);
		//std::packaged_task<bool(vector<int>&,bool)>dTask(Fun);		//spine�첽���ش���
		//pFuture = dTask.get_future();
		//std::thread cthread(std::move(dTask),m_WarResouse.find(ResourceType::Load_Spine)->second,true);
		//cthread.detach();
		//m_InitFuture = true;
	}

	void SpineDataManage::LoadRoleAction()
	{
		for (auto tRoleID : mVecRole)
		{
			char tJson[60]   = {0};
			char tAltlas[60] = {0};
			sprintf(tJson,"Spine/%d.json",tRoleID);
			sprintf(tAltlas,"Spine/%d.atlas",tRoleID);
			LoadActionData(tJson,tAltlas,ToString(tRoleID));
			sprintf(tJson,"Spine/%d_texiao.json",tRoleID);
			sprintf(tAltlas,"Spine/%d_texiao.atlas",tRoleID);
			char tEffectKey[60] = {0};
			sprintf(tEffectKey,"%d_texiao",tRoleID);
			LoadActionData(tJson,tAltlas,tEffectKey);
		}
	}

	void SpineDataManage::LoadStoryAction()
	{
		for (auto tStroyRole: mVecStory)
		{
			char tJson[60]   = {0};
			char tAltlas[60] = {0};
			sprintf(tJson,"storyImage/Spine/%d.json",tStroyRole);
			sprintf(tAltlas,"storyImage/Spine/%d.atlas",tStroyRole);
			LoadActionData(tJson,tAltlas,ToString(tStroyRole));
		}
	}

	void SpineDataManage::LoadTrapAction()
	{
		for (auto tTrap: mTrapVec)
		{
			char tJson[60]   = {0};
			char tAltlas[60] = {0};
			sprintf(tJson,"Trap/%d.json",tTrap);
			sprintf(tAltlas,"Trap/%d.atlas",tTrap);
			LoadActionData(tJson,tAltlas,ToString(tTrap));
		}
	}

	void SpineDataManage::LoadActionData( const char* pJson,const char* pAtlas,const char* pKey )
	{
		std::string strFullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(pJson);
		if(!CCFileUtils::sharedFileUtils()->isFileExist(strFullPath))
		{
			CCLOG(" [ *Tips ] LoadSpineData::RealLoadAction Json File No Find %s",pJson);
			return;
		}
		strFullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(pAtlas);
		if(!CCFileUtils::sharedFileUtils()->isFileExist(strFullPath))
		{
			CCLOG(" [ *Tips ] LoadSpineData::RealLoadAction altlas File No Find %s",pAtlas);
			return;
		}
		spAtlas*atlas = spAtlas_createFromFile(pAtlas, 0);
		CCAssert(atlas, "Error reading atlas file.");
		spSkeletonJson* atlas_json = spSkeletonJson_create(atlas);
		atlas_json->scale = 1.0f;//����������С
		spSkeletonData* skeletonData = spSkeletonJson_readSkeletonDataFile(atlas_json, pJson);
		CCAssert(skeletonData, atlas_json->error ? atlas_json->error : "Error reading skeleton data file.");
		spSkeletonJson_dispose(atlas_json);
		if (getSpineData(pKey))
			return;
		mMapData[pKey] = make_pair(skeletonData,atlas);
	}

};
