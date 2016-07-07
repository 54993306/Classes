#include "UIFactory.h"
#include "Global.h"
#include "tools/StringUtil.h"

#include "common/PixelButton.h"
#include "common/CGameSound.h"

//#include "common/MultLanguage.h"

//#define CREATE_WITH_SPRITE_FRAME_NAME
//#define DISABLE_ANTIALIAS

//按钮九宫格默认样式
static char* ButtonDefaultBackgound = NULL;
void SetButtonDefaultBackground( const char* source ){ ButtonDefaultBackgound = (char*)source; }
static char* ButtonClickBackgound = NULL;
void SetButtonClickBackground( const char* source ){ ButtonClickBackgound = (char*)source; }

//默认矢量字体
#define DEFAULT_FONT "supermarket.ttf"//"Microsoft YaHei"		
#define DEFAULT_FONT1 "Arial"

//默认BMFont字体
static const char* BMFontDefault = "fonts/font.fnt";
void SetBMFontDefault( const char* font ){ BMFontDefault = font; }

//xml组件////////////////////////////////////////////////////////////////////////////////////////////
// CWidgetTouchModel SndButton::onTouchBegan(CCTouch *pTouch)
// {
// 	CWidgetTouchModel val = CButton::onTouchBegan(pTouch);
// 	CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(BUTTON1_MUSIC);
// 	return val;
// }


cocos2d::cocoswidget::CWidgetTouchModel SndButton::onTouchBegan( CCTouch* pTouch )
{
	CWidgetTouchModel val = CButton::onTouchBegan(pTouch);
	if(m_bPlaySound)
	{
		PlayEffectSound(SFX_Button);
	}
	return val;
}

void SndButton::onTouchEnded(CCTouch* pTouch, float fDuration)
{
	CButton::onTouchEnded(pTouch,fDuration);
}

SndButton* SndButton::create(const char* pNormal, const char* pSelected /*= NULL*/, const char* pDisabled /*= NULL*/)
{
	SndButton* pRet = new SndButton();
	if( pRet && pRet->initWithFile(pNormal, pSelected, pDisabled) )
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return NULL;
}

void SndButton::setPlaySound(bool isPlay)
{
	m_bPlaySound = isPlay;
}

static float SCREEN_HEIGHT = 0;
float getOffsetX(tinyxml2::XMLElement* XMLChild)
{
	float offset = 0;
	bool left = false;
	bool right = false;
	if( XMLChild->Attribute("AllowDrop") != NULL && strcmp( XMLChild->Attribute("AllowDrop"), "True" ) == 0 )
	{
		left = true;
	}
	if( XMLChild->Attribute("Focusable") != NULL && strcmp( XMLChild->Attribute("Focusable"), "True" ) == 0 )
	{
		right = true;
	}
	CCSize size = VirtualScreen::Instance()->VirtualSize();
	float dx = (DESIGN_WIDTH - size.width)/2;
	if(left)
	{
		offset = dx;
	}else if(right)
	{
		offset = -dx;
	}
	offset = 0;
	return offset;
}
void setWidthHeight(CCNode* target,float& width,float& height)
{
	do 
	{
		CC_BREAK_IF(!target);
		float w = target->boundingBox().size.width;
		float h = target->boundingBox().size.height;
		width = (width <=0)?w:width;
		height = (height <=0)?h:height;
		target->setScaleX(width/w);
		target->setScaleY(height/h);
	} while (0);
}
void ReadControlProperty( tinyxml2::XMLElement* XMLChild, std::string& name, float& left, float& top, int& tag, CCPoint& anchorPoint, float& opacity, bool& visibility,float& width,float& height)
{
	width = XMLChild->FloatAttribute("Width");
	height = XMLChild->FloatAttribute("Height");
	//默认为0
	left = XMLChild->FloatAttribute("Canvas.Left");	
	//默认为SCREEN_HEIGHT
	top = SCREEN_HEIGHT - XMLChild->FloatAttribute("Canvas.Top");

	tag = XMLChild->IntAttribute("Tag");			//必须大于0

	//锚点
	anchorPoint = ccp( 0.5f, 0.5f );
	const char* renderTransformOrigin = XMLChild->Attribute("RenderTransformOrigin");
	if( renderTransformOrigin != NULL )
	{
		anchorPoint.x = atof( renderTransformOrigin );
		anchorPoint.y = atof( strchr( renderTransformOrigin, ',' ) );
	}

	//透明度
	opacity = 1.0f;
	if( XMLChild->Attribute("Opacity") != NULL )
	{
		opacity = XMLChild->FloatAttribute("Opacity");
	}

	//控件名
	const char* str =XMLChild->Attribute("x:Name");
	if (str)
	{
		name = str;
	}

	//是否可视
	visibility = true;
	if( XMLChild->Attribute("d:IsHidden") != NULL && strcmp( XMLChild->Attribute("d:IsHidden"), "True" ) == 0 )
	{
		visibility = false;
	}
}

void ReadControlTransform( tinyxml2::XMLElement* XMLChild, float& ScaleX, float& ScaleY, float& SkewX, float& SkewY, float& RotateAngle, float& TranslateX, float& TranslateY)
{
	tinyxml2::XMLElement* subChild = XMLChild->FirstChildElement();
	if (subChild)
	{
		subChild = subChild->FirstChildElement();
		if (subChild)
		{
			subChild = subChild->FirstChildElement();
		}
	}
	while( subChild != NULL )
	{
		{
			const char* name = subChild->Name();
			if( strcmp( name, "ScaleTransform" ) == 0 )
			{
				if (subChild->Attribute("ScaleX"))
				{
					ScaleX =  subChild->FloatAttribute("ScaleX");
				}
				if (subChild->Attribute("ScaleY"))
				{
					ScaleY =  subChild->FloatAttribute("ScaleY");
				}
			}
			else if( strcmp( name, "SkewTransform" ) == 0 )
			{
				SkewX =  subChild->FloatAttribute("AngleX");
				SkewY =  subChild->FloatAttribute("AngleY");
			}
			else if( strcmp( name, "RotateTransform" ) == 0 )
			{
				RotateAngle = subChild->FloatAttribute("Angle");
			}
			else if( strcmp( name, "TranslateTransform" ) == 0 )
			{
				TranslateX = subChild->FloatAttribute("X");
				TranslateY = subChild->FloatAttribute("Y");
			}	
		}
		subChild = subChild->NextSiblingElement(); 
	}
	if (ScaleX==0||ScaleY==0)
	{
		ScaleX= ScaleY = 1;
	}
}

CImageView* CreateImage( const char* source, const char *name, float left, float top, int tag, CCPoint& anchorPoint, float opacity, bool visibility, float width,float height,float offset)
{
#ifdef CREATE_WITH_SPRITE_FRAME_NAME
	CImageView* image = CImageView::createWithSpriteFrameName( source );
#else 
	CImageView* image = CImageView::create( source );
#endif

#ifdef DISABLE_ANTIALIAS
	image->getTexture()->setAliasTexParameters();
#endif
	//float w = image->boundingBox().size.width;
	//float h = image->boundingBox().size.height;
	if( anchorPoint.x != 0.5f || anchorPoint.y != 0.5f ) image->setAnchorPoint( anchorPoint );
	if( tag > 0 ) image->setTag( tag );
	if( opacity != 1.0f ) image->setOpacity( opacity * 255 );
	if( visibility == false ) image->setVisible( false );
	//width = (width <=0)?w:width;
	//height = (height <=0)?h:height;
	//image->setScaleX(width/w);
	//image->setScaleY(height/h);
	
	setWidthHeight(image,width,height);
	if(strcmp(name,"")!=0)
	{
		image->setId(name);
	}
	image->setPosition( ccp( left + width*anchorPoint.x+offset, top - height*(1.0f-anchorPoint.y) ) );	//上下颠倒，所以锚点也要颠倒
	return image;
}
CImageView* CreateImageFromXML( tinyxml2::XMLElement* XMLChild )
{
	CCAssert( XMLChild->Attribute("Source") != NULL, "Source is NULL" );
	const char* source = XMLChild->Attribute("Source");

	float left; float top; int tag; CCPoint anchorPoint; float opacity; bool visibility;float width;float height;
	std::string name;
	ReadControlProperty( XMLChild, name, left, top, tag, anchorPoint, opacity, visibility,width,height);

	float offset = getOffsetX(XMLChild);
	return CreateImage( source, name.c_str(), left, top, tag, anchorPoint, opacity, visibility,width,height,offset);
}

CLabelBMFont* CreateText( const char* string, const char* fontFamily, float fontSize, const char *name, float left, float top, int tag, CCPoint& anchorPoint, float opacity, bool visibility,float width,float height
						 ,float offset)
{
	//CCLabelTTF* label = CCLabelTTF::create( string, fontFamily, fontSize );	//TTF版本
	CLabelBMFont* label = CLabelBMFont::create( string, BMFontDefault );
	//36pt	-> 49 px
	//24pt	-> 33 px
	//22pt	-> 31 px
	//20pt	-> 27 px
	//18pt  -> 24 px
	//16pt  -> 21 px
	//14pt  -> 19 px
	//12pt	-> 16 px
	//11pt	-> 15 px
	//10pt	-> 14 px
	//9pt	-> 12 px
	if( fontSize > 36 ) label->setScale( 49.0f/24.0f );
	else if( fontSize > 24 ) label->setScale( 33.0f/24.0f );
	else if( fontSize > 22 ) label->setScale( 31.0f/24.0f );
	else if( fontSize > 20 ) label->setScale( 27.0f/24.0f );
	else if( fontSize > 18 ) label->setScale( 24.0f/24.0f );
	else if( fontSize > 16 ) label->setScale( 21.0f/24.0f );
	else if( fontSize > 14 ) label->setScale( 19.0f/24.0f );
	else if( fontSize > 12 ) label->setScale( 16.0f/24.0f );
	else if( fontSize > 11 ) label->setScale( 15.0f/24.0f );
	else if( fontSize > 10 ) label->setScale( 14.0f/24.0f );
	else if( fontSize >  9 ) label->setScale( 12.0f/24.0f );
#ifdef DISABLE_ANTIALIAS
	label->getTexture()->setAliasTexParameters();
#else
	label->getTexture()->setAntiAliasTexParameters();
#endif
	//float width = label->boundingBox().size.width;
	//float height = label->boundingBox().size.height;
	if( anchorPoint.x != 0.5f || anchorPoint.y != 0.5f ) label->setAnchorPoint( anchorPoint );
	if( tag > 0 ) label->setTag( tag );
	if( opacity != 1.0f ) label->setOpacity( opacity * 255 );
	if( visibility == false ) label->setVisible( false );
	//label->setPosition( ccp( left + width*anchorPoint.x, top - height*(1.0f-anchorPoint.y) ) );	//上下颠倒，所以锚点也要颠倒
	anchorPoint.x = 0;
	if(strcmp(name,"")!=0)
	{
		label->setId(name);
	}
	label->setAnchorPoint(anchorPoint);
	label->setPosition(left+offset,top - height*(1.0f-anchorPoint.y));
	return label;
}
CLabelBMFont* CreateTextFromXML( tinyxml2::XMLElement* XMLChild )
{
	const char* Text = (char*)XMLChild->Attribute("Text");
	if( Text == NULL ) Text = "";

	const char* fontFamily = XMLChild->Attribute("FontFamily");
	if( fontFamily == NULL ) fontFamily = DEFAULT_FONT;

	float fontSize = XMLChild->FloatAttribute("FontSize");
	if( fontSize == 0 ) fontSize = 12;

	float left; float top; int tag; CCPoint anchorPoint; float opacity; bool visibility;float width;float height;
	std::string name;
	ReadControlProperty( XMLChild, name,left, top, tag, anchorPoint, opacity, visibility,width,height);
	float offset = getOffsetX(XMLChild);
	return CreateText( Text, fontFamily, fontSize, name.c_str(), left, top, tag, anchorPoint, opacity, visibility,width,height,offset);
}

CLabel* CreateLabel( const char* string, const char* fontFamily, float fontSize, const char *name,float left, float top, int tag, CCPoint& anchorPoint, float opacity, bool visibility,float width,float height
						 ,bool isCenter,bool isSetDimension, float offset,int langId)
{
	CLabel* label = CLabel::create( string, fontFamily, fontSize );	//TTF版本
#ifdef DISABLE_ANTIALIAS
	label->getTexture()->setAliasTexParameters();
#else
	label->getTexture()->setAntiAliasTexParameters();
#endif
	if( anchorPoint.x != 0.5f || anchorPoint.y != 0.5f ) label->setAnchorPoint( anchorPoint );
	if( tag > 0 ) label->setTag( tag );
	if( opacity != 1.0f ) label->setOpacity( opacity * 255 );
	if( visibility == false ) label->setVisible( false );
	
	anchorPoint.x = 0;
	if (isCenter)
	{
		anchorPoint.x = 0.5;
	}
	if(strcmp(name,"")!=0)
	{
		label->setId(name);
	}
	label->setAnchorPoint(anchorPoint);
	width = label->boundingBox().size.width;
	if (isSetDimension&&!isCenter)
	{
		label->setDimensions(CCSizeMake(width,0));
	}

	if (langId>0)
	{
		label->setString(GETLANGSTR(langId));
	}

	label->setPosition(ccp(left+offset+ anchorPoint.x*width,top - height*(1.0f-anchorPoint.y)));
	return label;
}
CLabel* CreateLabelFromXML( tinyxml2::XMLElement* XMLChild )
{
	const char* Text = (char*)XMLChild->Attribute("Content");
	if( Text == NULL ) Text = "";

	const char* fontFamily = XMLChild->Attribute("FontFamily");
	if( fontFamily == NULL ) fontFamily = DEFAULT_FONT;
	else fontFamily = DEFAULT_FONT1;

	float fontSize = XMLChild->FloatAttribute("FontSize");
	if( fontSize == 0 ) fontSize = 12;

	float left; float top; int tag; CCPoint anchorPoint; float opacity; bool visibility;float width;float height;
	std::string name;
	ReadControlProperty( XMLChild, name,left, top, tag, anchorPoint, opacity, visibility,width,height);
	float offset = getOffsetX(XMLChild);

	const char* strCenter = XMLChild->Attribute("IsTabStop");
	bool isCenter = false;
	if (strCenter&&strcmp(strCenter,"True")==0)
	{
		isCenter =true;
	}

	const char* strDimensions = XMLChild->Attribute("ClipToBounds");
	bool isSetDimension = false;
	if (strDimensions&&strcmp(strDimensions,"True")==0)
	{
		isSetDimension =true;
	}

	const char* langId = XMLChild->Attribute("TabIndex");
	int lang = 0;
	if (langId&&strcmp(langId,"")!=0)
	{
		lang = atoi(langId);
	}
	CLabel *label = CreateLabel( Text, fontFamily, fontSize, name.c_str(), left, top, tag, anchorPoint, opacity, visibility,width,height,isCenter,isSetDimension,offset,lang);
	return label;
}

CCNode* CreateNodeFromXML( tinyxml2::XMLElement* XMLChild )
{
	float left; float top; int tag; CCPoint anchorPoint; float opacity; bool visibility;float width;float height;
	std::string name;
	ReadControlProperty( XMLChild, name,left, top, tag, anchorPoint, opacity, visibility,width,height);
	
	CCNode *node = CCNode::create();
	if( anchorPoint.x != 0.5f || anchorPoint.y != 0.5f ) node->setAnchorPoint( anchorPoint );
	if( tag > 0 ) node->setTag( tag );
	if( visibility == false ) node->setVisible( false );
	node->setPosition( ccp( left + width*anchorPoint.x, top - height*(1.0f-anchorPoint.y) ) );	
	return node;
}

CButton* CreateButton( const char* content, const char* fontFamily, float fontSize, const char *name,float width, float height, float left, float top,	int tag, CCPoint& anchorPoint, float opacity, bool visibility )
{
	CButton* button = CButton::create();
	button->getLabel()->initWithString( content, fontFamily, fontSize );
	bool autoSize = ( (width == 0) || (height == 0) );
	if( autoSize == true )
	{
		width = button->getLabel()->boundingBox().size.width+8+8;
		height = button->getLabel()->boundingBox().size.height+2+2;
	}
	if( ButtonDefaultBackgound != NULL )
	{
		button->setContentSize( CCSizeMake(width,height) );
		button->setScale9Enabled(true);
		button->setNormalImage(ButtonDefaultBackgound);
	}
	if( ButtonClickBackgound != NULL )
	{
		button->setScale9Enabled(true);
		button->setSelectedImage(ButtonClickBackgound);
	}
	if(name) button->setId(name);
	if( anchorPoint.x != 0.5f || anchorPoint.y != 0.5f ) button->setAnchorPoint( anchorPoint );
	if( tag > 0 ) button->setTag( tag );
	if( opacity != 1.0f ) button->setOpacity( opacity * 255 );
	if( visibility == false ) button->setVisible( false );
	button->setPosition( ccp( left + width*anchorPoint.x, top - height*(1.0f-anchorPoint.y) ) );	//上下颠倒，所以锚点也要颠倒
	return button;
}
CButton* CreateButtonFromXML( tinyxml2::XMLElement* XMLChild )
{
	const char* content = XMLChild->Attribute("Content");
	if( content == NULL ) content = "";

	const char* fontFamily = XMLChild->Attribute("FontFamily");
	if( fontFamily == NULL ) fontFamily = DEFAULT_FONT;

	float fontSize = XMLChild->FloatAttribute("FontSize");
	if( fontSize == 0 )	fontSize = 12;

	//float width = XMLChild->FloatAttribute("Width");
	//float height = XMLChild->FloatAttribute("Height");

	float left; float top; int tag; CCPoint anchorPoint; float opacity; bool visibility;float width;float height;
	std::string name;
	ReadControlProperty( XMLChild, name,left, top, tag, anchorPoint, opacity, visibility,width,height);
	return CreateButton( content, fontFamily, fontSize, name.c_str(), width, height, left, top, tag, anchorPoint, opacity, visibility );
}
CButton* CreateCCButton( const char* text, const char* fontFamily, float fontSize, const char *name, bool isPixelEnable,char* normalImage, char* clickedImage, char* disableImage, float left, float top,	int tag, CCPoint& anchorPoint, float opacity, bool visibility,float width,float height,float offsetX,bool isSacle9Enable)
{
	//CButton* button = CButton::create();

	CButton* button = nullptr;
	if (isPixelEnable)
	{
		button =  CPixelButton::create();
		((CPixelButton*)button)->setImage(normalImage);
	}
	else
	{
		button = SndButton::create();
	}
	if (isSacle9Enable)
	{
// 		button->setContentSize(CCSizeMake(width,height));
		button = SndButton::createWith9Sprite(CCSizeMake(width,height),normalImage,clickedImage,disableImage);
		button->setScale9Enabled(isSacle9Enable);
	}
	else
	{
		button = SndButton::create();
		button->setScale9Enabled(isSacle9Enable);
		if( text != NULL )
				button->getLabel()->initWithString(text, fontFamily, fontSize);
	
			if( normalImage != NULL )
			{
		#ifdef CREATE_WITH_SPRITE_FRAME_NAME
				button->setNormalSpriteFrame( CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(normalImage) );
		#else
				button->setNormalImage(normalImage);
		#endif
			}
	
			if( clickedImage != NULL )
			{
		#ifdef CREATE_WITH_SPRITE_FRAME_NAME
				button->setSelectedSpriteFrame( CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(clickedImage) );
		#else
				button->setSelectedImage(clickedImage);
		#endif
			}
	
			if( disableImage != NULL )
			{
		#ifdef CREATE_WITH_SPRITE_FRAME_NAME
				button->setDisabledSpriteFrame( CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(disableImage) );
		#else
				button->setDisabledImage(disableImage);
		#endif
			}
	}

	//float width = button->boundingBox().size.width;
	//float height = button->boundingBox().size.height;
	if(strcmp(name,"")!=0)
	{
		button->setId(name);
	}
	if( anchorPoint.x != 0.5f || anchorPoint.y != 0.5f ) button->setAnchorPoint( anchorPoint );
	if( tag > 0 ) button->setTag( tag );
	if( opacity != 1.0f ) button->setOpacity( opacity * 255 );
	if( visibility == false ) button->setVisible( false );
	setWidthHeight(button,width,height);

	button->setPosition( ccp( left + width*anchorPoint.x + offsetX, top - height*(1.0f-anchorPoint.y) ) );	//上下颠倒，所以锚点也要颠倒
	return button;
}
CButton* CreateCCButtonFromXML( tinyxml2::XMLElement* XMLChild )
{
	char* text = (char*)XMLChild->Attribute("Text");	

	const char* fontFamily = XMLChild->Attribute("FontFamily");
	if( fontFamily == NULL ) fontFamily = DEFAULT_FONT;

	float fontSize = XMLChild->FloatAttribute("FontSize");
	if( fontSize == 0 )	fontSize = 16;

	char* normalImage = (char*)XMLChild->Attribute("NormalImage");

	char* clickedImage = (char*)XMLChild->Attribute("ClickedImage");	

	char* disableImage = (char*)XMLChild->Attribute("DisableImage");

	char *pixelEnable= (char*)XMLChild->Attribute("IsEnabled");

	bool isPixelEnale = false;
	if (pixelEnable&&strcmp(pixelEnable,"False")==0)
	{
		isPixelEnale = true;
	}

	char *isScaleEnable= (char*)XMLChild->Attribute("IsScaleEnable");

	bool isScale9Enable = false;
	if (isScaleEnable&&strcmp(isScaleEnable,"True")==0)
	{
		isScale9Enable = true;
	}

	float left; float top; int tag; CCPoint anchorPoint; float opacity; bool visibility;float width =0;float height =0;
	std::string name;
	ReadControlProperty( XMLChild, name,left, top, tag, anchorPoint, opacity, visibility,width,height);
	float offsetX = getOffsetX(XMLChild);
	return CreateCCButton( text, fontFamily, fontSize, name.c_str(), isPixelEnale, normalImage, clickedImage, disableImage, left, top, tag, anchorPoint, opacity, visibility,width,height,offsetX,isScale9Enable);
}

CLabelAtlas* CreateAtlasLabel( const char* string, const char* charMapFile, unsigned int itemWidth, unsigned int itemHeight, unsigned int startCharMap, float left, float top, const char *name, int tag, CCPoint& anchorPoint, float opacity, bool visibility )
{
	CLabelAtlas* label = CLabelAtlas::create( string, charMapFile, itemWidth, itemHeight, startCharMap );

	float width = label->boundingBox().size.width;
	float height = label->boundingBox().size.height;
	//float width = strlen(string) * itemWidth;
	//float height = itemHeight;

	//if( anchorPoint.x != 0.5f || anchorPoint.y != 0.5f )	//CCLabelAtlas的锚点默认不是中间
	if(strcmp(name,"")!=0)
	{
		label->setId(name);
	}
	label->setAnchorPoint( anchorPoint );
	if( tag > 0 ) label->setTag( tag );
	if( opacity != 1.0f ) label->setOpacity( opacity * 255 );
	if( visibility == false ) label->setVisible( false );
	label->setPosition( ccp( left + width*anchorPoint.x, top - height*(1.0f-anchorPoint.y) ) );	//上下颠倒，所以锚点也要颠倒

	return label;
}
CLabelAtlas* CreateAtlasLabelFromXML( tinyxml2::XMLElement* XMLChild )
{
	//必选
	const char* Text = XMLChild->Attribute("Text");
	if( Text == NULL ) Text = "";

	CCAssert( XMLChild->Attribute("CharMapFile") != NULL, "CharMapFile is NULL" );
	const char* charMapFile = XMLChild->Attribute("CharMapFile");

	CCAssert( XMLChild->Attribute("CharWidth") != NULL, "CharWidth is NULL" );
	int itemWidth = XMLChild->IntAttribute("CharWidth");

	CCAssert( XMLChild->Attribute("CharHeight") != NULL, "CharHeight is NULL" );
	int itemHeight = XMLChild->IntAttribute("CharHeight");

	CCAssert( XMLChild->Attribute("StartCharMap") != NULL, "StartCharMap is NULL" );
	const char* startCharMap = XMLChild->Attribute("StartCharMap");
	std::string name;
	float left; float top; int tag; CCPoint anchorPoint; float opacity; bool visibility;float width;float height;
	ReadControlProperty( XMLChild,name, left, top, tag, anchorPoint, opacity, visibility,width,height);
	return CreateAtlasLabel( Text, charMapFile, itemWidth, itemHeight, startCharMap[0], left, top, name.c_str(), tag, anchorPoint, opacity, visibility );
}

CToggleView* CreateToggleButton( const char* content, const char* fontFamily, float fontSize, const char *name,char* checkedImage, char* uncheckedImage,char* disableImage, float left, float top, int tag, CCPoint& anchorPoint, float opacity, bool visibility )
{
	CToggleView* button = CToggleView::create();
	if( content != NULL )
		button->getLabel()->initWithString( content, fontFamily, fontSize );

	if( uncheckedImage != NULL )
	{
#ifdef CREATE_WITH_SPRITE_FRAME_NAME
		button->setNormalSpriteFrame( CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(uncheckedImage) );
#else
		button->setNormalImage(uncheckedImage);
#endif
	}

	if( checkedImage != NULL )
	{
#ifdef CREATE_WITH_SPRITE_FRAME_NAME
		button->setNormalSpriteFrame( CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(checkedImage) );
#else
		button->setSelectedImage( checkedImage );
#endif
	}

	if (disableImage)
	{
		button->setDisabledImage(disableImage);
	}

	float width = button->boundingBox().size.width;
	float height = button->boundingBox().size.height;
	if(strcmp(name,"")!=0)
	{
		button->setId(name);
	}
	if( anchorPoint.x != 0.5f || anchorPoint.y != 0.5f ) button->setAnchorPoint( anchorPoint );
	if( tag > 0 ) button->setTag( tag );
	if( opacity != 1.0f ) button->setOpacity( opacity * 255 );
	if( visibility == false ) button->setVisible( false );
	button->setPosition( ccp( left + width*anchorPoint.x, top - height*(1.0f-anchorPoint.y) ) );	//上下颠倒，所以锚点也要颠倒
	return button;
}
CToggleView* CreateToggleButtonFromXML( tinyxml2::XMLElement* XMLChild )
{
	char* content = (char*)XMLChild->Attribute("Content");	

	const char* fontFamily = XMLChild->Attribute("FontFamily");
	if( fontFamily == NULL ) fontFamily = DEFAULT_FONT;

	float fontSize = XMLChild->FloatAttribute("FontSize");
	if( fontSize == 0 )	fontSize = 12;

	char* checkedImage = (char*)XMLChild->Attribute("CheckedImage");

	char* uncheckedImage = (char*)XMLChild->Attribute("UncheckedImage");	

	char* disableImage = (char*)XMLChild->Attribute("DisableImage");

	std::string name;
	float left; float top; int tag; CCPoint anchorPoint; float opacity; bool visibility;float width;float height;
	ReadControlProperty( XMLChild, name,left, top, tag, anchorPoint, opacity, visibility,width,height);
	return CreateToggleButton( content, fontFamily, fontSize, name.c_str(), checkedImage, uncheckedImage, disableImage, left, top, tag, anchorPoint, opacity, visibility );
}

CCheckBox* CreateCheckBox( char* normalImage, char* selectImage, char* pressImage, char* disableImage, const char *name, float left, float top, int tag, CCPoint& anchorPoint, float opacity, bool visibility, bool isCheck)
{
	CCheckBox* button = CCheckBox::create();

	if( normalImage != NULL )
	{
#ifdef CREATE_WITH_SPRITE_FRAME_NAME
		button->setNormalSpriteFrame( CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(normalImage) );
#else
		button->setNormalImage(normalImage);
#endif
	}

	if( selectImage != NULL )
	{
#ifdef CREATE_WITH_SPRITE_FRAME_NAME
		button->setCheckedPressSpriteFrame( CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(selectImage) );
#else
		button->setCheckedImage(selectImage);
#endif
	}

	if (pressImage)
	{
		button->setNormalPressImage(pressImage);
	}

	if (disableImage)
	{
		button->setDisabledNormalImage(disableImage);
	}

	button->setChecked(isCheck);
	float width = button->boundingBox().size.width;
	float height = button->boundingBox().size.height;
	if(strcmp(name,"")!=0)
	{
		button->setId(name);
	}
	if( anchorPoint.x != 0.5f || anchorPoint.y != 0.5f ) button->setAnchorPoint( anchorPoint );
	if( tag > 0 ) button->setTag( tag );
	if( opacity != 1.0f ) button->setOpacity( opacity * 255 );
	if( visibility == false ) button->setVisible( false );
	button->setPosition( ccp( left + width*anchorPoint.x, top - height*(1.0f-anchorPoint.y) ) );	//上下颠倒，所以锚点也要颠倒
	return button;
}
CCheckBox* CreateCheckBoxFromXML( tinyxml2::XMLElement* XMLChild )
{

	char* checkedImage = (char*)XMLChild->Attribute("SelectImage");

	char* normalImage = (char*)XMLChild->Attribute("NormalImage");	

	char* pressImage = (char*)XMLChild->Attribute("PressImage");

	char* disableImage = (char*)XMLChild->Attribute("DisableImage");

	bool isCheck = false;
	if( XMLChild->Attribute("IsCheck") != NULL && strcmp( XMLChild->Attribute("IsCheck"), "False" ) != 0 )
	{
		isCheck = true;
	}

	float left; float top; int tag; CCPoint anchorPoint; float opacity; bool visibility;float width;float height;
	std::string name;
	ReadControlProperty( XMLChild,name, left, top, tag, anchorPoint, opacity, visibility,width,height);
	return CreateCheckBox(normalImage,checkedImage, pressImage, disableImage,name.c_str(), left, top, tag, anchorPoint, opacity, visibility,isCheck);
}

CRadioButton* CreateRadioButton( char* normalImage, char* selectImage, char* pressImage, char* disableImage, const char *name, float left, float top, int tag, CCPoint& anchorPoint, float opacity, bool visibility, bool isCheck)
{
	CRadioButton* button = CRadioButton::create();

	if( normalImage != NULL )
	{
#ifdef CREATE_WITH_SPRITE_FRAME_NAME
		button->setNormalSpriteFrame( CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(normalImage) );
#else
		button->setNormalImage(normalImage);
#endif
	}

	if( selectImage != NULL )
	{
#ifdef CREATE_WITH_SPRITE_FRAME_NAME
		button->setCheckedPressSpriteFrame( CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(selectImage) );
#else
		button->setCheckedImage(selectImage);
#endif
	}

	if (pressImage)
	{
		button->setNormalPressImage(pressImage);
	}

	if (disableImage)
	{
		button->setDisabledNormalImage(disableImage);
	}

	button->setChecked(isCheck);
	float width = button->boundingBox().size.width;
	float height = button->boundingBox().size.height;

	if(strcmp(name,"")!=0)
	{
		button->setId(name);
	}
	if( anchorPoint.x != 0.5f || anchorPoint.y != 0.5f ) button->setAnchorPoint( anchorPoint );
	if( tag > 0 ) button->setTag( tag );
	if( opacity != 1.0f ) button->setOpacity( opacity * 255 );
	if( visibility == false ) button->setVisible( false );
	button->setPosition( ccp( left + width*anchorPoint.x, top - height*(1.0f-anchorPoint.y) ) );	//上下颠倒，所以锚点也要颠倒
	return button;
}
CRadioButton* CreateRadioButtonFromXML( tinyxml2::XMLElement* XMLChild )
{

	char* checkedImage = (char*)XMLChild->Attribute("SelectImage");

	char* normalImage = (char*)XMLChild->Attribute("NormalImage");	

	char* pressImage = (char*)XMLChild->Attribute("PressImage");

	char* disableImage = (char*)XMLChild->Attribute("DisableImage");

	bool isCheck = false;
	if(XMLChild->Attribute("IsCheck")&&strcmp( XMLChild->Attribute("IsCheck"), "False" ) != 0 )
	{
		isCheck = true;
	}

	float left; float top; int tag; CCPoint anchorPoint; float opacity; bool visibility;float width;float height;
	std::string name;
	ReadControlProperty( XMLChild, name,left, top, tag, anchorPoint, opacity, visibility,width,height);
	return CreateRadioButton(normalImage,checkedImage, pressImage, disableImage, name.c_str(), left, top, tag, anchorPoint, opacity, visibility,isCheck);
}

CImageViewScale9* CreateImageScale9( const char* source, const char *name, float left, float top, int tag, CCPoint& anchorPoint, float opacity, bool visibility, float width,float height,float offset)
{
	CImageViewScale9 *imageScale9 = CImageViewScale9::create(source);
	imageScale9->setContentSize(CCSizeMake(width,height));
// 	imageScale9->setCapInsets(CCRectMake(0.1*width,0.1*height,0.8*width,0.8*height));
	if( anchorPoint.x != 0.5f || anchorPoint.y != 0.5f ) imageScale9->setAnchorPoint( anchorPoint );
	if( tag > 0 ) imageScale9->setTag( tag );
	if( opacity != 1.0f ) imageScale9->setOpacity( opacity * 255 );
	if( visibility == false ) imageScale9->setVisible( false );
	//setWidthHeight(imageScale9,width,height);
	if(strcmp(name,"")!=0)
	{
		imageScale9->setId(name);
	}
	imageScale9->setPosition( ccp( left + width*anchorPoint.x+offset, top - height*(1.0f-anchorPoint.y) ) );
	return imageScale9;
}

CImageViewScale9* CreateImageScale9FromXML( tinyxml2::XMLElement* XMLChild )
{
	CCAssert( XMLChild->Attribute("Source") != NULL, "Source is NULL" );
	const char* source = XMLChild->Attribute("Source");

	float left; float top; int tag; CCPoint anchorPoint; float opacity; bool visibility;float width;float height;
	std::string name;
	ReadControlProperty( XMLChild, name, left, top, tag, anchorPoint, opacity, visibility,width,height);

// 	float scaleX = 0; float scaleY = 0; float skewX = 0; float skewY = 0; float rotateAngle = 0; float translateX = 0; float translateY = 0;
// 	ReadControlTransform( XMLChild, scaleX, scaleY, skewX, skewY, rotateAngle, translateX, translateY );

	float offset = getOffsetX(XMLChild);
	return CreateImageScale9( source, name.c_str(), left, top, tag, anchorPoint, opacity, visibility,width,height,offset);
}

CSlider* CreateSlider( const char* backgroundImage, const char* forgroundImage, const char* thumbImage, const char *name, float maxValue, float minValue, float value, float left, float top, int tag, CCPoint& anchorPoint, float opacity, bool visibility )
{
	if( forgroundImage == NULL )
		forgroundImage = backgroundImage;
	CSlider* slider = CSlider::create( thumbImage, forgroundImage );
	if (backgroundImage)
	{
		slider->setBackgroundImage(backgroundImage);
	}
	slider->setMaxValue(maxValue);
	slider->setMinValue(minValue);
	slider->setDirection(eProgressBarDirectionLeftToRight);
	slider->setValue(value);
	float width = slider->boundingBox().size.width;
	float height = slider->boundingBox().size.height;
	if( anchorPoint.x != 0.5f || anchorPoint.y != 0.5f ) slider->setAnchorPoint( anchorPoint );
	if( tag > 0 ) slider->setTag( tag );
	if( opacity != 1.0f ) slider->setOpacity( opacity * 255 );
	if( visibility == false ) slider->setVisible( false );
	if(strcmp(name,"")!=0)
	{
		slider->setId(name);
	}
	slider->setPosition( ccp( left + width*anchorPoint.x, top - height*(1.0f-anchorPoint.y) ) );	//上下颠倒，所以锚点也要颠倒
	slider->changeValueAndExecuteEvent(1,true);
	return slider;
}
CSlider* CreateSliderFromXML( tinyxml2::XMLElement* XMLChild )
{
	//CCAssert( XMLChild->Attribute("BackgroundImage") != NULL, "BackgroundImage is NULL" );
	char* backgroundImage = (char*)XMLChild->Attribute("BackgroundImage");	

	char* forgroundImage = (char*)XMLChild->Attribute("ForgroundImage");

	char* thumbImage = (char*)XMLChild->Attribute("ThumbImage");

	float maxValue = XMLChild->FloatAttribute("MaxValue");
	if( maxValue == 0 ) maxValue = 100;

	float minValue = XMLChild->FloatAttribute("MinValue");

	float value = XMLChild->FloatAttribute("Value");

	float left; float top; int tag; CCPoint anchorPoint; float opacity; bool visibility;float width;float height;
	std::string name;
	ReadControlProperty( XMLChild, name,left, top, tag, anchorPoint, opacity, visibility,width,height);
	return CreateSlider( backgroundImage, forgroundImage, thumbImage, name.c_str(), maxValue, minValue, value, left, top, tag, anchorPoint, opacity, visibility );
}

CProgressBar* CreateProgress(const char* backgroundImage,const char* forgroundImage, const char *name,int type, float minValue, float maxValue, float value, float left, float top, int tag, CCPoint& anchorPoint, float opacity, bool visibility )
{
	CProgressBar *progress = CProgressBar::create(forgroundImage);
	if (backgroundImage)
	{
		progress->setBackgroundImage(backgroundImage);
	}
	
	progress->setDirection(CProgressBarDirection(type));
	progress->setMinValue(minValue);
	progress->setMaxValue(maxValue);
	progress->setValue(value);

	if(strcmp(name,"")!=0)
	{
		progress->setId(name);
	}
	float width = progress->boundingBox().size.width;
	float height = progress->boundingBox().size.height;
	if( anchorPoint.x != 0.5f || anchorPoint.y != 0.5f ) progress->setAnchorPoint( anchorPoint );
	if( tag > 0 ) progress->setTag( tag );
	if( opacity != 1.0f ) progress->setOpacity(opacity * 255 );
	if( visibility == false ) progress->setVisible( false);
	progress->setPosition( ccp( left + width*anchorPoint.x, top - height*(1.0f-anchorPoint.y) ) );	//上下颠倒，所以锚点也要颠倒
	return progress;
}
CProgressBar* CreateProgressFromXML( tinyxml2::XMLElement* XMLChild )
{
	//CCAssert( XMLChild->Attribute("BackgroundImage") != NULL, "BackgroundImage is NULL" );
	char* backgroundImage = (char*)XMLChild->Attribute("BackgroundImage");	

	CCAssert( XMLChild->Attribute("ForgroundImage") != NULL, "ForgroundImage is NULL" );
	char* forgroundImage = (char*)XMLChild->Attribute("ForgroundImage");

	float maxValue = XMLChild->FloatAttribute("MaxValue");
	if( maxValue == 0 ) maxValue = 100;

	float minValue = XMLChild->FloatAttribute("MinValue");

	float value = XMLChild->FloatAttribute("Value");

	int  type = XMLChild->IntAttribute("Type");
	if (type>3)
	{
		type = 0;
	}
	float left; float top; int tag; CCPoint anchorPoint; float opacity; bool visibility;float width;float height;
	std::string name;
	ReadControlProperty( XMLChild, name,left, top, tag, anchorPoint, opacity, visibility,width,height);
	return CreateProgress(backgroundImage,forgroundImage,name.c_str(),type, minValue,maxValue,value, left, top, tag, anchorPoint, opacity, visibility );
}

CScrollView* CreateScrollView(int type,int sizeWidth, int sizeHeight,  const char *name, float left, float top, int tag, CCPoint& anchorPoint, float opacity, bool visibility,float width, float height)
{
	CScrollView *scrollView = nullptr;
	if (type==1)
	{
		scrollView = CScrollView::create(CCSizeMake(sizeWidth,sizeHeight));
	}
	else if (type==2)
	{
		scrollView = CListView::create(CCSizeMake(sizeWidth,sizeHeight));
	}
	else if (type==3)
	{
		scrollView = CTableView::create(CCSizeMake(sizeWidth,sizeHeight));
	}
	else if (type==4)
	{
		scrollView = CGridView::create(CCSizeMake(sizeWidth,sizeHeight));
	}
	else if (type==5)
	{
		scrollView = CPageView::create(CCSizeMake(sizeWidth,sizeHeight));
	}
	else if (type==6)
	{
		scrollView = CGridPageView::create(CCSizeMake(sizeWidth,sizeHeight));
	}

	scrollView->setContainerSize(CCSizeMake(sizeWidth,sizeHeight));
	scrollView->setDirection(eScrollViewDirectionVertical);
	if(strcmp(name,"")!=0)
	{
		scrollView->setId(name);
	}
	if( tag > 0 ) scrollView->setTag( tag );
	if( opacity != 1.0f ) scrollView->setOpacity( opacity * 255 );
	if( visibility == false ) scrollView->setVisible( false );
	scrollView->setAnchorPoint(ccp(0,0));
	scrollView->setPosition( ccp( left, top - height) );	//上下颠倒，所以锚点也要颠倒
	return scrollView;
}
CScrollView* CreateScrollFromXML( tinyxml2::XMLElement* XMLChild )
{
	float sizeWidth = XMLChild->FloatAttribute("SizeWidth");
	if( sizeWidth == 0 ) sizeWidth = 100;

	float sizeHeight = XMLChild->FloatAttribute("SizeHeight");
	if (sizeHeight ==0)
	{
		sizeHeight = 100;
	}
	int  type = XMLChild->IntAttribute("Type");
	if (type==0)
	{
		type = 1;
	}
	float left; float top; int tag; CCPoint anchorPoint; float opacity; bool visibility;float width;float height;
	std::string name;
	ReadControlProperty( XMLChild, name,left, top, tag, anchorPoint, opacity, visibility,width,height);
	return CreateScrollView(type, sizeWidth,sizeHeight, name.c_str(), left, top, tag, anchorPoint, opacity, visibility,width,height);
}

CRadioBtnGroup *createRadioBtnGroupFromXml(tinyxml2::XMLElement* XMLChild)
{
	float left; float top; int tag; CCPoint anchorPoint; float opacity; bool visibility;float width;float height;
	std::string name;
	ReadControlProperty( XMLChild,name, left, top, tag, anchorPoint, opacity, visibility,width,height);
	CRadioBtnGroup *group = CRadioBtnGroup::create();
	group->setContentSize(CCSizeMake(width,height));
	
	group->setTag(tag);
	if( anchorPoint.x != 0.5f || anchorPoint.y != 0.5f ) group->setAnchorPoint( anchorPoint );
	group->setAnchorPoint(ccp(0,0));
	group->setPosition(ccp( left, top - height));

	if(name!="")
	{
		group->setId(name.c_str());
	}

	const char* drop = XMLChild->Attribute("AllowDrop");
	bool isHorizontal = false;
	if (drop&&strcmp(drop,"True")==0)
	{
		isHorizontal = true;
	}

	tinyxml2::XMLElement* subChild = XMLChild->FirstChildElement();

	while( subChild != NULL )
	{
		const char* name = subChild->Name();
		if( strcmp( name, "UI:RadioButton" ) == 0 )
		{
			CRadioButton *btn = CreateRadioButtonFromXML(subChild);
			CCArray *arr = strSplit(subChild->Attribute("Margin"),",");float scaleX = 1.0f; float scaleY = 1.0f; float skewX = 0; float skewY = 0; float rotateAngle = 0; float translateX = 0; float translateY = 0;
			ReadControlTransform( subChild, scaleX, scaleY, skewX, skewY, rotateAngle, translateX, translateY );

			btn->setScaleX(scaleX);
			btn->setScaleY(scaleY);
			if (rotateAngle!=0)
			{
				btn->setRotation(rotateAngle);
			}

			if (translateX!=0||translateY!=0)
			{
				btn->setPositionX(btn->getPositionX()+translateX);
				btn->setPositionY(btn->getPositionY()-translateY);
			}
			float fLeft = ((CCString*)arr->objectAtIndex(0))->floatValue();
			float fTop = ((CCString*)arr->objectAtIndex(1))->floatValue();

			if (isHorizontal)
			{	
// 				btn->setPosition( ccp(fLeft + btn->boundingBox().size.width*anchorPoint.x, height - (fTop + btn->boundingBox().size.height /** 0.5*/) ));
				btn->setPosition( ccp(fLeft + btn->boundingBox().size.width*anchorPoint.x, (height - fTop /*- btn->boundingBox().size.height*/ ) * 0.5 ));
			}
			else
			{
				btn->setPosition( ccp(fLeft + width*anchorPoint.x, height - (fTop + btn->boundingBox().size.height *0.5)));
			}
			group->addButton(btn,btn->getTag());
		}
		subChild = subChild->NextSiblingElement();
	}
	return group;
}

void ReadXmlNode( tinyxml2::XMLElement* XMLCanvas, CLayout* MainLayer )
{
	tinyxml2::XMLElement* XMLChild = XMLCanvas->FirstChildElement();
	CCNode *pWidget = nullptr;
	while( XMLChild != NULL )
	{
		//必选
		const char* name = XMLChild->Name();
		if( strcmp( name, "Image" ) == 0 )
		{
			pWidget = CreateImageFromXML( XMLChild );
		}
		else if( strcmp( name, "TextBlock" ) == 0 )
		{
			pWidget = CreateTextFromXML( XMLChild );
			if( XMLChild->Attribute("Foreground") != NULL )	((CLabelBMFont*)pWidget)->setColor( GetColorFromXML( XMLChild->Attribute("Foreground") ) );
		}
		else if( strcmp( name, "Button" ) == 0 )
		{
			pWidget = CreateButtonFromXML( XMLChild );
			if( XMLChild->Attribute("Foreground") != NULL )	((CButton*)pWidget)->getLabel()->setColor( GetColorFromXML( XMLChild->Attribute("Foreground") ) );
		}
		else if( strcmp( name, "UI:CCButton" ) == 0 )
		{
			pWidget = CreateCCButtonFromXML( XMLChild );
			if( XMLChild->Attribute("Foreground") != NULL )	((CButton*)pWidget)->getLabel()->setColor( GetColorFromXML( XMLChild->Attribute("Foreground") ) );
		}
		else if( strcmp( name, "UI:AtlasLabel" ) == 0 )
		{
			pWidget = CreateAtlasLabelFromXML( XMLChild );
		}
		else if( strcmp( name, "UI:ToggleButton") == 0 )
		{
			pWidget = CreateToggleButtonFromXML( XMLChild );
			if( XMLChild->Attribute("Foreground") != NULL )	((CToggleView*)pWidget)->getLabel()->setColor( GetColorFromXML( XMLChild->Attribute("Foreground") ) );
		}
		else if( strcmp( name, "UI:Slider" ) == 0 )
		{
			pWidget = CreateSliderFromXML( XMLChild );
		}
		else if ( strcmp( name, "UI:ProgressBar")==0)
		{
			pWidget = CreateProgressFromXML(XMLChild);
		}
		else if ( strcmp(name, "UI:ScrollView") ==0)
		{
			pWidget = CreateScrollFromXML(XMLChild);
		}
		else if (strcmp(name,"UI:TextRich") ==0)
		{
			pWidget = CTextRich::create();
		}
		else if (strcmp(name, "UI:CheckBox") ==0)
		{
			pWidget = CreateCheckBoxFromXML(XMLChild);
		}
		else if (strcmp(name, "UI:RadioButton") ==0)
		{
			pWidget = CreateRadioButtonFromXML(XMLChild);
		}
		else if (strcmp(name, "UI:ImageViewScale9") ==0)
		{
			pWidget = CreateImageScale9FromXML(XMLChild);
		}
		else if (strcmp(name, "Label") ==0)
		{
			pWidget = CreateLabelFromXML(XMLChild);
			if( XMLChild->Attribute("Foreground") != NULL )	
			{
				((CLabel*)pWidget)->setColor(GetColorFromXML( XMLChild->Attribute("Foreground")));
			}
		}
		else if (strcmp(name, "UI:CCNode") ==0)
		{
			pWidget = CreateNodeFromXML(XMLChild);
		}
		else if (strcmp(name,"Grid") == 0)
		{
			//	CRadioBtnGroup *radioGroup = createRadioBtnGroupFromXml(XMLChild);
			pWidget = createRadioBtnGroupFromXml(XMLChild);
		}
		else if (strcmp(name,"Canvas")==0)
		{
			pWidget =  CLayout::create();
			CLayout *bo = CLayout::create();
			int tag = XMLChild->IntAttribute("Tag");	
			if (tag!=0)
			{
				pWidget->setTag(tag);
			}

			const char* str =XMLChild->Attribute("x:Name");
			if (str)
			{
				((CLayout*)pWidget)->setId(str);
			}

 			pWidget->setPosition(ccp(MainLayer->getContentSize().width/2,MainLayer->getContentSize().height/2));
			pWidget->setContentSize(MainLayer->getContentSize());

			float width = XMLChild->FloatAttribute("Width");
			float height = XMLChild->FloatAttribute("Height");
			if (width!=0&&height!=0&&str&&strstr(str,"Cell"))
			{
				pWidget->setContentSize(CCSizeMake(width,height));
			}

			if( XMLChild->Attribute("d:IsHidden") != NULL && strcmp( XMLChild->Attribute("d:IsHidden"), "True" ) == 0 )
			{
				pWidget->setVisible(false);
			}

			ReadXmlNode(XMLChild, dynamic_cast<CLayout*>(pWidget));
		}

		if (pWidget)
		{
			MainLayer->addChild(pWidget);
			if ( XMLChild->Attribute("Panel.ZIndex") != NULL)
			{
				pWidget->setZOrder(XMLChild->IntAttribute("Panel.ZIndex"));
			}

			float scaleX = 1.0f; float scaleY = 1.0f; float skewX = 0; float skewY = 0; float rotateAngle = 0; float translateX = 0; float translateY = 0;
			ReadControlTransform( XMLChild, scaleX, scaleY, skewX, skewY, rotateAngle, translateX, translateY );

			pWidget->setScaleX(scaleX);
			pWidget->setScaleY(scaleY);
			if (rotateAngle!=0)
			{
				pWidget->setRotation(rotateAngle);
			}

			if (translateX!=0||translateY!=0)
			{
				pWidget->setPositionX(pWidget->getPositionX()+translateX);
				pWidget->setPositionY(pWidget->getPositionY()-translateY);
			}

			CWidget *wid  = dynamic_cast<CWidget*>(pWidget);
			if (wid)
			{
				wid->setDescription(name);
			}
		}
		XMLChild = XMLChild->NextSiblingElement();
	}
}

CLayout* LoadComponent( const char* xml )
{
	tinyxml2::XMLDocument doc;

#if ( CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID )
	std::string filePath = CCFileUtils::sharedFileUtils()->fullPathForFilename(xml);
	CCLOG("filePath: %s", filePath.c_str());
	CCString* xmlContent = CCString::createWithContentsOfFile(filePath.c_str());
	tinyxml2::XMLError error = doc.Parse(xmlContent->getCString());
	if( error != tinyxml2::XML_SUCCESS ) return NULL;
#else 

#if( CC_TARGET_PLATFORM == CC_PLATFORM_WP8 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT )

	std::string s = xml;
	std::string ext = ".xaml";
	s.replace(s.find(ext), ext.size(), ".xml");
	const char* filePath = s.c_str();

	CCString* xmlContent = CCString::createWithContentsOfFile(filePath);
	tinyxml2::XMLError error = doc.Parse(xmlContent->getCString());
	if( error != tinyxml2::XML_SUCCESS ) return NULL;
#else
	tinyxml2::XMLError error = doc.LoadFile( CCFileUtils::sharedFileUtils()->fullPathForFilename(xml).c_str() );
	if( error != tinyxml2::XML_SUCCESS )
	{
		CCLOG("[ *ERROR ] CLayout* LoadComponent %s ",xml);
		return NULL;
	}
#endif

#endif

	tinyxml2::XMLNode* node = doc.FirstChildElement();
	tinyxml2::XMLElement* XMLCanvas = node->FirstChildElement("Canvas");
	if( XMLCanvas == NULL ) return NULL;

	if( XMLCanvas->Attribute("Height") != NULL )
		SCREEN_HEIGHT = XMLCanvas->FloatAttribute("Height");
	else
		SCREEN_HEIGHT = CCDirector::sharedDirector()->getWinSize().height;
	CLayout* MainLayer = CLayout::create();
	MainLayer->setId("UI");
	MainLayer->setAnchorPoint(CCPointCenter);
	if( XMLCanvas->Attribute("Height") != NULL && XMLCanvas->Attribute("Width") != NULL )
		MainLayer->setContentSize( ccp( XMLCanvas->FloatAttribute("Width"), XMLCanvas->FloatAttribute("Height") ) );
	else
		MainLayer->setContentSize( CCDirector::sharedDirector()->getWinSize() );

	ReadXmlNode(XMLCanvas, MainLayer);

	return MainLayer;
}

void SetButtonClickEvent( CCNode* designLayer, int tag, CCObject* pTarget, SEL_ClickHandler pHandler )
{
	CButton* node = (CButton*)designLayer->getChildByTag(tag);
	((CButton*)(node))->setOnClickListener( pTarget, pHandler );
}

void LogTime(char* msg)
{
	struct cc_timeval tv;     
	CCTime::gettimeofdayCocos2d( &tv, NULL );
	long time = tv.tv_sec * 1000 + tv.tv_usec / 1000;    
	printf("%s:%d\n",msg,time);
}