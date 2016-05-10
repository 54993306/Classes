#include "UICloneMgr.h"

void UICloneMgr::cloneProperties(CCNode *destWidget, CCNode *srcWidget)
{
	destWidget->setPosition(srcWidget->getPosition());
	destWidget->setAnchorPoint(srcWidget->getAnchorPoint());
	destWidget->setScaleX(srcWidget->getScaleX());
	destWidget->setScaleY(srcWidget->getScaleY());
	destWidget->setRotationX(srcWidget->getRotationX());
	destWidget->setRotationY(srcWidget->getRotationY());
	destWidget->setTag(srcWidget->getTag());
	destWidget->setVisible(srcWidget->isVisible());
	destWidget->setZOrder(srcWidget->getZOrder());
}

CButton* UICloneMgr::cloneButton(CCNodeRGBA *node)
{
	 CButton *button = dynamic_cast<CButton*>(node);
	 if (button)
	 {
		 CButton *btn = CButton::create();
		 btn->setNormalTexture(dynamic_cast<CCSprite*>(button->getNormalImage())->getTexture());
		 if (button->getSelectedImage())
		 {
			 btn->setSelectedTexture(dynamic_cast<CCSprite*>(button->getSelectedImage())->getTexture());
		 }
		 if (button->getDisabledImage())
		 {
			 btn->setDisabledTexture(dynamic_cast<CCSprite*>(button->getDisabledImage())->getTexture());
		 }
	     cloneProperties(btn, button);
		 cloneWidgetProperties(btn, button);
		 return btn;
	 }
	 return nullptr;
}

CImageView* UICloneMgr::cloneImageView(CImageView* imageView)
{
	CImageView *image = CImageView::create();
	image->setTexture(imageView->getTexture());
	image->setTextureRect(imageView->getTextureRect());
	cloneProperties(image,imageView);
	cloneWidgetProperties(image,imageView);
	return image;
}

CImageViewScale9* UICloneMgr::cloneImageViewScale9(CImageViewScale9* imageView)
{
	CImageViewScale9 *image = CImageViewScale9::create(imageView->getFile());
	image->setContentSize(imageView->getContentSize());
	cloneProperties(image,imageView);
	cloneWidgetProperties(image,imageView);
	return image;
}

CLabelBMFont* UICloneMgr::cloneLabelBMFont(CLabelBMFont *label)
{
	CLabelBMFont *labBm = CLabelBMFont::create();
	labBm->setFntFile(label->getFntFile());
	labBm->setString(label->getString());
	labBm->setColor(label->getColor());
	cloneProperties(labBm,label);
	cloneWidgetProperties(labBm,label);
	return labBm;
}

CLabel* UICloneMgr::cloneLable(CLabel *label)
{
	CLabel *lab = CLabel::create();
	lab->setFontName(label->getFontName());
	lab->setFontSize(label->getFontSize());
	lab->setString(label->getString());
//	lab->setHorizontalAlignment(label->getHorizontalAlignment());
	lab->setDimensions(label->getDimensions());
	lab->setColor(label->getColor());

	cloneProperties(lab,label);
	cloneWidgetProperties(lab,label);
	return lab;
}

CLabelAtlas* UICloneMgr::cloneAtlasLable(CLabelAtlas *label)
{
	CLabelAtlas* lab = CLabelAtlas::create(label->getString(), label->getMapFile(), label->getItemWidth(), label->getItemHeight(),label->getStartCharMap());
	cloneProperties(lab,label);
	cloneWidgetProperties(lab,label);
	return lab;
}

CRadioButton* UICloneMgr::cloneRadioBtn(CRadioButton *btn)
{
	CRadioButton *button = CRadioButton::create();
	button->setNormalTexture(btn->getNormalSprite()->getTexture());
	button->setCheckedTexture(btn->getCheckSprite()->getTexture());
	if (btn->getCheckPressSprite())
	{
		button->setNormalPressTexture(btn->getCheckPressSprite()->getTexture());
	}
	if (btn->getDisableNormalSprite())
	{
		button->setDisabledNormalTexture(btn->getDisableNormalSprite()->getTexture());
	}

	cloneProperties(button,btn);
	cloneWidgetProperties(button,btn);
	return button;
}

CCheckBox* UICloneMgr::cloneCheckBox(CCheckBox *btn)
{
	CCheckBox *button = CCheckBox::create();
	button->setNormalTexture(btn->getNormalSprite()->getTexture());
	button->setCheckedTexture(btn->getCheckSprite()->getTexture());
	if (btn->getCheckPressSprite())
	{
		button->setNormalPressTexture(btn->getCheckPressSprite()->getTexture());
	}
	if (btn->getDisableNormalSprite())
	{
		button->setDisabledNormalTexture(btn->getDisableNormalSprite()->getTexture());
	}
	cloneProperties(button,btn);
	cloneWidgetProperties(button,btn);
	return button;
}

void UICloneMgr::cloneWidgetProperties(CWidget *destWidget, CWidget *srcWidget)
{
	destWidget->setEnabled(srcWidget->isEnabled());
	destWidget->setTouchEnabled(srcWidget->isTouchEnabled());
	if (srcWidget->getId())
	{
		destWidget->setId(srcWidget->getId());
	}
}
// 
// CLabelAtlas* UICloneMgr::cloneLabelAtlas(CLabelAtlas *label)
// {
// 	
// }

//拷贝进度条
CProgressBar* UICloneMgr::cloneProgressBar(CProgressBar *btn)
{
	CProgressBar *bar = CProgressBar::create();
	if (btn->getBackgroundImage())
	{	
		bar->setBackgroundTexture(btn->getBackgroundImage()->getTexture());	
	}
	bar->setProgressTexture(btn->getProgressSprite()->getTexture());
	bar->setMaxValue(btn->getMaxValue());
	bar->setMinValue(btn->getMinValue());
	bar->setValue(btn->getValue());
	cloneProperties(bar, btn);
	cloneWidgetProperties(bar,btn);
	return bar;
}

CLayout * UICloneMgr::cloneLayout(CLayout *lay)
{
	CCArray *children = lay->getChildren();

	CLayout *layout = CLayout::create(); 
	cloneProperties(layout,lay);
	cloneWidgetProperties(layout,lay);

	layout->setContentSize(lay->getContentSize());

	CWidget *clone = nullptr;

	for (int i=0; i<lay->getChildrenCount(); ++i)
	{
		CWidget *pWidget = dynamic_cast<CWidget*>(children->objectAtIndex(i));
		const char* name = pWidget->getDescription();
		if( strcmp( name, "Image" ) == 0 )
		{
			clone = cloneImageView((CImageView*)pWidget);
		}
		else if( strcmp( name, "TextBlock" ) == 0 )
		{
			clone = cloneLabelBMFont((CLabelBMFont*)pWidget);		
		}
		else if( strcmp( name, "Button" ) == 0 )
		{
			clone = cloneButton((CButton*)pWidget);
		}
		else if( strcmp( name, "UI:CCButton" ) == 0 )
		{
			clone = cloneButton((CButton*)pWidget);
		}
// 		else if( strcmp( name, "UI:AtlasLabel" ) == 0 )
// 		{
// 			clone = cloneButton((CButton*)pWidget);
// 		}
// 		else if( strcmp( name, "UI:ToggleButton") == 0 )
// 		{
// 			clone = cloneButton((CButton*)pWidget);
// 		}
// 		else if( strcmp( name, "UI:Slider" ) == 0 )
// 		{
// 			clone = cloneButton((CButton*)pWidget);
// 		}
		else if ( strcmp( name, "UI:ProgressBar")==0)
		{
			clone = cloneProgressBar((CProgressBar*)pWidget);
		}
		else if (strcmp(name, "UI:CheckBox") ==0)
		{
			clone = cloneCheckBox((CCheckBox*)pWidget);
		}
		else if (strcmp(name, "UI:RadioButton") ==0)
		{
			clone = cloneRadioBtn((CRadioButton*)pWidget);
		}
		else if( strcmp( name, "Canvas" ) == 0 )
		{
			clone = cloneLayout((CLayout*)pWidget);
		}
		else if (strcmp(name, "Label") ==0)
		{
			clone = cloneLable((CLabel*)pWidget);
		}
		else if (strcmp(name,"UI:AtlasLabel")==0)
		{
 			clone = cloneAtlasLable((CLabelAtlas*)pWidget);
		}
		else if (strcmp(name,"UI:ImageViewScale9")==0)
		{
			clone = cloneImageViewScale9((CImageViewScale9 *)pWidget);
		}
		if (clone)
		{		
			layout->addChild(dynamic_cast<CCNode*>(clone));
		}
	}
	return layout;
}
