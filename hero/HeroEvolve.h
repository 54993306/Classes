#pragma once

/******************************************************
*文件名称:	HeroEvolve.h
*编写日期:	2016-6-20-12:05
*编写作者:	YPF
*功能描述:	英雄进阶面板
*******************************************************/

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "mainCity/CityData.h"
#include "hero/HeroData.h"

#define HERO_EVOLVE_RANK_MAX 5
#define HERO_EVOLVE_ANGLE_GAP 72
#define HERO_EVOLVE_BALL_SCALE 1.0f

class CNewHero;
class CHeroEvolveDataPanel;
class CHeroEvolveEffectLayer;

class CHeroEvolve: public BaseLayer
{
public:
	CHeroEvolve( );
	CREATE_LAYER( CHeroEvolve );
    virtual bool init( );
    void onEnter( );
    void onExit( );

public:
	CC_SYNTHESIZE(bool, m_bAskNextQualityHero, AskNextQualityHero);	//是否仅仅请求英雄数据

	void updateEvolve( CHero *pHero );			//整体更新进阶界面
	void evolveSuccess( CHero* pHero );			//进阶成功，特效执行

	void askForRankData( int iRank );										//请求阶段数据
	void askForRankDataCallBack( CHero*pHero );					//请求到了数据回调

private:
	void updateHead( CHero *pHero );			//更新头像信息
	void updateStoneNum( CHero *pHero );	//更新品质和魂石数量

	
private:
	void onEvolveBtn( CCObject* pSender );		//进阶按钮
	void heroCall( CCObject* pSender );	//获取魂石
	void onPressBallData( CCObject* pSender, CTouchPressState iState );	//触摸显示进阶数据

	void updateHeroInfo( const TMessage& tMsg ); //英雄数据请求回调
	void compaseSuccess( CCObject* pObj );		//魂石合成后，数量更新回调-之前有调和塔，现在没有了
	bool checkHeroEvolveOpen( CHero *pHero );	//是否到达进阶条件

private:
	void initProgressAndBall( );	//初始化进度条和球
	void updateProgressAndBalll( ); //更新进度条和球

	void showEvoleEffect( CHero *pOldHero, CHero *pNewHero );		//进阶特效
	void showEvoleEffectCallBack( );		//进阶特效回调

	void saveHeroData( CHero *pHero, bool bResetCurrentHero=false );		//保存英雄数据
	void setCurrentHeroData( int iRank, bool bForce=false );						//设置指向当前英雄数据

	void uiChangeWhenHeroRankChange( );			//英雄阶级变化时的一些UI变化

	void resetHeroData( );			//重置英雄数据

	void updateEvolveButtonStatus();		//更新进阶按钮状态

	void ballChangeBegin();				//特效执行时，球开始变化

private:
	CLayout							*m_ui;
	CHero								m_pHero[HERO_EVOLVE_RANK_MAX];		//英雄各阶段数据
	CHero								*m_pCurrentRankHeroData;						//英雄当前阶段数据
	CNewHero						*m_pNewHero;
	CCArmature						*m_pHeroArmature;

	CCProgressTimer				*m_pCircleBar;											//圆形进度条
	CImageView						*m_pBall[HERO_EVOLVE_RANK_MAX];		//5个球
	float									m_fCircleBarRadius;									//圆环半径
	float									m_fBallRadius;											//球半径
	float									m_fAngleGap;											//角度忽略值

	CHeroEvolveDataPanel		*m_pDataPanel;										//触摸弹出的数据面板
	int									m_iCurrentTouchBallIndex;						//当前摸着的球
	CHeroEvolveEffectLayer	*m_pEffectLayer;										//进阶效果层
};