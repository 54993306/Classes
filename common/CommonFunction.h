#pragma once
#include "cocos2d.h"
#include "CCTK/GridView.h"
#include "CCTK/TableView.h"
#include "protos/protocol.h"
#include "tollgate/Chapter.h"
#include "model/CRoleData.h"
#include "model/HeroInfoGM.h"

USING_NS_CC_WIDGET;
using namespace cocos2d;
/************************************************************************
				公用方法				//  [9/9/2014 xc_lin]
************************************************************************/

//震屏
//震动对象、震动幅度、震动次数、总震动时间、回调函数
void runEffect_ShakeX(CCNode *node, float range, float runTime, int times,CCCallFuncN* pOverCallBack=NULL);

void runEffect_ShakeY(CCNode *node, float range, float runTime, int times,CCCallFuncN* pOverCallBack=NULL);

//设置随机种子
void srandNum();

//滤镜功能
static void setEnableRecursiveCascading(CCNode* node, bool enable,ccColor3B color,GLubyte alpha)
{
	if (!node)
		return;
	CCRGBAProtocol* rgba = dynamic_cast<CCRGBAProtocol*>(node);
	if (rgba)
	{
		rgba->setCascadeColorEnabled(enable);
		rgba->setCascadeOpacityEnabled(enable);
		rgba->setColor(color);
		rgba->setOpacity(alpha);
	}

	CCObject* obj;
	CCArray* children = node->getChildren();
	CCARRAY_FOREACH(children, obj)
	{
		CCNode* child = (CCNode*)obj;
		setEnableRecursiveCascading(child, enable,color,alpha);
	}
}
//判断对象是否为空,空则输出错误信息
bool outPutERRORMsg(const char* address,void* ob);
//用于遍历节点执行暂停的方法
void LGPause( CCNode* pNode , bool AllPause = false);

void LGResume( CCNode* pNode );

//快速降序排序
void QuickSort(std::vector<int>&Vec,int left,int right);

//填充父节点
void NodeFillParent(CCNode* pNode, CCNode* pParent=nullptr);

//填充某个节点
void NodeFillNode(CCNode* pNode, CCNode* pAimNode);

//gridview 保持位置刷新
void ReloadGridViewWithSameOffset(CGridView* pGridView);

//tableview 保持位置刷新
void ReloadTableViewWithSameOffset(CTableView* pTableView);

//检查是否出屏幕
bool checkRectInScreen(CCRect rect, float fBorder=0.0f);

//根据类型和颜色获取图片全路径
std::string GetImageName(int iType, int iColor, int iThumb);

//获取quality
int GetQualityByType(int iType, int iQuality, int iColor=0);

//根据armor获取人物图片
std::string getImageName(CArmor* armor);

//根据chero获取人物图片
std::string getImageName(CHero* hero);

//根据cmonster获取人物图片
std::string getImageName(CMonster* monster);

//根据prize获取货币
void collectMoneyFromPrize(CGetPrizeRes& prizeRes);

//翻书效果
void showBookAnimate(CLayout* pNode, bool bToLeft=true);

//获得武将战力
int getCombatLevel(int combat);

//添加90X90星星层
CLayout *getStarLayout(int iStarCount, float fScale=0.4f);

//添加90X90星星层，五星，不够级有黑底
CLayout *getStarLayoutWithBlackBase(int iStarCount, float fScale=0.4f);

//根据奖励是英雄还是物品来使用iColor还是iStar/iQuality
CLayout *SmartGetStarLayout(const CPrize *pPrize, float fScale=0.4f);


void PlaySoundEvent(int sData);

//获取Armor类型
std::string getArmorTypeStr(int iType);


//设置属性框（新版；火水木）
CCTexture2D* SetRectColor(int iType);

//为什么要写这个函数，英雄的颜色改为采用水火木，之前英雄的品质改为星级，之前的星级改为进阶球
//设置颜色的时候，prize相关的东西要特殊处理（有英雄和物品一起的情况），item相关的东西要特殊处理（有魂石和物品一起的情况）
//只能统一做在这里
//void SmartSetRectItemColor(CCSprite* pSprite, int iColor, HeroInfoData* pHeroData=nullptr);  ///-魂石秘籍都是单色，不做特殊处理，

//只处理英雄跟物品一起的情况
void SmartSetRectPrizeColor(CCSprite* pSprite, const CPrize* pPrize, const Prize* pProPrize=nullptr, HeroInfoData* pHeroData=nullptr);

//核对日期是否到来
bool CheckDay( const char *str );

//裁切facebook头像为圆形
CImageView *MakeFaceBookHeadToCircle(CCSprite *pSprite);


//重置锚点并且保持位置
void ResetAnchorPointAndKeepSamePos( CCNode *pNode, CCPoint anchorpoint );