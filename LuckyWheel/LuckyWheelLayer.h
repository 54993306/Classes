#ifndef _LUCKYWHEELLAYER_
#define _LUCKYWHEELLAYER_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "common/CommonFunction.h"

#include "tollgate/ItemInfo.h"
#include "tollgate/MonsterInfo.h"

#include "LuckyWheel/LuckyWheelData.h"

#include <spine/spine-cocos2dx.h>
using namespace spine;

class CMonsterInfo;
class CItemInfo;

class CLuckyWheelLayer : public BaseLayer
{
public:
	CLuckyWheelLayer(void);
	~CLuckyWheelLayer(void);
	CREATE_LAYER(CLuckyWheelLayer);
	virtual bool init();
	void onEnter();
	void onExit();
	void onClose(CCObject* obj);
	void onUpdateItemListClick(CCObject* pSender);

	void onWheelBtnOnceClick(CCObject* obj);
	void onWheelBtnTenClick(CCObject* obj);
	void isOKTenBtn(CCObject* pSendr);

	void randomItemArray();//������н����б�
	void followAngle();//�Ʊ����ָ��
	void showOnceRotate();
	void showTenRotate();
	void callBackForTenActionGoing();
	void callBackForActionEnd();
	void callBackForShowGainPrize();

	void ProcessMsg(int type, google::protobuf::Message* msg);//����ص�
	void updateLotteryItem();	//���½����б�
	void initNoticeLabelShow();	//��ʼ��������ʾ
	void updateNoticeLabelShow();//���¹�����ʾ
	
	void initUILightOffEffect();//��ʼ���ƹ�Ч��
	void updateLightActionEffect(float delta);//�ƹ���ʾЧ��������
	void updateNoticeLabelShowSchedule(float delta);//���¹�����Ϣ����������

	void updateUserData();
	void onPress(CCObject* obj, CTouchPressState state);

private:
	CMonsterInfo* m_monsterInfo;
	CItemInfo* m_itemInfo;

	CLayout* m_ui;
	CLayout* m_item;
	CImageView* m_pointer;
	CImageView* m_prizeItemTag;
	CButton* m_onceBtn;
	CButton* m_tenBtn;
	CButton* m_closeBtn;

	int m_randomAng;	//����Ƕ�
	int m_iRotateAngle;	//��ת�Ƕ�
	unsigned int m_pItemListNum;	//��������
	int m_itemRandomIndex[10];		//��������
	int m_prizeIndex[10];		//��������

	int m_lotteryType;			//�齱���� 1.once 2.ten
	int m_recordTime;			//��¼����
	bool m_isRotateAction;
	CPrize m_getOncePrize;			//��һ�λ��
	vector<CPrize > m_getPrizeList;	//��ʮ�λ�ȡ

	vector<CPrize > m_prizeList;
	vector<CNotice >m_noticeList;
	int m_onceNeedGold;
	int m_tenNeedGold;

	vector<CLayout* > m_noticeLab;
	int m_showNoticeLabNum;
	float m_minPos;
	float m_maxPos;

	int m_lightNum ; //��¼light����������
	int m_changelight;
	float m_iPrizeAngle;//�����ʾ�ĽǶ�

	int m_startRotateTime;//��¼��ʼ��תʱ��
	bool m_isSlow;	//�Ƿ��ڼ���

	bool m_isUpdateItemList;//�Ƿ����Item�б�
};

#endif