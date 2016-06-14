#ifndef __BATTLERESULT__
#define __BATTLERESULT__

#include "AppUI.h"
/************************************************************* 
 *
 *
 *		Data : 2016.6.14
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/
//ÿһ����Ѫ���ݣ�����Ϊ��Ѫ���ͺ͵�Ѫ��ֵ
namespace BattleSpace{

	struct STR_LostHp
	{
		STR_LostHp();
		int hitType;
		int hitNum;			//�������Ϊ����,���ת��Ϊ�����˺�
		int anger;
	};
	class BaseRole;
	class ActObject;
	class BattleResult : public CCObject
	{
	public:
		BattleResult();
		virtual ~BattleResult();
		virtual bool init(){return true;}
		CREATE_FUNC(BattleResult);			
		vector<unsigned int> m_HitTargets;								//�ܻ��佫ID
		map<unsigned int,BaseRole*> m_Alive_s;							//�洢�ܻ�������佫��Ϣ
		map<unsigned int,STR_LostHp> m_LostHp;
		map<unsigned int,int> m_CurrHp;									//ԭ��������Ѫ�����仯,��������
		map<unsigned int,int> m_Repel;									//��¼���˸���
		map<unsigned int,unsigned int> m_AddBuf;
		map<unsigned int,unsigned int> m_DelBuf;
		CC_SYNTHESIZE(BaseRole*,m_alive,Alive);
		CC_SYNTHESIZE(int,m_usType,usType);								//����Ŀ��Ѫ���仯����
		CC_SYNTHESIZE(int,m_usNum,usNum);								//����Ŀ��Ѫ���仯
	protected:
	};
};
#endif