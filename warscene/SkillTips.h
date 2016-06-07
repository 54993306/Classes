#ifndef _SKILLTIPS_
#define _SKILLTIPS_
#include "AppUI.h"
#include "scene/layer/LayerManager.h"
//长按技能提示实现
namespace BattleSpace{
	class WarAlive;

	class SkillTips : public BaseLayer
	{
	public:
		SkillTips();
		~SkillTips();
		virtual void onExit();
		virtual void onEnter();
		virtual bool init();
		CREATE_FUNC(SkillTips);
		void removeTips(CCObject* ob);
		bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
		void setSkillInfo(CCObject* target,WarAlive* alive);
	private:
		CLayout *m_ui;
	};
};
#endif