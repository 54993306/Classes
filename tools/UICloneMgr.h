#ifndef UI_CLONE_MGR
#define UI_CLONE_MGR
#include "AppUI.h"

class UICloneMgr
{
public:
	//拷贝按钮
	static CButton* cloneButton(CCNodeRGBA *node);
	//拷贝图片
	static CImageView* cloneImageView(CImageView* imageView);
	//拷贝自定义字体文本
	static CLabelBMFont* cloneLabelBMFont(CLabelBMFont *label);
	//拷贝文本
	static CLabel* cloneLable(CLabel *label);
	//拷贝单选按钮
	static CRadioButton* cloneRadioBtn(CRadioButton *btn);
	//拷贝复选按钮
	static CCheckBox*  cloneCheckBox(CCheckBox *btn);

	//拷贝进度条
	static CProgressBar*  cloneProgressBar(CProgressBar *btn);

	static CLayout * cloneLayout(CLayout *lay);

	static CImageViewScale9* cloneImageViewScale9(CImageViewScale9* imageView);
	static CLabelAtlas* cloneAtlasLable(CLabelAtlas *label);
	//拷贝altas文本
//	static CLabelAtlas* cloneLabelAtlas(CLabelAtlas *label);
protected:
	static void cloneProperties(CCNode *destWidget, CCNode *srcWidget);
	static void cloneWidgetProperties(CWidget *destWidget, CWidget *srcWidget);
	

};


#endif // UI_CLONE_MGR
