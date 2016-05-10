#ifndef __REWARDHARD_LAYER_
#define __REWARDHARD_LAYER_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"

struct CBountyStage;

class CRewardHardLayer: public BaseLayer
{
public:
	CREATE_LAYER(CRewardHardLayer);
	CRewardHardLayer();
	virtual bool init();
	void onEnter();
	void onExit();
	void onStage(CCObject* pSender);
	void setStageList(vector<CBountyStage> pList);
	void onClose(CCObject* pSender);

	CC_SYNTHESIZE(int, m_iChapterId, ChapterId);

	void roleUpdate(const TMessage& tMsg);
private:
	CLayout *m_ui;
	vector<CBountyStage> m_stageList;
};


#endif