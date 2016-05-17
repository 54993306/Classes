#include "StoryLayer.h"
#include "Global.h"
#include "scene/WarScene.h"
#include "tools/ToolDefine.h"
#include "model/DataCenter.h"
#include "model/WarManager.h"
#include "common/CommonFunction.h"
#include "tools/CCShake.h"
#include "common/CGameSound.h"
#include "warscene/EffectData.h"
#include "Battle/BattleMessage.h"

StoryLayer::StoryLayer()
:m_ui(nullptr),m_index(0),m_StoryStep(nullptr),m_LastStep(nullptr)
,m_isStory(0),m_SType(0),m_Bgm(""), m_bOpenTouch(true),m_lastNum(0)
{}

StoryLayer::~StoryLayer()
{
	NOTIFICATION->removeAllObservers(this);
	REMOVE_TARGET(m_ui);
	CC_SAFE_RELEASE(m_ui);
	m_ui = nullptr;
	m_VecStep.clear();
	m_MapPoint.clear();
	m_StoryStep = nullptr;
}

bool StoryLayer::init()
{
	if (!BaseLayer::init())
		return false;
	m_isStory = false;
	m_ui = LoadComponent("story.xaml");
	m_ui->retain();
	m_ui->setPosition(VCENTER);
	this->setContentSize(m_ui->getContentSize());
	this->setTouchPriority(StoryPriority);
	this->setIsShowBlack(false);
	this->addChild(m_ui);
	this->setTouchEnabled(false);
	NOTIFICATION->addObserver(this,callfuncO_selector(StoryLayer::CreateStory),B_LayerMoveEnd,nullptr);

	CImageViewScale9* rightJumpBtn = (CImageViewScale9*)m_ui->getChildByTag(rightJumpBtn_tag);
	rightJumpBtn->setTouchEnabled(true);
	rightJumpBtn->setOnClickListener(this,ccw_click_selector(StoryLayer::PostEnd));
	
	rightJumpBtn->setPosition(m_ui->convertToNodeSpace(ccp(VRIGHT-60, VTOP-40)));						//跳过按钮做相对位置处理(屏幕适配用)
	CCNode* rightJumpBtnText = (CCNode*)m_ui->getChildByTag(rightJumpLabel_tag);
	rightJumpBtnText->setPosition(m_ui->convertToNodeSpace(ccp(VRIGHT-60, VTOP-40)));
	
	return true;
}

void StoryLayer::mapAddZero()
{
	m_StoryStep = nullptr;
	if (m_MapPoint.empty())
	{
		m_MapPoint[storyPanel_tag] = CCPoint(0,0);				//容错处理使用
		clearNode(true);
	}
}

bool StoryLayer::LoadFile(int storytype, StoryData* pStoryData)
{
	mapAddZero();
	if(!pStoryData)
	{
#if CC_TARGET_PLATFORM != CC_PLATFORM_WIN32
		if (DataCenter::sharedData()->getWar()->getTollgete()->star>=1)		//第一次打关卡才出现关卡剧情
			return false;
#endif	
		pStoryData = DataCenter::sharedData()->getWar()->getStoryData();
	}
	vector<StoryStep*>* vec = pStoryData->getStory(StoryType(storytype));
	if (! vec) return false;
	m_VecStep.assign(vec->begin(),vec->end());
	m_StoryStep = m_VecStep.at(m_index);
	step();
	return true;
}

void StoryLayer::clearNode( bool initMap /*= false*/ )
{
	CCArray* childs = m_ui->getChildren();
	vector<int>RemoveChilds;
	CCObject* obj = nullptr;
	CCARRAY_FOREACH(childs,obj)
	{
		CCNode* child = (CCNode*)obj;
		if (initMap)
		{
			CCPoint p = child->getPosition();
			m_MapPoint[child->getTag()] = p;
		}
		if (child->getTag() == rightJumpBtn_tag 
			|| child->getTag() == rightJumpLabel_tag
			|| child->getTag() == talkDown_Bg 
			|| child->getTag() == talkUp_Bg )
			continue;
		RemoveChilds.push_back(child->getTag());
	}
	for (auto i:RemoveChilds)
		m_ui->removeChildByTag(i);							//不能在循环内直接移除子节点
}

CCPoint& StoryLayer::getPointIntag( int tag )
{
	map<int,CCPoint>::iterator iter = m_MapPoint.find(tag);
	if (iter != m_MapPoint.end())
	{
		return iter->second;
	}else{
		CCLOG("[ *ERROR ] StoryLayer::getPointIntag %d",tag);
		return m_MapPoint[storyPanel_tag];
	}
}

void StoryLayer::clearBackGround()
{
	if (!m_LastStep)
		return;
	CCNode* node = m_ui->getChildByTag(talkBg_tag);
	if (node)
	{
		node->runAction(CCSequence::create(CCFadeOut::create(0.25f),CCRemoveSelf::create(),nullptr));
	}else{
		CCLOG("[ *ERROR ]StoryLayer::background 1");
	}
}

void StoryLayer::updateBackGround()
{
	char bg_path[60] = {0};
	sprintf(bg_path,"mainUI/%d.png",m_StoryStep->getTexture());
	CCSprite* bg = CCSprite::create(bg_path);
	if (bg)
	{
		bg->setTag(talkBg_tag);
		bg->setPosition(getPointIntag(talkBg_tag));
		bg->runAction(CCFadeIn::create(0.25f));
		m_ui->addChild(bg);
	}else{
		CCLOG("[ *ERROR ]StoryLayer::background 2");
	}
}
//背景图
void StoryLayer::backGround()
{
	if (!m_StoryStep->getTexture())
		return;
	if (m_LastStep&&m_LastStep->getTexture() == m_StoryStep->getTexture())
		return;
	clearBackGround();
	updateBackGround();
}
//音乐音效
void StoryLayer::storymusic()
{
	PlayEffectSound(SFX_423);	
	char sfx[60] = {0};
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	sprintf(sfx,"SFX/Test/%d.mp3",m_StoryStep->getSFX());
#else
	sprintf(sfx,"SFX/%d.ogg",m_StoryStep->getSFX());
#endif	
	PlayEffectSound(sfx);
	
	if (m_StoryStep->getBGM() < 0)					//双引号是没有、0是不变、非0非双引号则播放
		GameSound->stopMusic();
	if (!m_StoryStep->getBGM())
		return;
	if (m_LastStep && m_StoryStep->getBGM() == m_LastStep->getBGM())
		return;
	char bgm[60] = {0};
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	sprintf(bgm,"BGM/Test/%d.mp3",m_StoryStep->getBGM());
#else
	sprintf(bgm,"BGM/%d.ogg",m_StoryStep->getBGM());
#endif	
	PlayBackgroundMusic(bgm,true);
}

void StoryLayer::clearContent()
{
	if (!m_LastStep)
		return;
	CCNode* talkConten1 = (CCNode*)m_ui->getChildByTag(pangbai_tag);
	if (talkConten1)
		talkConten1->runAction(CCSequence::create(CCFadeOut::create(0.25f),CCRemoveSelf::create(),nullptr));
	CCNode* talkConten2 = (CCNode*)m_ui->getChildByTag(talkConten_tag);
	if (talkConten2)
		talkConten2->runAction(CCSequence::create(CCFadeOut::create(0.25f),CCRemoveSelf::create(),nullptr));
}
//旁白
void StoryLayer::aside()
{
	this->setIsShowBlack(true);
	this->setOpacity(255);
	CLabel* talkConten = CLabel::create("",FONT_NAME,30);
	std::string sStory = m_StoryStep->getConten();
	talkConten->setString(strReplace(sStory,"\\n","\n").c_str());
	talkConten->stopAllActions();
	talkConten->setVisible(true);
	talkConten->runAction(CCFadeIn::create(0.3f));
	talkConten->setTag(pangbai_tag);
	talkConten->setPosition(getPointIntag(pangbai_tag));
	m_ui->addChild(talkConten);
	//旁白自动步骤,
	if(m_StoryStep->gettime()<0.5f)
		m_StoryStep->settime(0.5f);
	talkConten->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(m_StoryStep->gettime()), CCCallFunc::create(this, callfunc_selector(StoryLayer::updateForAutoStep))));
}
//刷新文字内容
void StoryLayer::updateContent()
{
	m_ui->setVisible(true);
	CLabel* talkConten = CLabel::create("",FONT_NAME,30);
	talkConten->setAnchorPoint(ccp(0,0.5f));
	talkConten->setPosition(ccp(/*m_StoryStep->getCx()*/200,m_StoryStep->getCy()));
	std::string sStory = m_StoryStep->getConten();
	talkConten->setString(strReplace(sStory,"\\n","\n").c_str());
	talkConten->runAction(CCFadeIn::create(0.3f));
	talkConten->setTag(talkConten_tag);
	m_ui->addChild(talkConten);
	this->setIsShowBlack(false);
}

void StoryLayer::blackBackImage()
{
	if (!m_StoryStep->getBlack())
		return;
	this->setIsShowBlack(true);
	this->setOpacity(255);
}

void StoryLayer::displayTackImage()
{
	if (m_StoryStep->getShowTalkBg())	//上下黑框
	{
		m_ui->getChildByTag(talkDown_Bg)->setVisible(true);
		m_ui->getChildByTag(talkUp_Bg)->setVisible(true);
	}else{
		m_ui->getChildByTag(talkDown_Bg)->setVisible(false);
		m_ui->getChildByTag(talkUp_Bg)->setVisible(false);
	}
}
//文字内容
void StoryLayer::content()
{
	clearContent();
	displayTackImage();
	if (m_StoryStep->getAside())
	{
		aside();
	}else{
		updateContent();
	}
	blackBackImage();
}
//剧情角色
void StoryLayer::storyrole()
{
	if (m_StoryStep->getAside())
	{
		clearRole();
		return;
	}
	if (m_LastStep&&!m_LastStep->getAside())
	{
		if ((m_LastStep->getright()==m_StoryStep->getright())
		&&  (m_LastStep->getRoleID()==m_StoryStep->getRoleID()))
		{
			Skeleton(true);
			return;      //相同的人在相同的方向,图片和名字都不需更新,处理表情
		}
		clearRole();
		newRole();
	}else{
		newRole();
	}
}

void StoryLayer::nameBackground()
{
	CCSprite* nameBg = CCSprite::create("public/mask2.png");
	if (nameBg)
	{
		if (m_StoryStep->getright())
		{
			nameBg->setTag(rightNameBg_tag);
			nameBg->setPosition(getPointIntag(rightNameBg_tag));
		}else{
			nameBg->setTag(leftNameBg_tag);
			nameBg->setPosition(getPointIntag(leftNameBg_tag));
		}
		nameBg->runAction(CCFadeIn::create(0.25f));
		m_ui->addChild(nameBg);
	}else{
		CCLOG("[ *ERROR ] StoryLayer::nameBackground storyID=%d",m_StoryStep->getstoryId());
	}
}

void StoryLayer::newRole()
{
	Skeleton();
	nameBackground();
	CLabel* name = CLabel::create(m_StoryStep->getRoleName().c_str(),FONT_NAME,25);
	name->runAction(CCFadeIn::create(0.25f));
	if (m_StoryStep->getright())
	{
		name->setTag(rightName_tag);
		name->setPosition(ccpAdd(ccp(60,0),getPointIntag(rightName_tag)));
	}else{
		name->setTag(leftName_tag);
		name->setPosition(ccpAdd(ccp(60,0),getPointIntag(leftName_tag)));
	}
	m_ui->addChild(name);
}

SkeletonAnimation* StoryLayer::getSkeleton(bool update)
{
	if (update)
	{
		if (m_StoryStep->getright())
		{
			return (SkeletonAnimation*)m_ui->getChildByTag(rightImg_tag);
		}else{
			return (SkeletonAnimation*)m_ui->getChildByTag(leftImg_tag);
		}
	}else{
		SpData* data = DataCenter::sharedData()->getWar()->getSpineData(ToString(m_StoryStep->getRoleID()));
		if (!data)
		{
			CCLOG("[ *ERROR ] StoryLayer::storyrole roleId=%d",m_StoryStep->getRoleID());
			return nullptr;
		}
		return SkeletonAnimation::createWithData(data->first);
	}
}
//下一帧才能获取得到spine的大小
void StoryLayer::setAnimationPosition()
{
	CCSize size = CCDirector::sharedDirector()->getWinSize();
	SkeletonAnimation* Animation = nullptr;
	if (m_StoryStep->getright())
	{
		Animation = (SkeletonAnimation*)m_ui->getChildByTag(rightImg_tag);
		Animation->setPosition(ccp(size.width-Animation->boundingBox().getMaxX()+m_StoryStep->getRolex(),65+m_StoryStep->getRoley()));
	}else{
		Animation = (SkeletonAnimation*)m_ui->getChildByTag(leftImg_tag);
		Animation->setPosition(ccp(Animation->boundingBox().getMaxX()+m_StoryStep->getRolex(),65+m_StoryStep->getRoley()));
	}
	if (Animation)
	{
		if (m_StoryStep->getTurn())
			Animation->setScaleX(-1);
		Animation->setVisible(true);
	}else{
		CCLOG("[ *ERROR ] StoryLayer::setAnimationPosition");
	}
}

void StoryLayer::updateSkeleton( SkeletonAnimation* Animation )
{
	Animation->setVisible(false);
	if (m_StoryStep->getright())
	{
		Animation->setTag(rightImg_tag);
	}else{
		Animation->setTag(leftImg_tag);
	}
	CCCallFunc* caf = CCCallFunc::create(this,callfunc_selector(StoryLayer::setAnimationPosition));
	if (m_StoryStep->getRoleShark())
	{
		Animation->runAction(CCSequence::create(CCDelayTime::create(0.02f),caf,CCFadeIn::create(0.25f),CCShake::create(0.4f,13.0f),nullptr));
	}else{
		Animation->runAction(CCSequence::create(CCDelayTime::create(0.02f),caf,CCFadeIn::create(0.25f),nullptr));
	}
	Animation->setZOrder(-1);
	m_ui->addChild(Animation);
}

void StoryLayer::setSkeletionAction( SkeletonAnimation* Animation )
{
	bool fintAction = false;
	for (int i = 0;i<Animation->state->data->skeletonData->animationsCount;i++)
	{
		if (strcmp(Animation->state->data->skeletonData->animations[i]->name,m_StoryStep->getAction().c_str())==0)
		{
			fintAction = true;
			Animation->setAnimation(0,m_StoryStep->getAction().c_str(),true);
			break;
		}
	}
	if (!fintAction)
	{
		string actionName = Animation->state->data->skeletonData->animations[0]->name;
		Animation->setAnimation(0,actionName.c_str(),true);
		CCLOG("[ *ERROR ] StoryLayer::storyrole Lost ActionName = %s",m_StoryStep->getAction().c_str());
	}
}

void StoryLayer::Skeleton(bool update/*=false*/)
{
	SkeletonAnimation*  Animation = getSkeleton(update);
	if (!Animation)
	{
		CCLOG("[ *ERROR ] StoryLayer::Skeleton");
		return;
	}
	if (!update)
		updateSkeleton(Animation);
	setSkeletionAction(Animation);
}

void StoryLayer::clearRole()
{
	for (int i=leftImg_tag;i<=rightName_tag;i++)
	{
		CCNode* node = (CCNode*)m_ui->getChildByTag(i);
		if (node)
		{
			node->setTag(0);								//设置为0防止被二次根据tag获取目标操作
			node->runAction(CCSequence::create(CCFadeOut::create(0.25f),CCRemoveSelf::create(),nullptr));
		}
	}
}
//不断创建新的对象,而不是使用同一个对象执行淡入淡出操作,就不会出现冲突的情况了
void StoryLayer::step()
{
	storymusic();						//音乐音效
	backGround();						//背景图片
	content();							//文字内容
	storyrole();
	if (m_StoryStep->getShark())
		this->runAction(CCShake::create(0.4f,13.0f));
}
//重写BaseLayer的TouchEnd方法要调用一次父类的方法，将触摸消息下发下去再处理(按钮类的消息)
void StoryLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent) 
{ 
	CWidgetWindow::ccTouchEnded(pTouch,pEvent);
	if (m_isStory)NextStep(); 
}

void StoryLayer::PostEnd(CCObject* ob)
{ 
	clearNode();
	this->setVisible(false);
	m_isStory = false;
	m_index = 0;
	m_lastNum = 0;
	m_VecStep.clear();
	m_StoryStep = nullptr;
	this->setTouchEnabled(false);
	NOTIFICATION->postNotification(B_StoryOver,nullptr);
}

void StoryLayer::updateForTouch( float dt ){	m_bOpenTouch = true; }

void StoryLayer::NextStep() 
{ 
	//触摸加锁，0.35s一次
	if(!m_bOpenTouch)
		return;
	m_bOpenTouch = false;
	scheduleOnce(schedule_selector(StoryLayer::updateForTouch), 0.35f);

	m_index += 1;
	if(m_index >= m_VecStep.size())
	{
		PostEnd(nullptr);
		return;
	}
	m_LastStep = m_StoryStep;
	m_StoryStep = m_VecStep.at(m_index);
	step();
}
//剧情开始和剧情结束都会对UI层产生一些影响，比如影响了ui层的bar的量
void StoryLayer::CreateStory(CCObject* ob)
{
	CreateStoryWithStoryData(ob, nullptr);
}

void StoryLayer::CreateStoryWithStoryData( CCObject* ob, StoryData* pStoryData )
{
	m_SType = ((CCInteger*)ob)->getValue();
	if (!LoadFile(m_SType, pStoryData))
	{
		PostEnd(nullptr);
		return;
	}
	m_isStory = true;
	this->setTouchEnabled(true);
	this->setVisible(true);
}

void StoryLayer::updateForAutoStep()
{
	if (m_isStory)
		NextStep(); 
}

void StoryLayer::hideJumpBtn()
{
	CImageViewScale9* rightJumpBtn = (CImageViewScale9*)m_ui->getChildByTag(rightJumpBtn_tag);
	CCNode* rightJumpBtnText = (CCNode*)m_ui->getChildByTag(rightJumpLabel_tag);
	rightJumpBtn->setVisible(false);
	rightJumpBtnText->setVisible(false);
}