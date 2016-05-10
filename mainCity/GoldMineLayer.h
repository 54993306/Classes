#ifndef __GOLDMINE_LAYER_
#define __GOLDMINE_LAYER_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
//  金矿类
#include "CityData.h"
class CGlodMineLayer:public BaseLayer
{
public:
	CGlodMineLayer():m_selectHero(0),m_time(0){}
	~CGlodMineLayer(){}
	CREATE_LAYER(CGlodMineLayer);
	virtual bool init();
	void onEnter();
	void onExit();

	void onClickButton(CCObject* pSender);

	void waitTime();

	void timeDelay(float delt);

	void showBuildInfo(const CCity& ct);

	void runFadeAction(const CCity &ct);

	void recBuildInfo(int type, google::protobuf::Message *msg);

	void updateHero(const TMessage& tMsg);


private:
	CLayout *m_ui;
	CCity m_city;
	int  m_selectHero;
	CLabel *m_time;
	int m_second;
};



#endif // !__GOLDMINE_LAYER_
