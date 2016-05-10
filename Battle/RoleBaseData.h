/** 
 *
 *		Data : 2016.4.12
 *	
 *		Name : ServerRoleBaseData
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : �������������Ļ���������Ϣ(����˫������Ҫ����Ϣ)
 *
 */
#include "cocos2d.h"
using namespace std;
using namespace cocos2d;

class RoleBaseData : public CCObject
{
public:
	RoleBaseData();
	CC_SYNTHESIZE(int		,m_RoleID			,RoleID);						//�佫ID
	CC_SYNTHESIZE(int		,m_ServerID			,ServerID);						//�������ṩID
	CC_SYNTHESIZE(int		,m_Model			,Model);						//ģ��ID
	CC_SYNTHESIZE(bool		,m_Enemy			,Enemy);						//��Ӫ
	CC_SYNTHESIZE(float		,m_MoveSpeed		,MoveSpeed);					//�ƶ��ٶ�(��/��)
	CC_SYNTHESIZE(float		,m_AtkInterval		,AtkInterval);					//�����ٶ�(��/��)
	CC_SYNTHESIZE(int		,m_Attribute		,Attribute);					//�佫����(��1,ˮ2,ľ3)
	CC_SYNTHESIZE(int		,m_Row				,Row);							//�佫��ռ����
	CC_SYNTHESIZE(int		,m_Col				,Col);							//�佫��ռ����
	CC_SYNTHESIZE(int		,m_Grid				,Grid);							//�佫վλ
	CC_SYNTHESIZE(int		,m_Maxhp			,Maxhp);						//���Ѫ��ֵ
	CC_SYNTHESIZE(int		,m_Hp				,Hp);							//��ǰѪ��ֵ
	CC_SYNTHESIZE(int		,m_Attack			,Attack);						//����
	CC_SYNTHESIZE(int		,m_Defense			,Defense);						//����
	CC_SYNTHESIZE(int		,m_Crit				,Crit);							//����
	CC_SYNTHESIZE(int		,m_Hit				,Hit);							//����
	CC_SYNTHESIZE(int		,m_Dodge			,Dodge);						//����
	CC_SYNTHESIZE(int		,m_Agility			,Agility);						//����
	CC_SYNTHESIZE(int		,m_Revert			,Revert);						//�ظ�
	CC_SYNTHESIZE(int		,m_Zoom				,Zoom);							//���ű�
	CC_SYNTHESIZE(int		,m_RoleType			,RoleType);						//�佫����(�����ٻ��͹�����������)
	CC_SYNTHESIZE(bool		,m_Summon			,Summon);						//�Ƿ�Ϊ�ٻ����佫


	//int CallType;				//�ٻ�����(Control)
	//int MstType;				//��������
	//vector<CBuff> buffList;	//�佫����ս��ʱ��������buf
	//int hasitem;				//������Ʒ����1����2����3����4������5�����
	//int	level;				//�佫�ȼ�
	//string name;				//����
private:

};