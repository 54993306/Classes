#ifndef _BATCH_LAYER_
#define _BATCH_LAYER_
#include "AppUI.h"
#include "Global.h"
class BatchLayer : public CCLayer
{
public:
	BatchLayer();
	virtual ~BatchLayer();
	virtual bool init();
	CREATE_FUNC(BatchLayer);
	void setBatch(int totalBatch,int currBatch,bool reset = false);
	int setHpPe();
private:
	CProgressBar* m_BatchPro;
	CProgressBar* m_HpPro;
	CLayout* m_batchLayer;
};
enum BatchLayerTag
{
	hpNum_tag=1,
	batchNum_tag,
	hpBar_tag,
	barchBar_tag,
	sword_tag,
	hpPe_tag,			//血量百分比
	currBatch_tag,
	allBatch_tag,
	ziti_tag,
	currhp_tag,
	xiegan_tag,
};
#endif