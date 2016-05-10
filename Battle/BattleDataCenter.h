/** 
 *
 *		Data : 2016.4.13
 *	
 *		Name : BattleDataCenter
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : ��������ս������,������ͼ������ؿ����ã����飬����Ч�������ݡ�
 *
 */

#ifndef __BATTLEDATACENTER__
#define __BATTLEDATACENTER__



#include <string>
#include "cocos2d.h"
#include "protos/protocol.h"


class BattleDataCenter
{
public:
	static BattleDataCenter* ShareBattleDataControl();

public:
	void BattleDataInit(BattleResponse *Response);



private:
	BattleDataCenter();
	static BattleDataCenter* m_DataControl;
	class SingletonDestroy
	{
	public :
		~SingletonDestroy(){
			if (m_DataControl != NULL )
			{
				delete m_DataControl;
				m_DataControl = NULL ;
			}
		}
	};
	static SingletonDestroy m_Destroy;
};

#define BattleData BattleDataCenter::ShareBattleDataControl()

#endif