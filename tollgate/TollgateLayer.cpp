#include "TollgateLayer.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "scene/alive/ActObject.h"
#include "tools/UICloneMgr.h"
#include "netcontrol/CPlayerControl.h"
#include "common/color.h"
#include "model/WarManager.h"
#include "Sweep.h"
#include "TollgatePreview.h"
#include "sign/PopItem.h"
#include "scene/AnimationManager.h"
#include "GMessage.h"
#include "guide/GuideManager.h"
#include "tools/ShowTexttip.h"
#include "common/CommonFunction.h"

#include "common/CGameSound.h"
#include "Resources.h"
#include "common/ShaderDataHelper.h"
#include "common/CSpecialProgress.h"

CTollgateLayer::CTollgateLayer():m_selectChapterIndex(0),m_isStory(true),m_iCurrentTouchDir(TollgateTouchDirNull)
	,m_iLastNormlChapter(-1), m_iLastSpecialChapter(-1),m_bTouchLock(false),m_iPreChapterIndex(1),m_iCurrentPrizeType(1)
	,m_bExitWithNoHardChapter(false)
{
	m_nowChapter[0] = 0;
	m_nowChapter[1] = 0;
	m_iOpenChapterCount[0] = 0;
	m_iOpenChapterCount[1] = 0;
}
using namespace BattleSpace	;

CTollgateLayer::~CTollgateLayer()
{
}


bool CTollgateLayer::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("MaskTollgate");
		lay->setContentSize(CCSizeMake(1138,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("Stage.xaml");  
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		return true;
	}
	return false;

}


void CTollgateLayer::onEnter()
{
	BaseLayer::onEnter();

	//关卡章节滑动列表
	m_pageView = (CPageView*)(m_ui->findWidgetById("scroll"));
	m_pageView->setDirection(eScrollViewDirectionHorizontal);
	m_pageView->setSizeOfCell(m_pageView->getContentSize());
	m_pageView->setCountOfCell(0);
	m_pageView->setAutoRelocate(true);
	m_pageView->setDeaccelerateable(false);
	m_pageView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CTollgateLayer::pageviewDataSource));
	// 	m_pageView->setOnPageChangedListener(this,ccw_pagechanged_selector(CTollgateLayer::onPageChange));
	m_pageView->setDragable(false);
	m_pageView->reloadData();


	//左按钮
	CButton* leftbtn = (CButton*)(m_ui->findWidgetById("left"));
	leftbtn->setOnClickListener(this,ccw_click_selector(CTollgateLayer::onLeftStage));
	/*
	CCFadeTo *fade1 = CCFadeTo::create(0.8f,100);
	CCFadeTo *fade2 = CCFadeTo::create(0.8f,255);
	leftbtn->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(fade1,fade2)));
	*/

	//右按钮
	CButton* rightbtn = (CButton*)(m_ui->findWidgetById("right"));
	rightbtn->setOnClickListener(this,ccw_click_selector(CTollgateLayer::onRightStage));
	/*
	fade1 = CCFadeTo::create(0.8f,100);
	fade2 = CCFadeTo::create(0.8f,255);
	rightbtn->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(fade1,fade2)));
	*/

	CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_ui->getChildByTag(10);
	for (int i = 0; i < 2; i++)
	{
		CRadioButton *radioBtn= (CRadioButton*)(radioGroup->getChildByTag(i+1));
		radioBtn->setOnCheckListener(this,ccw_check_selector(CTollgateLayer::onSwitchBtn));
	}

	//退出 
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setId("close");
	pClose->setPosition(m_ui->convertToNodeSpace(ccp(VLEFT+50, VTOP-50)));
	pClose->setOnClickListener(this,ccw_click_selector(CTollgateLayer::onClose));
	m_ui->addChild(pClose, 999);


	GetTcpNet->registerMsgHandler(ChapterList,this,CMsgHandler_selector(CTollgateLayer::ProcessMsg));
	GetTcpNet->registerMsgHandler(StageList,this,CMsgHandler_selector(CTollgateLayer::ProcessMsg));
	GetTcpNet->registerMsgHandler(GetStagePrize,this,CMsgHandler_selector(CTollgateLayer::ProcessMsg));


	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("warpublic/warning.plist");//将plist文件加载进入缓存
	AnimationManager::sharedAction()->ParseAnimation("warning");
	CCAnimation *culAnim = AnimationManager::sharedAction()->getAnimation("warning");
	culAnim->setDelayPerUnit(0.05f);
	CCAnimate* pAnimate = CCAnimate::create(culAnim);
	m_pWarning  = CCSprite::create();
	m_pWarning->setPosition(ccp(1138/2, 640/2));
	m_ui->addChild(m_pWarning, -1);
	m_pWarning->runAction(CCRepeatForever::create(pAnimate));
	m_pWarning->setVisible(!m_isStory);


	UserData *user = DataCenter::sharedData()->getUser()->getUserData();
	//user->setNewStep(7);
	if (user->getNewStep()>0)
	{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	 this->scheduleOnce(schedule_selector(CTollgateLayer::runGuideStep),0.4f);
#else
	 this->scheduleOnce(schedule_selector(CTollgateLayer::runGuideStep),0.4f);
#endif
	}
}


void CTollgateLayer::onExit()
{
	BaseLayer::onExit();
	GetTcpNet->unRegisterAllMsgHandler(this);
	if (!LayerManager::instance()->getLayer("CTaskLayer"))
	{	
		CSceneManager::sharedSceneManager()->PostMessageA(SHOW_HEAD,0,nullptr,nullptr);
	}
	//NOTIFICATION->postNotification(SHOW_MAIN_SCENE);
}

void CTollgateLayer::onLeftStage(CCObject* pSender)
{
	if (m_selectChapterIndex>1)
	{
		StageMap::iterator iter = m_stageMap.find(m_isStory?m_selectChapterIndex-1:m_selectChapterIndex+200-1);
		if (iter!=m_stageMap.end())
		{
			m_selectChapterIndex--;
			CCPoint pos = m_pageView->getContentOffset();
			m_pageView->setContentOffset(ccp(-(m_selectChapterIndex-1)*m_pageView->getSizeOfCell().width,0));
			m_pageView->reloadData();	
			updateChapter(&m_chapterList[m_isStory].at(m_selectChapterIndex-1));
			checkLeftAndRightButton();
		}
		else
		{
			m_iCurrentTouchDir = TollgateTouchDirLeft;
			GetTcpNet->sendStageList(m_isStory?m_selectChapterIndex-1:m_selectChapterIndex+200-1);
		}

		//showBookAnimate(m_ui, false);
	}
}

void CTollgateLayer::runGuideStep(float dt)
{
	UserData *user = DataCenter::sharedData()->getUser()->getUserData();
	//	user->setNewStep(1);
	int step = user->getNewStep();
	if (step>2&&step<=25)
	{
		CGuideManager::getInstance()->setTaskStep(user->getNewStep(),1);
		CGuideManager::getInstance()->enterGuide();
	}
	this->unschedule(schedule_selector(CTollgateLayer::runGuideStep));
}

void CTollgateLayer::onRightStage(CCObject* pSender)
{
	if (m_selectChapterIndex<m_chapterList[m_isStory].size()&&m_chapterList[m_isStory].at(m_selectChapterIndex).isOpen)
	{
		StageMap::iterator iter = m_stageMap.find(m_isStory?m_selectChapterIndex+1:m_selectChapterIndex+200+1);
		if (iter!=m_stageMap.end())
		{
			m_selectChapterIndex++;
			CCPoint pos = m_pageView->getContentOffset();
			m_pageView->setContentOffset(ccp(-(m_selectChapterIndex-1)*m_pageView->getSizeOfCell().width,0));
			m_pageView->reloadData();	
			updateChapter(&m_chapterList[m_isStory].at(m_selectChapterIndex-1));
			checkLeftAndRightButton();
		}
		else
		{
			m_iCurrentTouchDir = TollgateTouchDirRight;
			GetTcpNet->sendStageList(m_isStory?m_selectChapterIndex+1:m_selectChapterIndex+200+1);
		}

		//showBookAnimate(m_ui, true);
	}
}


void CTollgateLayer::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

CCObject* CTollgateLayer::pageviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
{
	CPageViewCell *pCell = (CPageViewCell*)pConvertCell;
	if (!pCell)
	{
		pCell = new CPageViewCell();
		pCell->autorelease();
		addCell(uIdx, pCell);
	}
	else
	{
		pCell->removeAllChildren();
		addCell(uIdx,pCell);
	}
	return pCell;
}

void CTollgateLayer::addCell(unsigned int uIdx, CPageViewCell * pCell)
{
	// 	if (uIdx==0)
	{
		const vector<StageWidget> *data = DataCenter::sharedData()->getStageData()
			->getStageWidgets(m_chapterList[m_isStory][m_selectChapterIndex-1].id);

		if (!data) return;
		for (int i = 0; i < data->size(); i++)
		{
			CImageView *sprite = nullptr;
			CButton *btn = nullptr;
			CCNode *pNode = nullptr;
			StageWidget widget = data->at(i);
			//CStage &stage = m_stageMap[m_chapterList[m_selectChapterIndex-1].id].at(widget.stageId-1);
			if (widget.type=="image")
			{
				pNode = CImageView::create(widget.normalImage.c_str());
				pNode->setZOrder(-3);
				if(!pNode)
				{
					CCLOG("ERROR  CTollgateLayer::addCell");
					pNode = CImageView::create("tollgate/map001.png");
				}
				sprite = (CImageView*)pNode;
			}
			else if (widget.type=="btn")
			{
				pNode = CButton::create(widget.normalImage.c_str());
				btn = (CButton*)pNode;
			}

			btn = dynamic_cast<CButton*>(pNode);
			if (btn)
			{
				//	btn->setUserData(&m_stageMap[m_chapterList[m_selectChapterIndex-1].id].at(widget.stageId-1));
				btn->setOnClickListener(this,ccw_click_selector(CTollgateLayer::onBattle));
				btn->setZOrder(widget.stageType);
				btn->setEnabled(m_stageMap[m_chapterList[m_isStory][m_selectChapterIndex-1].id].at(widget.stageId-1).isOpen);
			}

			if (pNode)
			{
				pNode->setPosition(ccpAdd(widget.position,ccp(20,68)));

				if (widget.tag>0)
				{
					pNode->setTag(widget.tag);
				}
				pNode->setScaleX(widget.scaleX);
				pNode->setScaleY(widget.scaleY);
				if (widget.widgetId!="")
				{
					// 					((CWidget *)pNode)->setId(widget.widgetId.c_str());
					if (widget.widgetId.find("hero")!=string::npos)
					{
						btn->setId(widget.widgetId.c_str());
						pNode->setAnchorPoint(ccp(0.5,0.0));
						pNode->setPositionY(pNode->getPositionY()-pNode->getContentSize().height/2);

						//人物会动
						CCScaleTo *tobig = CCScaleTo::create(0.55f,1.0*widget.scaleX,1.0*widget.scaleY);
						CCScaleTo *toSmall = CCScaleTo::create(0.55f,0.96*widget.scaleX, 0.96*widget.scaleY);
						pNode->stopAllActions();
						pNode->runAction(CCRepeatForever::create(CCSequence::create(toSmall,tobig,nullptr)));

						//如果是最后一个，变黑
						CStage &stage = m_stageMap[m_chapterList[m_isStory][m_selectChapterIndex-1].id].at(widget.stageId-1);
						if(i==data->size()-1 && stage.star<=0)
						{
							CButton* pHero = (CButton*)pNode;
							((CCNodeRGBA*)pHero->getNormalImage())->setColor(ccc3(0, 0, 0));
							((CCNodeRGBA*)pHero->getNormalImage())->setOpacity(230);
						}
					}
					else if (widget.widgetId.find("prize")!=string::npos)
					{
						if (widget.stageId-1<m_stageMap[m_chapterList[m_isStory][m_selectChapterIndex-1].id].size())
						{
							CStage *stage =&m_stageMap[m_chapterList[m_isStory][m_selectChapterIndex-1].id].at(widget.stageId-1);
							//0 没有道具，1 有道具、可领取，2 有道具、不可领取
							pNode->setTag(100+widget.stageId);
							if (stage->prize==2)
							{
								((CButton*)pNode)->getNormalImage()->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
								CCSequence *seque = CCSequence::createWithTwoActions(CCMoveBy::create(0.44f,ccp(0, -8)),CCMoveBy::create(0.44f,ccp(0, 8)));
								CButton *prizeBtn = (CButton*)pNode;
								prizeBtn->getNormalImage()->runAction(CCRepeatForever::create(seque));
							}
							else if (stage->prize==0)
							{
								pNode->setVisible(false);
							}
							else if (stage->prize==1)
							{
								CButton *prizeBtn = (CButton*)pNode;
								prizeBtn->setUserData(stage);
								prizeBtn->setOnClickListener(this,ccw_click_selector(CTollgateLayer::onGetPrize));
								CImageView* pImage = CImageView::create(widget.normalImage.c_str());
								pImage->setPosition(ccp(prizeBtn->getContentSize().width/2, prizeBtn->getContentSize().height/2));
								prizeBtn->addChild(pImage, 11, 11);
								int begin =widget.normalImage.find_first_of('/');
								int end = widget.normalImage.find_first_of('_');
								string sub_img = widget.normalImage.substr(begin+1,end-begin-1);
								CCAnimation *anim = AnimationManager::sharedAction()->getAnimation(sub_img.c_str());
								anim->setDelayPerUnit(0.2f);
								CCSequence *seque = CCSequence::createWithTwoActions(CCMoveBy::create(0.6f,ccp(0, -10)),CCMoveBy::create(0.6f,ccp(0, 10)));
								pImage->runAction(CCRepeatForever::create(CCAnimate::create(anim)));
								prizeBtn->setUserObject(CCInteger::create(atoi(sub_img.c_str())));
								prizeBtn->runAction(CCRepeatForever::create(seque));
							}
						}
					}
				}

				if (widget.stageId>0)
				{
					if (widget.stageId-1<m_stageMap[m_chapterList[m_isStory][m_selectChapterIndex-1].id].size())
					{
						CStage &stage = m_stageMap[m_chapterList[m_isStory][m_selectChapterIndex-1].id].at(widget.stageId-1);
						pNode->setUserData(&m_stageMap[m_chapterList[m_isStory][m_selectChapterIndex-1].id].at(widget.stageId-1));

						if (!stage.isOpen&&widget.tag>1&&widget.widgetId.find("prize")==string::npos)
						{
							if (widget.type=="btn")
							{
								if(!(i==data->size()-1 && stage.star<=0))
								{
									((CButton*)pNode)->getNormalImage()->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
								}								
							}
							else
							{
								((CButton*)pNode)->getNormalImage()->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
							}
						}
						else if (widget.normalImage=="tollgate/xingxing_1.png")
						{
							CCTexture2D* texture= CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("tollgate/xingxing_%d.png",stage.star+1)->getCString());
							((CCSprite*)btn->getNormalImage())->setTexture(texture);
						}
						else if (widget.normalImage=="tollgate/star_1.png")
						{
							if (stage.star>0)
							{
								CCTexture2D* texture= CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("tollgate/star_%d.png",stage.star)->getCString());
								((CCSprite*)btn->getNormalImage())->setTexture(texture);
							}
							else
							{
								pNode->setVisible(false);
							}
						}
						//隐藏闯关通过关卡
						if (widget.stageType==0&&stage.isOpen&&stage.star>0&&i>0)
						{
							pNode->setVisible(true);
//#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
//							pNode->setVisible(true);
//#else
//							pNode->setVisible(false);
//#endif		
						}
						if (stage.isOpen&&stage.star==0&&i>0&&m_isStory)
						{
							CCSprite *arrow = (CCSprite*)pCell->getChildByTag(100);
							CCSprite *light = (CCSprite*)pCell->getChildByTag(1011);
							if (!light)
							{
								light = CCSprite::create();
								CCAnimation * pLightAnimation = AnimationManager::sharedAction()->getAnimation("9061");
								pLightAnimation->setDelayPerUnit(0.1f);
								CCAnimate* pLightAnimate = CCAnimate::create(pLightAnimation);
								light->runAction(CCRepeatForever::create(pLightAnimate));
								light->setPosition(ccp(pNode->getPositionX(),pNode->getPositionY()));
								light->setTag(1011);
								pCell->addChild(light);
								CCFadeTo *fade = CCFadeTo::create(0.44f,130);
								CCFadeTo *fade1 = CCFadeTo::create(0.44f,255);
								light->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(fade1, fade)));
							}

						}
						if (m_isStory||widget.stageType==1)
						{
							pCell->addChild(pNode);
						}
						if (widget.normalImage=="tollgate/star_1.png"&&stage.star==0)
						{
							pNode->setVisible(false);
						}
					}
				}
			}
			//CCGrayUtil::addGray(pNode);

		}
	}

	

	//标题和背景放在pageView上面
	CCSprite* pName = nullptr;
	if (m_isStory)
	{
		CLayout* lay = (CLayout*)(m_ui->findWidgetById("lay_title"));
		CLayout *Layer = UICloneMgr::cloneLayout(lay);
		CCSprite* pNameBg = (CCSprite*)Layer->findWidgetById("name_bg");
		Layer->removeChild(pNameBg);
		pCell->addChild(pNameBg, -2);
		pNameBg->setPosition(m_pageView->convertToNodeSpace(pNameBg->getPosition()));
		pName = (CCSprite*)Layer->findWidgetById("name");
		Layer->removeChild(pName);
		pCell->addChild(pName, -1);
		pName->setPosition(m_pageView->convertToNodeSpace(pName->getPosition()));
	}
	else
	{
		CLayout* lay = (CLayout*)(m_ui->findWidgetById("lay_title1"));
		CLayout *Layer = UICloneMgr::cloneLayout(lay);
		CCSprite* pNameBg = (CCSprite*)Layer->findWidgetById("name_bg1");
		Layer->removeChild(pNameBg);
		pCell->addChild(pNameBg, -2);
		pNameBg->setPosition(m_pageView->convertToNodeSpace(pNameBg->getPosition()));
		pName = (CCSprite*)Layer->findWidgetById("name1");
		Layer->removeChild(pName);
		pCell->addChild(pName, -1);
		pName->setPosition(m_pageView->convertToNodeSpace(pName->getPosition()));
	}
	CCNode *star1 = m_ui->findWidgetById("star1");
	star1->setVisible(m_isStory);
	CCNode *star2 = m_ui->findWidgetById("star2");
	star2->setVisible(!m_isStory); 
	//章节标题
	CChapter *chapter = &m_chapterList[m_isStory][m_selectChapterIndex-1];
	CCTexture2D *texture = CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("tollgate/chapter_%d.png",chapter->id/*>200?chapter->id-200:chapter->id*/)->getCString());
	if(!texture)
	{
		texture = CCTextureCache::sharedTextureCache()->addImage("tollgate/chapter_1.png");
	}
	pName->setTexture(texture);
	pName->setTextureRect(CCRectMake(0,0,texture->getContentSize().width,texture->getContentSize().height));
}

void CTollgateLayer::updateChapter(CChapter *chapter)
{
	if (chapter)
	{
		//当前关卡获得的星星比例
		CLabel *level = (CLabel *)(m_ui->findWidgetById("level"));
		level->setString(CCString::createWithFormat("%d/%d",chapter->star,chapter->totalStar)->getCString());
		
		//星星比例进度条显示
		CProgressBar *process = (CProgressBar *)(m_ui->findWidgetById("process"));
		process->setMinValue(0);
		process->setMaxValue(chapter->totalStar);
		process->setValue(chapter->star);
		//CSpecialProgress* pCostBar = CSpecialProgress::CreateWithProgressBar(process);
		//pCostBar->addParticleEffect("lz/yellow.plist");
		////m_pCostBar->addHead(pProcessHead);
		//process->getParent()->addChild(pCostBar);
		//process->removeFromParentAndCleanup(true);


		//宝箱
		CImageView *maskBg = (CImageView*)(m_ui->findWidgetById("item1"));
		CImageView *mask = (CImageView*)(m_ui->findWidgetById("mask1"));
		CImageView* prize = (CImageView*)(m_ui->findWidgetById("prize"));
		maskBg->setVisible(true);
		mask->setVisible(true);
		prize->setVisible(true);

		prize->setScale(0.9f);
		if (chapter->prize==1)
		{
			prize->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderDefault));
			prize->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.2f, 1.0f), CCScaleTo::create(0.2f, 0.9f))));
			mask->setUserData(chapter);
			mask->setTouchEnabled(true);
			mask->setOnClickListener(this,ccw_click_selector(CTollgateLayer::onChapterPrize));
		}
		else if(chapter->prize==2)
		{
			maskBg->setVisible(false);
			mask->setVisible(false);
			prize->setVisible(false);
		}
		else
		{
			prize->stopAllActions();
			prize->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
		}
	}
}

void CTollgateLayer::onSwitchBtn( CCObject *pSender, bool bChecked )
{
	if(m_bTouchLock) return;

	CRadioButton *btn = (CRadioButton*)pSender;
	int selIndex = 2*btn->getTag();
	if (bChecked)
	{
		PlayEffectSound(SFX_Button);

		//精英转普通
		if(selIndex==2)
		{
			m_isStory = true;
			m_iLastSpecialChapter = m_selectChapterIndex;

			if(m_iLastNormlChapter == -1)
			{
				//重新请求所有关卡信息
				CPlayerControl::getInstance().sendChapterList(0);
			}
			else
			{
				m_selectChapterIndex = m_iLastNormlChapter;

				StageMap::iterator iter = m_stageMap.find(m_isStory?m_selectChapterIndex:m_selectChapterIndex+200);
				if (iter!=m_stageMap.end())
				{
					CCPoint pos = m_pageView->getContentOffset();
					m_pageView->setContentOffset(ccp(-(m_selectChapterIndex-1)*m_pageView->getSizeOfCell().width,0));
					m_pageView->reloadData();	
					updateChapter(&m_chapterList[m_isStory].at(m_selectChapterIndex-1));
					checkLeftAndRightButton();
					showEffectStoryOrNot();
				}
			}
		}
		//普通转精英
		else
		{
			m_isStory = false;
			m_iLastNormlChapter = m_selectChapterIndex;

			if(m_iLastSpecialChapter == -1)
			{
				//重新请求所有关卡信息
				CPlayerControl::getInstance().sendChapterList(1);
			}
			else
			{
				m_selectChapterIndex = m_iLastSpecialChapter;

				StageMap::iterator iter = m_stageMap.find(m_isStory?m_selectChapterIndex:m_selectChapterIndex+200);
				if (iter!=m_stageMap.end())
				{
					CCPoint pos = m_pageView->getContentOffset();
					m_pageView->setContentOffset(ccp(-(m_selectChapterIndex-1)*m_pageView->getSizeOfCell().width,0));
					m_pageView->reloadData();	
					updateChapter(&m_chapterList[m_isStory].at(m_selectChapterIndex-1));
					checkLeftAndRightButton();
					showEffectStoryOrNot();
				}
			}
		}
	}
}

//接收到服务器数据
void CTollgateLayer::ProcessMsg(int type, google::protobuf::Message *msg)
{
	if(!isVisible())
	{
		this->setVisible(true);
		//NOTIFICATION->postNotification(HIDE_MAIN_SCENE);

		//showBookAnimate(m_ui);
	}

	if(!msg) return;

	if (type==ChapterList)
	{
		ChapterResponse *res= (ChapterResponse*)msg;

		if (res->nowchapter()==0 &&  !m_isStory)
		{
			//设置了没有精英关卡就退出
			if(m_bExitWithNoHardChapter)
			{
				LayerManager::instance()->pop();
				LayerManager::instance()->pop();
				ShowPopTextTip(GETLANGSTR(1000));
				return;
			}
			ShowPopTextTip(GETLANGSTR(1000));
			m_isStory = true;
			showEffectStoryOrNot();
			return;
		}

		showEffectStoryOrNot();

		//清除之前的章节数据
		m_chapterList[m_isStory].clear();

		//当前章节ID
		m_nowChapter[m_isStory] = res->nowchapter();

		//所有章节数据
		m_iOpenChapterCount[m_isStory] = 0;
		for (int i = 0; i < res->chapterlist_size(); i++)
		{
			const Chapter& chap = res->chapterlist(i);
			CChapter chapter;
			chapter.read(chap);
			m_chapterList[m_isStory].push_back(chapter);
			if(chap.isopen())
			{
				//统计开放的章节
				m_iOpenChapterCount[m_isStory]++;
			}
			if (chap.id()==m_nowChapter[m_isStory])
			{
				m_selectChapterIndex = i+1;
			}
		}


		if(m_isStory)
		{
			m_iLastNormlChapter = m_selectChapterIndex;
		}
		else
		{
			m_iLastSpecialChapter = m_selectChapterIndex;
		}

		//没有数据，返回
		if (m_chapterList[m_isStory].size()==0) return;

		//所有关卡数据
		m_currStageList.clear();
		for (int i = 0; i < res->stagelist_size(); i++)
		{
			const StageInfo &info = res->stagelist(i);
			CStage stage;
			stage.read(info);
			m_currStageList.push_back(stage);
		}
		m_stageMap[m_nowChapter[m_isStory]] = m_currStageList;

		//更新章节信息
		m_pageView->setCountOfCell(m_chapterList[m_isStory].size());
		m_pageView->reloadData();

		updateChapter(&m_chapterList[m_isStory].at(m_selectChapterIndex-1));
		
		//设置左右按钮状态
		checkLeftAndRightButton();
	}
	//之请求了单章节的关卡数据
	else if (type==StageList)
	{
		if(m_iCurrentTouchDir==TollgateTouchDirLeft) m_selectChapterIndex--;
		if(m_iCurrentTouchDir==TollgateTouchDirRight) m_selectChapterIndex++;
		if(m_iCurrentTouchDir==TollgateTouchDirNull) m_selectChapterIndex = m_iPreChapterIndex;

		StageListResponse *res = (StageListResponse *)msg;
		m_currStageList.clear();
		for (int i = 0; i < res->stagelist_size(); i++)
		{
			const StageInfo &info = res->stagelist(i);
			CStage stage;
			stage.read(info);
			m_currStageList.push_back(stage);
		}

		m_stageMap[m_chapterList[m_isStory][m_selectChapterIndex-1].id] = m_currStageList;
		m_pageView->setContentOffset(ccp(-(m_selectChapterIndex-1)*m_pageView->getSizeOfCell().width,0));
		m_pageView->reloadData();
		updateChapter(&m_chapterList[m_isStory].at(m_selectChapterIndex-1));

		checkLeftAndRightButton();
	}
	//请求的奖励数据，直接弹窗显示
	else if (type==GetStagePrize)
	{
		//Mr.yan需要数据对接，暂时屏蔽
		GetPrizeRes *res =(GetPrizeRes*)msg;

		if(res->result() == 0)
		{
			return;
		}

		//关卡领奖
		if(m_iCurrentPrizeType == 1)
		{
			//特效
			m_pCurrentStagePrizeBtn->getNormalImage()->stopAllActions();
			int animId = ((CCInteger*)m_pCurrentStagePrizeBtn->getUserObject())->getValue()+1;
			CCAnimation *anim = AnimationManager::sharedAction()->getAnimation(ToString(animId));
			m_pCurrentStagePrizeBtn->setEnabled(false);
			m_pCurrentStagePrizeBtn->removeChildByTag(11);
			m_pCurrentStagePrizeBtn->getNormalImage()->runAction(CCSequence::createWithTwoActions(CCRepeat::create(CCAnimate::create(anim),2),CCRemoveSelf::create()));

			CStage *stage= (CStage*)m_pCurrentStagePrizeBtn->getUserData();
			stage->prize = 0;

			m_pCurrentStagePrizeBtn = nullptr;
		}
		//章节领奖
		else
		{
			//宝箱
			CImageView *maskBg = (CImageView*)(m_ui->findWidgetById("item1"));
			CImageView* prize = (CImageView*)(m_ui->findWidgetById("prize"));
			maskBg->setVisible(false);
			prize->setVisible(false);

			//特效
			CCSprite* pEffect = CCSprite::create("common/cell.png");
			pEffect->setPosition(ccp(maskBg->getPositionX()+maskBg->getContentSize().width/2-5, maskBg->getPositionY()+maskBg->getContentSize().height/2+5));
			prize->getParent()->addChild(pEffect, maskBg->getZOrder());
			CCAnimation *anim = AnimationManager::sharedAction()->getAnimation(ToString(9001));
			pEffect->runAction(CCSequence::createWithTwoActions(CCRepeat::create(CCAnimate::create(anim),2),CCRemoveSelf::create()));

			//更新缓存的当前章节的礼包状态
			m_chapterList[m_isStory].at(m_selectChapterIndex-1).prize = 2;
		}

		CGetPrizeRes prizeRes;
		prizeRes.result = 1;
		for (int i = 0; i < res->prizes_size(); i++)
		{  
			CPrize prize;
			prize.read(res->prizes(i));
			prizeRes.prizeList.push_back(prize);
		}

		//弹框
		CPopItem *popItem = CPopItem::create();
		LayerManager::instance()->push(popItem);
		popItem->popPrizeRes(&prizeRes);
		popItem->setVisible(false);
		popItem->runAction(CCSequence::create(CCDelayTime::create(0.5f), CCShow::create(), nullptr));

		PlayEffectSound(SFX_432);

		//收集货币
		collectMoneyFromPrize(prizeRes);
	}
}

//进入具体关卡
void CTollgateLayer::onBattle(CCObject* pSender)
{
	PlayEffectSound(SFX_Button);

	CButton *btn = (CButton*)pSender;
	CStage *stage = (CStage*)btn->getUserData();
	int zorder = btn->getZOrder();
	if (stage&&stage->isOpen)
	{
		//DataCenter::sharedData()->getWar()->setChapter(m_chapterList[m_isStory][m_selectChapterIndex-1]);
		DataCenter::sharedData()->getWar()->setLastStageId(stage->id); 
		if (stage->star>=1)
			DataCenter::sharedData()->getWar()->setFirstStage(false);
		//DataCenter::sharedData()->getWar()->setTollgate(*stage); 
		DataCenter::sharedData()->getWar()->setStageType(zorder);
		DataCenter::sharedData()->getWar()->setNormal(m_isStory);
		DataCenter::sharedData()->getWar()->setChapterIndex(m_selectChapterIndex);
		DataCenter::sharedData()->getWar()->setChapterCount(m_iOpenChapterCount[m_isStory]);

		CTollgatePreview *preview = CTollgatePreview::create();
		preview->setIsLastStageInChapter(isLasStageInChapter(*stage));
		LayerManager::instance()->push(preview);
		preview->setStage(stage->id, stage->name.c_str());
		CStageInfoRes *ir = DATAPOOL->getStageInfoById(stage->id);		
		preview->setNormal(m_isStory);
		if (ir&&!CGuideManager::getInstance()->getIsRunGuide())
		{
			preview->showStageInfo(*ir);
		}
		else
		{
			CPlayerControl::getInstance().sendStageInfo(stage->id);
		}
	}
}

//请求关卡奖励
void CTollgateLayer::onGetPrize(CCObject* pSender)
{
	CButton *btn =(CButton*)(pSender);
	CStage *stage= (CStage*)btn->getUserData();
	CPlayerControl::getInstance().sendGetStagePrize(1,stage->id);

	m_pCurrentStagePrizeBtn = btn;
	m_iCurrentPrizeType = 1;
}

//请求章节奖励
void CTollgateLayer::onChapterPrize(CCObject* pSender)
{
	CImageView *btn =(CImageView*)(pSender);
	CChapter *chapter= (CChapter*)btn->getUserData();
	CPlayerControl::getInstance().sendGetStagePrize(2,chapter->id);
	//btn->setVisible(false);

	m_iCurrentPrizeType = 2;
}

void CTollgateLayer::checkLeftAndRightButton()
{
	//只有一章节
	CButton* leftbtn = (CButton*)(m_ui->findWidgetById("left"));
	leftbtn->setVisible(m_selectChapterIndex!=1);
	CButton* rightbtn = (CButton*)(m_ui->findWidgetById("right"));
	rightbtn->setVisible(m_selectChapterIndex!=m_iOpenChapterCount[m_isStory]);
}

void CTollgateLayer::showEffectStoryOrNot()
{
	m_pWarning->setVisible(!m_isStory);

	CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_ui->getChildByTag(10);
	CRadioButton *radioBtn1= (CRadioButton*)(radioGroup->getChildByTag(1));
	CRadioButton *radioBtn2= (CRadioButton*)(radioGroup->getChildByTag(2));
	m_bTouchLock = true;
	radioBtn1->setChecked(m_isStory);
	radioBtn2->setChecked(!m_isStory);
	m_bTouchLock = false;
}

void CTollgateLayer::setStory( bool bStory )
{
	m_isStory = bStory;
}

void CTollgateLayer::selectChapter( int iLastChapterCount, int iIndexChapter )
{
	//是否开启了新章节, 默认新章节
	if(m_iOpenChapterCount[m_isStory] > iLastChapterCount)
	{
		return;
	}

	//没有新章节开放,但已经是当前章节
	if(m_selectChapterIndex == iIndexChapter)
	{
		return;
	}

	m_iPreChapterIndex = iIndexChapter;
	GetTcpNet->sendStageList(m_isStory?iIndexChapter:iIndexChapter+200);
}

bool CTollgateLayer::isLasStageInChapter( const CStage& stage )
{
	CStage& lastStage = m_stageMap[m_chapterList[m_isStory][m_selectChapterIndex-1].id].back();
	bool bLast = (lastStage.id == stage.id);
	bool bNoStar = stage.star<=0;
	return bLast && bNoStar;
}
