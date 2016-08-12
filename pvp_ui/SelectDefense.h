#pragma once

/******************************************************
*文件名称:	SelectDefense.h
*编写日期:	2016-6-13-17:16
*编写作者:	YPF
*功能描述:	选择防守阵型
*******************************************************/

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "protos/protocol.h"
#include "model/CRoleData.h"
#include "spine/spine-cocos2dx.h"

#define SelectDefenseHeadWaitMoveAction 1

using namespace spine;

class CSelectDefense:public BaseLayer
{
public:

	CSelectDefense();
	~CSelectDefense();
	CREATE_LAYER(CSelectDefense);
	virtual bool init();
	void onEnter();
	void onExit();

protected:
	void onSwitchBtn(CCObject *pSender, bool bChecked);
	void onSwitchStrategy(CCObject *pSender, bool bChecked);
	void updateHeroList();
	void updateCurrHeroList();
	void addGridCell(unsigned int uIdx, CGridViewCell* pCell);

	CCObject* gridviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void processMessage(int type, google::protobuf::Message *msg);
	void addHeroSelectCell(unsigned int uIdx, CGridViewCell* pCell);

	//保存队伍信息
	void saveTeamData();

	//选择英雄的边框效果
	void setSelectHeroRect( int iId );

	//英雄上阵选中状态效果
	void setSelectHeroEffect( CGridViewCell *pCell, bool bSelected);
	//根据id获取scrollview中的英雄cell
	CGridViewCell *getHeroCellFromScrollViewById( int iId );
	//是否在队伍里
	bool isHeroSelected( int iId );
	//更新选中英雄数量
	void updateSelectHeroNum();

	//选择英雄
	void onClickHero( CCObject *pSender );
	//点击普通格子
	void onClickTile( CCObject *pSender );
	//点击队长格子
	void onClickCaptainTile( CCObject *pSender );
	//点击已经生成的头像
	void onClickIcon(CCObject *pSender);

	//检查修改, 无修改，不上传
	bool checkModify( const PvpTeamData&  newData);

	//某个格子上是否有人
	bool checkIsHeroStandOnTile( CCNode *pTile );

	//待移动状态切换
	void setWaitMoveState( CImageView *pIcon, bool bFlag );

	//获取当前待移动状态的icon
	CImageView *getWaitMoveIcon();

	//根据id找heroicon
	CImageView *getHeroIconById( int iId );

private:
	//初始化特效
	void initEffect();

	//初始化格子
	bool initGreen();

	//生成五个英雄icon
	void createHeroIcon();

	void initHeroIconByMsg();
	//更新策略
	void updateStrategy( int iIndex );

	//更新队长技
	void updateCaptainSkill( CHero *pHero );

	//创建拖动英雄
	void createDragHero( );
	//拖动英雄
	void dragHero( CCTouch* pTouch );
	//放置英雄
	void putDownHero( CCTouch* pTouch);

	/*****/
	int getGridByPoint(CCPoint pPoint);

	CCPoint getPointByGrid(int pGrid);

	//队长是否已选
	bool isCaptainSelected( );

	//生成icon
	void initIconByPos( CCPoint pos );
	//替换icon
	void changeIconByPos( CImageView *pIcon,  CCPoint pos );
	//移除icon
	void removeIcon( CImageView *pIcon );

	bool hasInBattle();

	void clearIcon();

	void changeFloor(bool pShow);

	void reset(CCObject *pSender);

	//底板效果
	void setFloorEffect(bool bShow);

	//播放策略特效
	void playStrategyEffect(int iIndex);

private:
	CLayout *m_ui;

	//当前选择的策略
	int m_iStrategyIndex;

	//英雄信息cell
	CLayout *m_cell;

	//英雄列表
	CGridView *m_gridView;

	//英雄列表备份
	vector<CHero> m_heroList;
	//英雄地图备份
	map<int,vector<CHero*>> m_heroMap;
	//当前选中的英雄列表
	vector<CHero*> *m_currHeroList;

	//当前英雄数据
	CHero* m_pDragHeroData;

	//单触摸控制
	CCTouch* m_pTouch;
	bool m_bHold;
	bool m_bBaseTouch;

	//4X4格子
	CLayout* m_pGreenLay;
	//队长格子
	CImageView* m_Captain;
	CLayout* mIconNode;
	CImageView* mMoveIcon;
	vector<CImageView*>mHeroIcons;
	map<int,CCPoint> mIconPoint;
	int mStrategyIndex;
	CCPoint mGridSize;
	bool mgetBattleInfo;
	bool mgetHeroList;
	PvpTeamData mPvPData;

	//人数已满
	bool m_bAllSelected;
	SkeletonAnimation	*m_pStrategyEffect[3];
	MaskLayer *m_pEffectLayer;
};