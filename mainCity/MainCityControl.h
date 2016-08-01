#ifndef __MAIN_CITYCONTROL__
#define __MAIN_CITYCONTROL__
#include "net/MessageHandler.h"
#include "net/CNetClient.h"
#include "protos/protocol.h"
#include "CityData.h"
/*#include "mainScene.h"*/
class CMainScene;

using namespace protos;
using namespace protos::common;



class CMainCityControl:public CCNode
{
public:
	static CMainCityControl* getInstance();
	~CMainCityControl();

	void sendEnterCity();
	void sendCityInfo(int cityId);
	void sendGarrisonHero(int cityId, int heroId, int replaceId=0);  //replaceId 被替换的英雄id
	void sendGetCityResource(int cityId); // 请求收获城市资源 (26)   

	void sendShopRequest(int shopType);
    void sendShopBuyItem(int itemId, int buyNum, int type=1);

	void sendBuildLevelUp(int cityId);

	//调合道具35
	void sendComposeItem(int itemId, int armorId);

	void recMainCityInfo(int type, google::protobuf::Message *msg);

	bool isJustOpen(CCity * cb);

	void recBuildInfo(int type, google::protobuf::Message *msg);
	void resCityInfo();
	void recCityResource(int type, google::protobuf::Message *msg);
	void sendShopUpdate(int shopType);

	void updateCityHero(int cityId, CCity& city);
	bool isCityOpen(int cityId);//调和塔 12  铁匠捕 4  培植区6  酒馆10   悬赏府15   竞技场3  排行榜13  千层塔17 历练谷16

	vector<int> getGarrsionHeroId();

	CHero* getGarrsionHeroIdByCityId(int cityId);

	const CCity& getCityDataById( int iId );

protected:
	void removeTip(CCNode * node);
private:
	CMainCityControl();


	static CMainCityControl* instance;
	CMainScene *m_scene;
	CityData m_cityData;
	map<int,CCity> m_cityMap;
};

#endif