
#ifndef __BAG_INFOTIP__
#define __BAG_INFOTIP__

#include "scene/layer/LayerManager.h"
#include "protos/protocol.h"
#include "bagData.h"
class CBagInfoTip: public BaseLayer
{
public:
	CBagInfoTip(){};
	virtual bool init();
	CREATE_LAYER(CBagInfoTip);

	void setItemData(int index, const CItem* item);

// 	void onEnter();
// 	void onExit();
	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	// 	void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	void removeAllItem(int type);
	void updateTip(int index);
protected: 
	void addBasicInfo(const CItem *item); //基本信息
	void addEquipmentTip(const CItem *item);
	void addAdditionalTip(int index,CExtAttr& att); //附加能力
	void addOnlyAbilityTip(); //唯一能力
	void addPriceTip(int money);   //售价
private:
	CLayout *m_ui;
	int m_offsetHeight;
	int m_labelInter;
	int m_lineInter;
	int m_removeHeight;
	CCPoint m_basePoint;
};

#endif