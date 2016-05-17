#ifndef _STORY_LAYER_H_
#define _STORY_LAYER_H_
/************************************************************* 
 *
 *
 *		Data : 2016.5.17
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 剧情层
 *
 *
 *************************************************************/
#include "AppUI.h"
#include <spine/spine-cocos2dx.h>
#include "scene/layer/LayerManager.h"

class StoryStep;
class StoryData;
using namespace spine;
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
	void mapAddZero();
	void storymusic();
	void storyrole();
	void setAnimationPosition();
	void clearNode(bool initMap = false);
	void clearRole();

	void content();
	void aside();
	void clearContent();
	void updateContent();
	void blackBackImage();
	void displayTackImage();

	void backGround();
	void clearBackGround();
	void updateBackGround();

	void newRole();
	void nameBackground();

	void Skeleton(bool update = false);
	SkeletonAnimation* getSkeleton(bool update);
	void updateSkeleton(SkeletonAnimation*  Animation);
	void setSkeletionAction(SkeletonAnimation*  Animation);
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
#endif // !_STORY_LAYER_H_
