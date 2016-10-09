/************************************************************* 
*
*
*		Data : 2016.9.2
*	
*		Name : ActionModel
*
*		Author : Lin_Xiancheng
*
*		Description : 
*
*
*************************************************************/

#ifndef __ActionModel_H_
#define __ActionModel_H_
#include "cocos2d.h"
using namespace cocos2d;
using namespace std;
namespace BattleSpace
{
	//��CCAnimation������֮�Ϸ�װ������
	class ActionModel : public CCObject
	{
	public:
		ActionModel();
		virtual ~ActionModel();
		inline bool init(){return true;}
		CREATE_FUNC(ActionModel);
	public:
		void setActionFrames(const char* action,int Frames){m_ActionFrames[action] = Frames;}			//���ö���֡��
		void setActionSpeed(const char* action,float speed){m_ActionSpeed[action] = speed;}				//���ö���֡��
		int getActionFrames(const char* action);														//��ȡ����֡��
		float getActionSpeed(const char* action);														//��ȡ����֡��
		float getActionTime(const char* action){return getActionFrames(action)*getActionSpeed(action);}	//����ʱ��

		CC_SYNTHESIZE(int,m_Frames,EffectFrames);												//��Ч֡��
		CC_SYNTHESIZE(float,m_Speed,EffectSpeed);												//��Ч֡��
		inline float getEffectTime(){return m_Speed* m_Frames;};								//��ȡ ��Ч��ʱ��
		CC_SYNTHESIZE(string,m_name,Name);														//���ļ�������Ϊ������
		CC_SYNTHESIZE(char,m_type,Type);														//���ö�������
		CCRect getRect();																		//����һ֡�Ĵ�С��Ϊ���������Ĵ�С����
		CCAnimation* getAnimation(const char* action = nullptr);								//������Ч����
	protected:					
		map<string,int> m_ActionFrames;													//����-��Ӧ֡��
		map<string,float> m_ActionSpeed;												//����-��Ӧ֡��
	};
};
#endif