#ifndef __GUIDE_DATA_
#define __GUIDE_DATA_
#include "protos/protocol.h"
#include "cocos2d.h"
USING_NS_CC;

//引导节点类型
enum GuideNodeClipType
{
	GuideNodeClipWithStencil,		//区域镂空-过滤透明像素
	GuideNodeClipWithZorder,		//复制节点到clipnode
	GuideNodeClipWithStencilNoAlpha,		//区域镂空-不操作透明像素
};

struct GuideTask
{
	int step;
	string className;
	string className1;
	string type;
	string widgetId;
	int tag;
	int tag1;
	string desc;
	string name;
	CCPoint offset;
	int autoJump;
	bool isFinish;
	string text1;
	string text2;
	GuideNodeClipType nodeClickType;
	GuideTask();
};

class CGuideData
{
public:
	CGuideData();
	void parseXml(const char* xml);
	const vector<GuideTask>& getGuideTasks(int task);
	GuideTask* getTaskByStep(int task, int step);
protected:
private:
	typedef map<int,vector<GuideTask>> GuideMap;
	GuideMap m_guideMap;
};



#endif