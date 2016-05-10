#include "CommonFunction.h"
//#include "Vector2D.h"
#include "platform/CCEGLViewProtocol.h"
#include "platform/CCFileUtils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <typeinfo>
#include "warscene/ConstNum.h"
#include "model/DataDefine.h"
#include "model/DataCenter.h"
#include "GMessage.h"
#include "common/CGameSound.h"
#include "common/ShaderDataHelper.h"

//��������,�������ܴ���10
void runEffect_ShakeX(CCNode *node, float range, float runTime, int times,CCCallFuncN* pOverCallBack/*=NULL*/)
{
	if (node == NULL || times <= 0)
		return;

	CCPoint point = node->getPosition();
	CCPoint point1 = point;
	runTime /= times;

	CCMoveBy * move1 = CCMoveBy::create(runTime/2, ccp(range, 0));
	CCActionInterval  * move2 = move1->reverse();
	CCSequence * actionOnce = CCSequence::create(move1, move2, NULL);
	CCRepeat * actionLoop = CCRepeat::create(actionOnce, times);

	if (pOverCallBack) {
		CCAction * action = CCSequence::create(actionLoop, pOverCallBack, NULL);
		node->runAction(action);
	} else {
		node->runAction(actionLoop);
	}
	node->setPosition(point1);
}
void runEffect_ShakeY(CCNode *node, float range, float runTime, int times,CCCallFuncN* pOverCallBack/*=NULL*/)
{
	if (node == NULL || times <= 0)
		return;

	CCPoint point = node->getPosition();
	runTime /= times;
	CCFiniteTimeAction * actionLoop = NULL;
	CCMoveTo * move1 = CCMoveTo::create(runTime/4, ccp(point.x, point.y+range));
	CCMoveTo * move2 = CCMoveTo::create(runTime/4, ccp(point.x, point.y));
	CCFiniteTimeAction * actionOnce = CCSequence::create(move1, move2, NULL);
	actionLoop = CCRepeat::create((CCActionInterval*)actionOnce, times);

	if (pOverCallBack) {
		CCAction * action = CCSequence::create(actionLoop, pOverCallBack, NULL);
		node->runAction(action);
	} else {
		node->runAction(actionLoop);
	}
	node->setPosition(point);
}

void srandNum()
{
	time_t tt;
	time(&tt);
	struct tm * now;
	//��ñ���ʱ��
	now = localtime(&tt);
	/*CCLog("%d/%d/%d %d:%d:%d",now->tm_year+1900,now->tm_mon+1,
	now->tm_mday,now->tm_hour,now->tm_min,now->tm_sec);*/
	//ͨ��CCRANDOM_0_1()��õ��������ʵ����������ģ�Ҫ���ȡ�ӽ���ʵ���������Ҫ����һ������
	//�Ƚϳ��õķ������Ե�ǰʱ��Ϊ���ӣ���cocos2d-x���ṩ�˻�ȡʱ��ķ��������һ�ȡ��ʱ�����ȷ
	//cc_timeval�Ǹ��ṹ�壬���������������һ��������Ϊ��λ�ģ�һ������΢��Ϊ��λ��
	cc_timeval tv;
	CCTime::gettimeofdayCocos2d(&tv,NULL);
	//��ת��Ϊ����
	unsigned long reed = tv.tv_sec*1000+tv.tv_usec/1000;
	//srand()�д���һ�����������
	srand(reed);

	//���Ҫ���a~b֮���һ�����������Ӧ����CCRANDOM_0_1*(b-a)+a������Ĳ�������a��b
	//CCRANDOM_0_1()��ȡһ��0-1֮���float������
	return;
}
//�ж϶����Ƿ�Ϊ�գ��������Ϊ���򷵻��沢�Ҵ�ӡ������Ϣ�����򷵻ؼٱ�ʾ����Ϊ��
bool outPutERRORMsg(const char* address,void* ob)
{
	if (ob)return false;
	CCLOG("/************************************************************************/");
	CCLOG("[ *ERROR ] %s",address);
	CCLOG("/************************************************************************/");
	return true;
}

void LGPause( CCNode* pNode , bool AllPause /*=false*/)
{
	CCBool* UnPause = dynamic_cast<CCBool*>(pNode->getUserObject());
	if (!AllPause && UnPause && UnPause->getValue())
		return;
	pNode->pauseSchedulerAndActions();
	CCArray* children = pNode->getChildren();
	CCObject* obj = nullptr;
	CCARRAY_FOREACH(children,obj)
	{
		CCNode* child = (CCNode*)obj;
		LGPause(child); 
	}
}
void LGResume( CCNode* pNode )
{
	pNode->resumeSchedulerAndActions();
	CCArray* children = pNode->getChildren();
	CCObject* obj = nullptr;
	CCARRAY_FOREACH(children,obj)
	{
		CCNode* child = (CCNode*)obj;
		LGResume(child);
	}
}

void QuickSort( std::vector<int>&Vec,int left,int right )
{
	if (right>left)
	{
		int i=left;
		int j=right;
		int saveNum = Vec.at(i);
		while (i<j)
		{
			while (i<j&&saveNum>=Vec.at(j))
				j--;
			if (i<j)
			{
				Vec.at(i)=Vec.at(j);
				i++;
			}
			while (i<j&&Vec.at(i)>saveNum)
				i++;
			if(i<j)
			{
				Vec.at(j) = Vec.at(i);
				j--;
			}
		}
		Vec.at(i) = saveNum;
		QuickSort(Vec,left,i-1);
		QuickSort(Vec,i+1,right);
	}
}

//��丸�ڵ�
void NodeFillParent( CCNode* pNode, CCNode* pParent/*=nullptr*/ )
{
	if(!pNode)return;
	if(pParent == nullptr)
	{
		pParent = pNode->getParent();
		if(!pParent)return;
	}
	CCSize parentSize = pParent->getContentSize();
	pNode->setAnchorPoint(ccp(0.5f, 0.5f));
	pNode->setPosition(ccp(parentSize.width*0.5f, parentSize.height*0.5f));
	pNode->setScale(parentSize.width/pNode->getContentSize().width);
}

void NodeFillNode( CCNode* pNode, CCNode* pAimNode )
{
	CCSize parentSize = pAimNode->getContentSize();
	pNode->setAnchorPoint(ccp(0.5f, 0.5f));
	pNode->setPosition(ccp(parentSize.width*0.5f, parentSize.height*0.5f));
	pNode->setScale(parentSize.width/pNode->getContentSize().width);
}


//gridview ����λ��ˢ��
void ReloadGridViewWithSameOffset(CGridView* pGridView)
{
	CCPoint offSet = pGridView->getContentOffset();
	pGridView->reloadData();
	pGridView->setContentOffset(offSet);
}

void ReloadTableViewWithSameOffset( CTableView* pTableView )
{
	CCPoint offSet = pTableView->getContentOffset();
	pTableView->reloadData();
	pTableView->setContentOffset(offSet);
}

bool checkRectInScreen( CCRect rect, float fBorder )
{
	const CCRect& rectWindow = CCDirector::sharedDirector()->getRunningScene()->boundingBox();
	if(! (rect.getMinY()>rectWindow.getMinY()+fBorder))
	{
		return false;
	}
	if(! (rect.getMinX()>rectWindow.getMinX()+fBorder))
	{
		return false;
	}
	if(! ( rect.getMaxY()<rectWindow.getMaxY()-fBorder) )
	{
		return false;
	}
	if(! (rect.getMaxX()<rectWindow.getMaxX()-fBorder) )
	{
		return false;
	}
	return true;
}

std::string GetImageName( int iType, int iColor, int iThumb )
{
	std::string str;
	//��������(1 ���ߣ�2 Ӣ�ۣ�3 ͭǮ��4 Ԫ����5 ��ʳ)
	switch (iType)
	{
	case 0:
	case 1:
	case 3:
	case 4:
	case 5:
	case 6:
		{
			if(iColor>=10)//��ʯ,�ؼ�
			{
				str ="headImg/";
			}
			else
			{
				str = "prop/";
			}
		}
		break;
	case 2:
		{
			str ="headImg/";
		}
		break;
	default:
		break;
	}
	return CCString::createWithFormat("%s%d.png", str.c_str(), iThumb)->m_sString;
}

int GetQualityByType( int iType, int iQuality, int iColor )
{
	//Ӣ��
	if(iType==2)
	{
		return iColor;
	}
	//����
	return iQuality;
}

std::string getImageName( CHero* hero )
{
	std::string str ="headImg/";
	return CCString::createWithFormat("%s%d.png", str.c_str(), hero->thumb)->m_sString;
}

std::string getImageName( CMonster* monster )
{
	std::string str ="headImg/";
	return CCString::createWithFormat("%s%d.png", str.c_str(), monster->thumb)->m_sString;
}

std::string getImageName( CArmor* armor )
{
	std::string str ="headIcon/";
	return CCString::createWithFormat("%s%d.png", str.c_str(), armor->hero)->m_sString;
}

void collectMoneyFromPrize( CGetPrizeRes& prizeRes )
{
	UserData *data = DataCenter::sharedData()->getUser()->getUserData();

	int iSize = prizeRes.prizeList.size();
	//��������(1 ���ߣ�2 Ӣ�ۣ�3 ͭǮ��4 Ԫ����5 ��ʳ�� 6 ����)
	for(unsigned int i=0; i<iSize; i++)
	{
		const CPrize& prize = prizeRes.prizeList.at(i);
		int iType = prize.type;
		switch (iType)
		{
		case 3:
			{
				data->setCoin(data->getCoin()+prize.num);
			}
			break;
		case 4:
			{
				data->setRoleGold(data->getRoleGold()+prize.num);
			}
			break;
		case 5:
			{
				data->setRoleFood(data->getRoleFood()+prize.num);
			}
			break;
		case 6:
			{
				data->setRoleAction(data->getRoleAction()+prize.num);
			}
			break;
		default:
			break;
		}
	}
	CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO, 0, nullptr,nullptr);
}

void showBookAnimate(CLayout* pNode, bool bToLeft)
{
	CCNode* pPage = (CCNode*)pNode->findWidgetById("page");

	CCSprite* p2 = CCSprite::create("common/book_cover.png");
	p2->setPosition(pNode->convertToWorldSpace(pPage->getPosition()));
	pNode->getParent()->addChild(p2, 99);
	p2->setAnchorPoint(ccp(0.0f, 0.0f));
	p2->setOpacity(225);

	float fTime = 0.12f;

	float fBaseScaleX = -0.5f;
	float fAimScaleX = 1.0f;
	if(bToLeft)
	{
		fBaseScaleX = 0.5f;
		fAimScaleX = -1.0f;
	}

	p2->setSkewY(2);
	p2->setScaleX(fBaseScaleX);

	p2->runAction(CCSequence::create(
		CCEaseSineOut::create(CCSkewTo::create(fTime, 0, 4)), 
		CCEaseSineIn::create(CCSkewTo::create(fTime, 0, 0)),
		CCFadeOut::create(0.1f),
		CCRemoveSelf::create(),
		nullptr));

	p2->runAction(CCSequence::createWithTwoActions(
		CCEaseSineIn::create(CCScaleTo::create(fTime, 0, 1)), 
		CCEaseSineOut::create(CCScaleTo::create(fTime, fAimScaleX, 1))));

}

int getCombatLevel( int combat )
{
	int level = 0;
	if (combat>12000)
	{
		level = 1;//s
	}
	else if (combat>9000)
	{
		level = 2;//a
	}
	else if (combat>6000)
	{
		level = 3;//b
	}
	else if (combat>4000)
	{
		level = 4;//c
	}
	else if (combat>2000)
	{
		level = 5;//d
	}
	else if (combat>1000)
	{
		level = 6;//e
	}
	else
	{
		level = 7;//f
	}

	return level;
}

CLayout* getStarLayout(int iStarCount, float fScale/*=0.4f*/)
{
	CLayout* pLayout = CLayout::create();
	pLayout->setAnchorPoint(CCPointZero);
	CCSize size = CCSizeZero;
	for(unsigned int i=0; i<iStarCount; i++)
	{
		CCSprite* pSprite = CCSprite::create("common/xingxing.png");
		pSprite->setPosition(CCPointZero+ccp(pSprite->getContentSize().width*fScale*0.8*i, 0));
		pSprite->setScale(fScale);
		pLayout->addChild(pSprite);
		size = pSprite->getContentSize();
	}
	pLayout->setPosition(ccp(size.width*fScale*0.8*(5/2.0f-0.5f*(iStarCount-1)), 0)+ccp(14, 10));
	return pLayout;
}


CLayout* getStarLayoutWithBlackBase( int iStarCount, float fScale/*=0.4f*/ )
{
	CLayout* pLayout = CLayout::create();
	pLayout->setAnchorPoint(CCPointZero);
	for(unsigned int i=0; i<5; i++)
	{
		CCSprite* pSprite = CCSprite::create("common/xingxing.png");
		pSprite->setPosition(CCPointZero+ccp(pSprite->getContentSize().width*fScale*0.8*i, 0));
		pSprite->setScale(fScale);
		pLayout->addChild(pSprite);
		if(i>=iStarCount)
		{
			pSprite->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
		}
	}
	return pLayout;
}

void PlaySoundEvent( int sData )
{
	char str[60] = {0};
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	sprintf(str,"SFX/Test/%d.mp3",sData);											
#else
	sprintf(str,"SFX/%d.ogg",sData);												
#endif
	PlayEffectSound(str);				//����Ч����Ч
}

std::string getArmorTypeStr( int iType )
{
	//(1 ������2 ���ף�3 ñ�ӣ�4 Ь��)
	switch (iType)
	{
	case 1: return GETLANGSTR(77);
	case 2: return GETLANGSTR(78);
	case 3: return GETLANGSTR(80);
	case 4: return GETLANGSTR(79);
	default:
		break;
	}
	return "";
}
