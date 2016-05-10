#ifndef __GUIDE_MANAGER_
#define __GUIDE_MANAGER_
#include "protos/protocol.h"
#include "cocos2d.h"
#include "GuideData.h"
#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "GuideLayer.h"
USING_NS_CC;

class CGuideManager
{
public:
	static CGuideManager* getInstance();
	void setTaskStep(int task, int step);
	const CCPoint& geStepWidgetPosition();
	void enterGuide();
	void nextStep();
	CCNode* getStepNode(const GuideTask &task);
	CCNode* findStepWidget(BaseLayer * layer, const GuideTask &task);
	void exitGuide();

	void setJumpStep(bool isJump);

	bool getIsRunGuide();
    bool isAutoNextStep();
	int  getCurrTask();
	int  getCurrStep();
	GuideTask* getGuideTaskData();
	int  getOpenFunction();
protected:
private:
	CGuideManager();
	int m_guideTask;
	int m_guideStep;
	CGuideData m_guideData;
	CCPoint m_stepPos;
	static CGuideManager *m_instance;
	bool m_isRunGuiding;
	CGuideLayer *m_guideLayer;
	map<int,int> m_openFuncMap;
	bool m_isBeginGuide;
};

#endif