#ifndef __GAME_EDITOR_LAYER_H__
#define __GAME_EDITOR_LAYER_H__

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "common/CursorTextField.h"

class GameEditorLayer : public BaseLayer
{
public:
	GameEditorLayer();
	virtual ~GameEditorLayer();
	CREATE_FUNC(GameEditorLayer);
	virtual bool init();
	void OnClicked(CCObject* obj);
	void ToggleButtonClickedHandler(CCObject* obj);
	void AliveObjectClickedHandler(CCObject* obj);
	//virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);

private:
	CursorTextField** m_textFields;
	//m_textFieldColdDown
};
#endif // __GAME_EDITOR_LAYER_H__