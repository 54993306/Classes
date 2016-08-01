
#ifndef __TOLLGATE_LAYER_
#define __TOLLGATE_LAYER_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "Chapter.h"
#include "StageData.h"
#include "activity/HttpLoadImage.h"
#include "FaceBookImg.h"

//章节切换方向
enum TollgateTouchDir
{
	TollgateTouchDirNull,
	TollgateTouchDirLeft,
	TollgateTouchDirRight
};

class CTollgateLayer: public BaseLayer, public HttpLoadImageDelegate
{
public:
	CREATE_LAYER(CTollgateLayer);
	CTollgateLayer();
	~CTollgateLayer();
    virtual bool init();
    void onEnter();
    void onExit();
	
	void ProcessMsg(int type, google::protobuf::Message *msg);
	

	//设置精英还是普通
	void setStory(bool bStory);

	//选择章节 新开放
	void selectChapter(int iLastChapterCount, int iIndexChapter);

	CC_SYNTHESIZE(bool, m_bExitWithNoHardChapter, ExitWithNoHardChapter);
	void setLastChapter(int chapter);
	void imageLoadSuccessCallBack(string sTag, vector<char>* pBuffer);//好友头像下载回调

private:
	//关闭
	void onClose(CCObject* pSender);

	//选择关卡
	void onBattle(CCObject* pSender);

	//tableview相关
	CCObject* pageviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void addCell(unsigned int uIdx, CPageViewCell * pCell);

	void addFaceBookImg(CStage &stage, CImageView * fbBg);

	//左/右按钮切换到上一章/下一章
	void onLeftStage(CCObject* pSender);
	void onRightStage(CCObject* pSender);

	//更新章节信息-星星-宝箱
	void updateChapter(CChapter *chapter);

	//获取关卡奖励
	void onGetPrize(CCObject* pSender);
	//获取章节奖励
	void onChapterPrize(CCObject* pSender);

	//执行引导
	void runGuideStep(float dt);

	//检查并设置左右按钮状态
	void checkLeftAndRightButton();

	//切换普通和精英效果
	void showEffectStoryOrNot();

	//选项卡事件回调
	void onSwitchBtn(CCObject *pSender, bool bChecked);

	//是否是章节最后一个关卡
	bool isLasStageInChapter(const CStage& stage);
	void onFaceImg(CCObject* pSender);
	
	//弹提示框
	void showTips(CCNode *pNode, const std::string sInfo);

private:
	CLayout *m_ui;
	CTableView *m_pageView;
	CCSprite* m_pWarning;
	//章节列表
	vector<CChapter> m_chapterList[2];
	//关卡列表
	vector<CStage>  m_currStageList;
	//返回的当前章节
	int m_nowChapter[2];
	typedef map<int,vector<CStage>>  StageMap;
	StageMap m_stageMap;
	//选择的章节下标
	int m_selectChapterIndex;
	//是否普通关卡
	bool m_isStory;
	TollgateTouchDir m_iCurrentTouchDir;
	//切换精英时记录普通关卡下标
	int m_iLastNormlChapter;
	//切换普通时记录精英关卡下标
	int m_iLastSpecialChapter;
	bool m_bTouchLock;
	int m_iPreChapterIndex;
	//当前奖励类型（关卡还是章节）
	int m_iCurrentPrizeType;
	//关卡奖励按钮
	CButton* m_pCurrentStagePrizeBtn;
	//精英关卡特殊奖励按钮
	CImageView *m_pSpeciallPrizeBtn;

	int m_currChapterIndex;
	//当前开放的章节数量
	int m_iOpenChapterCount[2];
	int m_currChapter;
	CFacebookImg *m_fbImgLayer;
	int		m_iFirstLockedStage;			//即将要打但是没有开放的关卡
	CLayout *m_pTips;
};


#endif