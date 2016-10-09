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
				���÷���				//  [9/9/2014 xc_lin]
************************************************************************/

//����
//�𶯶����𶯷��ȡ��𶯴���������ʱ�䡢�ص�����
void runEffect_ShakeX(CCNode *node, float range, float runTime, int times,CCCallFuncN* pOverCallBack=NULL);

void runEffect_ShakeY(CCNode *node, float range, float runTime, int times,CCCallFuncN* pOverCallBack=NULL);

//�����������
void srandNum();

//�˾�����
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
//�ж϶����Ƿ�Ϊ��,�������������Ϣ
bool outPutERRORMsg(const char* address,void* ob);
//���ڱ����ڵ�ִ����ͣ�ķ���
void LGPause( CCNode* pNode , bool AllPause = false);

void LGResume( CCNode* pNode );

//���ٽ�������
void QuickSort(std::vector<int>&Vec,int left,int right);

//��丸�ڵ�
void NodeFillParent(CCNode* pNode, CCNode* pParent=nullptr);

//���ĳ���ڵ�
void NodeFillNode(CCNode* pNode, CCNode* pAimNode);

//gridview ����λ��ˢ��
void ReloadGridViewWithSameOffset(CGridView* pGridView);

//tableview ����λ��ˢ��
void ReloadTableViewWithSameOffset(CTableView* pTableView);

//����Ƿ����Ļ
bool checkRectInScreen(CCRect rect, float fBorder=0.0f);

//�������ͺ���ɫ��ȡͼƬȫ·��
std::string GetImageName(int iType, int iColor, int iThumb);

//��ȡquality
int GetQualityByType(int iType, int iQuality, int iColor=0);

//����armor��ȡ����ͼƬ
std::string getImageName(CArmor* armor);

//����chero��ȡ����ͼƬ
std::string getImageName(CHero* hero);

//����cmonster��ȡ����ͼƬ
std::string getImageName(CMonster* monster);

//����prize��ȡ����
void collectMoneyFromPrize(CGetPrizeRes& prizeRes);

//����Ч��
void showBookAnimate(CLayout* pNode, bool bToLeft=true);

//����佫ս��
int getCombatLevel(int combat);

//���90X90���ǲ�
CLayout *getStarLayout(int iStarCount, float fScale=0.4f);

//���90X90���ǲ㣬���ǣ��������кڵ�
CLayout *getStarLayoutWithBlackBase(int iStarCount, float fScale=0.4f);

//���ݽ�����Ӣ�ۻ�����Ʒ��ʹ��iColor����iStar/iQuality
CLayout *SmartGetStarLayout(const CPrize *pPrize, float fScale=0.4f);


void PlaySoundEvent(int sData);

//��ȡArmor����
std::string getArmorTypeStr(int iType);


//�������Կ��°棻��ˮľ��
CCTexture2D* SetRectColor(int iType);

//ΪʲôҪд���������Ӣ�۵���ɫ��Ϊ����ˮ��ľ��֮ǰӢ�۵�Ʒ�ʸ�Ϊ�Ǽ���֮ǰ���Ǽ���Ϊ������
//������ɫ��ʱ��prize��صĶ���Ҫ���⴦����Ӣ�ۺ���Ʒһ����������item��صĶ���Ҫ���⴦���л�ʯ����Ʒһ��������
//ֻ��ͳһ��������
//void SmartSetRectItemColor(CCSprite* pSprite, int iColor, HeroInfoData* pHeroData=nullptr);  ///-��ʯ�ؼ����ǵ�ɫ���������⴦��

//ֻ����Ӣ�۸���Ʒһ������
void SmartSetRectPrizeColor(CCSprite* pSprite, const CPrize* pPrize, const Prize* pProPrize=nullptr, HeroInfoData* pHeroData=nullptr);

//�˶������Ƿ���
bool CheckDay( const char *str );

//����facebookͷ��ΪԲ��
CImageView *MakeFaceBookHeadToCircle(CCSprite *pSprite);


//����ê�㲢�ұ���λ��
void ResetAnchorPointAndKeepSamePos( CCNode *pNode, CCPoint anchorpoint );