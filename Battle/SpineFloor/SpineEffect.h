/************************************************************* 
 *
 *	
 *		Name : SpineEffect
 *
 *		Author : Lin_Xiancheng
 *
 *		Time : [10/22/2016 ]
 *
 *		Description :  跟子弹类似，只不过没有速度和方向，触发方式是帧事件触发伤害
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
		int mGrid;//当前spine的伤害范围只是一个格子(后面可能拓展为多格子伤害或是溅射伤害),做受击目标判断
		int mSortieNumber;
		CCNode* mAnimation;
		BaseRole* mRole;
		BaseRole* mHitRole;
		skEffectData* mSkillEffect;
	};
};
