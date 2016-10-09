#ifndef _PAIRCARDSACTIVITY_
#define _PAIRCARDSACTIVITY_

/************************************************************************/
/*
	�ļ����ƣ�PairCardsActivity.h
	��д���ߣ�WYS
	��д���ڣ�16-8-26
	���ܱ��������ƻ��
*/
/************************************************************************/

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "common/CommonFunction.h"

#include "tollgate/ItemInfo.h"
#include "tollgate/MonsterInfo.h"

#include "LuckyActive/LuckyWheelData.h"

enum GameState
{
	unStartGame = 1,
	StartGame,
	gameOver
};

class CMonsterInfo;
class CItemInfo;

#define SHOWPRIZELIST_SIZE 20

class PairCardsActivityLayer : public BaseLayer
{
public:
	PairCardsActivityLayer();
	~PairCardsActivityLayer();
	CREATE_LAYER(PairCardsActivityLayer);
	virtual bool init();
	void onEnter();
	void onExit();
	void onClose(CCObject* obj);
	void callFuncClose(CCObject* pSender);

	void ProcessMsg(int type ,google::protobuf::Message* msg);
	void gameRuleExplain(CCObject* pSender);
	void initCards();
	void initCardsData();
	void updateCardsData();
	void judegeHaveSameCardsAndAction(CPrize* item);
	void setCardsListTouchEnableTo(bool enable);
	void setImageAllChildVisibleTo(CImageView* image,bool visible);
	void updateUserData();

	void onPress(CCObject* obj, CTouchPressState state);
	void updateOneCards();
	void startAction();
	void callFuncNOpenCardsAction(CCNode* node);
	void callFuncNOpenCardsOverAction(CCNode* node);
	void isBuyOpenCardsNum(CCObject* pSender);
	void showPrizePopItem();
	void openAllCards();
	void callFuncNShowPrizePopItemAction(CCNode* node);
	void callFuncSetBtnToTure();

	void onStartGameBtnClick(CCObject* obj);
	void startGame();
	void isGiveUpThisGame(CCObject* pSender);
	void callFuncStartGameAction(CCNode* node);
	void callFuncShowPos();
	void callFuncStartGameOverAction();

	void initNotice();
	void updateNoticeLabelShow(float delta);

private:
	CItemInfo* m_itemInfo;
	CMonsterInfo* m_monsterInfo;

	CLayout* m_ui;
	CImageView* m_cardsImage;
	CImageView* m_cardsBlueFrame;
	CLabel* m_cardsNameLab;
	CImageView* m_cardsPosImage;

	CImageView* m_redPoint;
	CLabel* m_currFreeTimesLab;
	CLabel* m_noticeLabel;
	CButton* m_startGameBtn;
	CButton* m_closeBtn;
	CCTexture2D* m_texture_on;
	CCTexture2D* m_texture_off;

	//CPrize m_prize;
	vector<CImageView* > m_ImageList;
	vector<CCPoint > m_cardsPos;	//��¼����λ��
	vector<CPrize > m_prizeItemList;	//���տ����б�
	vector<CPrize >m_showItemList;	//�Ѵ򿪿��Ƶ��б�
	vector<CPrize >m_getPrizeList;	//��õ���Դ�б�
	vector<CNotice > m_noticeList;//���չ����б�
	vector<CLabel* >m_noticeLab;

	bool m_isFree;	//�Ƿ����
	int m_needGold_GameTimes;	//����Ѵ�������
	int m_needGold_otherPairCardTimes;	//���ⷭ�ƻ���
	int m_gameTimes_free;	//ÿ����ѷ�����
	int m_gameTimes_Buy;	//ÿ�ֿɹ������

	int m_cardsIndex;//18�ſ�������ֵ
	int m_index_click[50];//�Ѵ򿪿��Ƶ���������
	int m_gameNum;//��Ϸ����
	int m_cardsNum;//��Ƭ����
	int m_gameState;	//��Ϸ״̬
	int m_openCardsNum;	//�򿪿�Ƭ����
	int m_currCard;//��¼���ƶ���ʱcard

	bool isShowPrizePopItem;	//�Ƿ���ʾ��������
	int m_record_onPressTimes;	//��¼������ƴ���
	
	float m_pairTime;	//����scaleʱ��
	float m_minPosY;
	float m_maxPosY;
	int m_currNoticeLab;	//��ǰ��ʾ��������
	bool m_isUpdateNotice;

	bool m_isInit;
};

#endif 