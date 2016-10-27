/************************************************************* 
 *
 *	
 *		Name : SpineEffect
 *
 *		Author : Lin_Xiancheng
 *
 *		Time : [10/22/2016 ]
 *
 *		Description :  ���ӵ����ƣ�ֻ����û���ٶȺͷ��򣬴�����ʽ��֡�¼������˺�
 *
 *
 *************************************************************/
#include "cocos2d.h"
#include "Battle/SpineDataManage.h"
using namespace std;
using namespace cocos2d;
namespace BattleSpace
{
	class BaseRole;
	class skEffectData;
	class SpineEffect : public CCNode
	{
	protected:
		SpineEffect(BaseRole* pRole,int pGrid);
	public:
		static SpineEffect* createSpineEffect(BaseRole*pRole,int pGrid);
	private:
		void AttackExcute();
		void initHitRole();
		void SpineActionEvent(int trackIndex,spEvent* Event);
	private:
		int mGrid;//��ǰspine���˺���Χֻ��һ������(���������չΪ������˺����ǽ����˺�),���ܻ�Ŀ���ж�
		int mSortieNumber;
		CCNode* mAnimation;
		BaseRole* mRole;
		BaseRole* mHitRole;
		skEffectData* mSkillEffect;
	};
};
