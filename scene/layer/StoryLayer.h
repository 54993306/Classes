#ifndef _STORY_LAYER_H_
#define _STORY_LAYER_H_
#include "AppUI.h"
#include "scene/layer/LayerManager.h"
enum Story_Tag
{
	talkBg_tag				=1,
	rightJumpBtn_tag,
	rightJumpLabel_tag,
	pangbai_tag,
	talkConten_tag,
	talkUp_Bg,
	talkDown_Bg,
	leftImg_tag,
	rightImg_tag,		
	leftNameBg_tag,
	rightNameBg_tag,
	leftName_tag,
	rightName_tag,
	storyPanel_tag			= 100,
};
class StoryStep;
class StoryData;
class StoryLayer : public BaseLayer
{
public:
	StoryLayer();
	virtual ~StoryLayer();
	CREATE_FUNC(StoryLayer);
	virtual bool init();
	void hideJumpBtn();
	bool LoadFile(int storytype, StoryData* pStoryData=nullptr);
	void CreateStory(CCObject* ob);
	void CreateStoryWithStoryData(CCObject* ob, StoryData* pStoryData=nullptr);
	CC_SYNTHESIZE(bool,m_isStory,isStory);						//当前是否为剧情状态
	CC_SYNTHESIZE(int,m_SType,SType);							//当前是否为剧情状态
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
protected:	
	void NextStep();
	void updateForTouch(float dt);								//触摸时间锁
	void updateForAutoStep();									//自动跳步骤
	void step();
	void PostEnd(CCObject* ob);	
protected:
	CCPoint& getPointIntag(int tag);
	void background();
	void storymusic();
	void content();
	void storyrole();
	void newRole();
	void Skeleton(bool update = false);
	void setAnimationPosition();
	void clearNode(bool initMap = false);
	void clearRole();
protected:	
	string m_Bgm;
	CLayout* m_ui;
	unsigned int m_index;
	StoryStep* m_LastStep;
	StoryStep* m_StoryStep;
	bool m_bOpenTouch;											//触摸时间锁
	int m_lastNum;												//上一个对话人物名字
	vector<StoryStep*> m_VecStep;
	map<int,CCPoint> m_MapPoint;
};
#endif // !_STORY_LAYER_H_
