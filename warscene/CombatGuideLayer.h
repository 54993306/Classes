#ifndef _COMBATGUIDE_
#define _COMBATGUIDE_
#include "AppUI.h"
#include "Global.h"
#include "scene/layer/LayerManager.h"

//************************************************************************/
//
//							做引导显示处理
//
//************************************************************************/
namespace BattleSpace{
	class BaseRole;
	class WarScene;
	class WarMapData;
	class WarMapLayer;
	class WarAliveLayer;
	class TouchIntercept;
	class CombatGuideStep;
	struct ImageData;

	enum LayerZorder
	{
		Background_Z = -1,
		Cover_Z,
		PointSp_Z,
	};
	class CombatGuideLayer : public BaseLayer
	{
	public:
		CombatGuideLayer();
		~CombatGuideLayer();
		CREATE_FUNC(CombatGuideLayer);
		CC_PROPERTY(CombatGuideStep*,m_Step,GuideStep)
			virtual bool init();
		virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);			//触摸开始调用
		CCPoint getPointByMapGrid(int grid);

		void ResponseRect(CombatGuideStep* step);								//响应区域处理(确定区域,是否在区域添加精灵)
		void SingleGrid(CombatGuideStep* step);
		void UIGuide(CombatGuideStep* step);
		void CallAlive(CombatGuideStep* step);
		CCSprite* PointSprite(CCPoint&p,int direction,int spritetype);		
		void initScene(WarScene*scene);
		void ClearGuideLayer();
		void ImageArray(CombatGuideStep* step);
		void spineSkeleton(ImageData& data);
		void initguideImage(ImageData& data);

		bool getUIRect(CCRect& rect , int uiCode);
		CCNode* getUINode(int uiCode);

		void ImageTextGuide(CombatGuideStep* step);
		void backBottom(CombatGuideStep* step);
		void roleName(CombatGuideStep* step);
		void describe(CombatGuideStep* step);
		void captainMark(CombatGuideStep* step);
		void resetAlive(CombatGuideStep* step);
		void creaAliveByVector(vector<BaseRole*>VecAlive,CombatGuideStep* step);
	private:
		CCNode* m_root;
		WarScene* m_Scene;
		WarMapData* m_mapData;
		WarMapLayer* m_MapLayer;
		WarAliveLayer* m_AliveLayer;	
		vector<CCRect> m_VecRect;
		CCLayerColor* m_LayerColor;
		CCRenderTexture* m_RenderTexture;
	};
};
#endif