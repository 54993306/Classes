#ifndef CPOP_TIP_LAYER
#define CPOP_TIP_LAYER

#include "AppUI.h"
#include "scene/layer/LayerManager.h"

enum PopTip
{
	PopTipConfirm,
	PopTipCancel
};

class CPopTip:public BaseLayer
{
public:
	CPopTip();
	~CPopTip();
	//CC_WIDGET_BACKGROUND;
	virtual bool init();
	CREATE_LAYER(CPopTip);
	/*static CPopTip* create();*/
	void onEnter();
	void onExit();
	//提示标题
	void setTitle(const char *title, const ccColor3B &color,char* fontName="Arial", float size=16);
	//玩家确认，取消按钮
	void addButton(char *norImg, char *selectImg, char *text=nullptr,int tag =0,float scale =1.0f);
	//提示的内容
	void addContentTip(const char *content, const ccColor3B &color = ccWHITE, char* fontName="Arail",float span = 18 ,float size = 30);
	//按钮响应
	void setButtonLisener(CCObject* pListener, SEL_ClickHandler pHandler);
	//设置是否点击窗口外关闭窗口
	void setIsTouchOutClose(bool isClose);
	//设置按钮离背景框底偏移
	void setBottomOffset(float offset);
	//添加前景图片
	void addBeforeImage(char *img, float offsetX, float offsetY);
	//按钮上添加image
	void buttonAddImage(char *img, int tag);

	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

	CButton *getButtonByTag(int tag);

	void runScaleAction();

	void runActionWithUi(CCAction* pAction);

	//添加富文本
	void addTextRichTip(const char *content, const ccColor3B &color = ccRED, char* fontName="Arail", float size = 13);
	void addRichImage(char *img);
	void reloadTextRich(const CCPoint &pos);
	void onClose(CCObject * pSender);
	void setBackImgScale(float scalex, float scaley);

	//只显示确定按钮
	void showConfirmOnly();
	CCNode *getConfirmButton();

protected:
	void setBackgroundImage(const char* pFile);
private:
	CCObject *m_listener;
	CCArray *m_btnList;
	SEL_ClickHandler m_handler;
	CLabel *m_tipContent;
	CLabel *m_title;

	CTextRich *m_textTip;

	CCNode* m_pBackgroundImage;
	bool m_isOutClose;
	float m_bottomOffset;

	CLayout* m_ui;
};
#endif