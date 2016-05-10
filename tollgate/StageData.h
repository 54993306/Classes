#ifndef __STAGE_DATA_
#define __STAGE_DATA_
#include "protos/protocol.h"
#include "cocos2d.h"
USING_NS_CC;

struct StageWidget
{
	string type;
	string widgetId;
	string normalImage;
	string selectImage;
	CCPoint position;
	float scaleX;
	float scaleY;
	int tag;
	int stageId;
	int stageType;
	StageWidget();
};

class CStageData
{
public:
	CStageData();
	void parseXml(const char* xml);
	const vector<StageWidget>* getStageWidgets(int chapter);
	const StageWidget* getWidgetByStage(int chapter, int index);
protected:
private:
	typedef map<int,vector<StageWidget>> StageMap;
	StageMap m_stageMap;
};



#endif