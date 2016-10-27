/************************************************************* 
*
*	
*		Name : SpineFloorLayer
*
*		Author : Lin_Xiancheng
*
*		Time : [10/22/2016 ]
*
*		Description :  
*
*
*************************************************************/
#include "cocos2d.h"
using namespace std;
using namespace cocos2d;


namespace BattleSpace
{
	class BaseRole;
	class SpineFloorLayer : public CCNode
	{
	public:
		SpineFloorLayer();
		~SpineFloorLayer();
		CREATE_FUNC(SpineFloorLayer);
		void initSpineEffect(CCObject* ob);
	protected:
		virtual bool init();
	};
};
