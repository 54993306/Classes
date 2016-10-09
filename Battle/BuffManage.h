#ifndef _BUFF_MANAGER_H_
#define _BUFF_MANAGER_H_
/************************************************************* 
 *
 *
 *		Data : 2016.5.27
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/
#include "cocos2d.h"
#include <map>
using namespace std;
using namespace cocos2d;
namespace BattleSpace
{
	class BaseRole;
	class BuffData;
	class BattleBuff;
	typedef map<int,BattleBuff*> BuffMap;	
	typedef map<int,vector<CCNode*>>	BuffEffectMapList;	

	class BuffManage : public CCObject
	{
	public:
		BuffManage();
		virtual ~BuffManage();
		CREATE_FUNC(BuffManage);
		CC_SYNTHESIZE(BaseRole*,mRole,Alive);
		void Buffclear();
		void clearDbuf();											//清除减益buf
		BattleBuff* getbuff(int bufID);
		BattleBuff* getbuffbyType(int buftype);
		void removeBuf(int id);
		void AddBuff(const BuffData* pData);							//将buff对象添加到武将身上
		bool AddBuffLogic(const BuffData* pData);						//buf替换添加逻辑
		void maxBuffJudge();
		void upDateBuff(float dt);
		BuffMap* getBuffMap(){return &mBuffMap;}
		vector<CCNode*>* getVecEffect(int buffid);					//判断是否已经存在效果数组
		void AddEffectVec(int buffid,vector<CCNode*>& Vec);
		BuffEffectMapList* getEffectMap(){return &m_EffectMap;}
	protected:
		BuffMap mBuffMap; 
		BuffEffectMapList m_EffectMap;										//存储武将身上的表现效果。
		virtual bool init();			
	};
};
#endif // _BUFFMANAGE_