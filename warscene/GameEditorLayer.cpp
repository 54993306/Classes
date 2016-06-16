#include "GameEditorLayer.h"

#include "GMessage.h"
#include "model/DataDefine.h"
#include "scene\BattleScene.h"

typedef enum TextFieldTypes {
	MOVE_SPEED,
	COLD_DOWN,
	GUARD_TYPE,
	GUARD_RANGE,
	INIT_COST,
	USE_COST,
	ADD_COST,
	MOVE_COST,
	MAX_COST,
	ATK_INTERVAL,
	ROLE_TYPE,
	HP,
	ATK,
	DEF,
	CRIT,
	HIT,
	DODGE,
	DEX,
	RENEW,
	COUNT,
} TextFieldTypes;

static char* STRING_MOVE_SPEED = "MoveSpeed";
static char* STRING_COLD_DOWN = "ColdDown";
static char* STRING_GUARD_TYPE = "guardType";
static char* STRING_GUARD_RANGE = "guardRange";
static char* STRING_INIT_COST = "initCost";
static char* STRING_USE_COST = "useCost";
static char* STRING_ADD_COST = "addCost";
static char* STRING_MOVE_COST = "moveCost";
static char* STRING_MAX_COST = "maxCost";
static char* STRING_ATK_INTERVAL = "atkInterval";
static char* STRING_ROLE_TYPE = "roletype";
static char* STRING_HP = "hp";
static char* STRING_ATK = "atk";
static char* STRING_DEF = "def";
static char* STRING_CRIT = "crit";
static char* STRING_HIT = "hit";
static char* STRING_DODGE = "dodge";
static char* STRING_DEX = "dex";
static char* STRING_RENEW = "renew";

static char* s_textFieldTexts[] = { STRING_MOVE_SPEED, 
									STRING_COLD_DOWN,
									STRING_GUARD_TYPE,
									STRING_GUARD_RANGE,
									STRING_INIT_COST,
									STRING_USE_COST,
									STRING_ADD_COST,
									STRING_MOVE_COST,
									STRING_MAX_COST,
									STRING_ATK_INTERVAL,
									STRING_ROLE_TYPE,
									STRING_HP,
									STRING_ATK,
									STRING_DEF,
									STRING_CRIT,
									STRING_HIT,
									STRING_DODGE,
									STRING_DEX,
									STRING_RENEW,
							};
	//int	posX;					//地图数组下标X[Y,X]
	//int	posY;					//地图数组下标Y[Y,X]
	//int	hp;						//血量
	//int	atk;					//攻击
	//int	def;					//防御
	//int	crit;					//暴击
	//int hit;					//命中
	//int dodge;					//闪避
	//int dex;					//敏捷
	//int renew;					//回复
	//int minRound;				//释放技能的最小回合数(怪物用)
	//int maxRound;				//释放技能的最大回合数
	//bool battle;				//武将是否为上阵状态(战斗中标记队长)
	//int zoom;					//缩放比
	//int combat;					//战斗力
	//string name;				//名称
	//int	level;					//英雄等级
	//int hasitem;				//掉落物品类型1、蓝2、金3、彩4、道具5、金币
	//int addHurtRate;			//增伤比率
	//int cutHurtRate;			//减伤比率
	//int ice_def;				//冰防御
	//int fire_def;				//火防御
	//int thunder_def;			//雷防御	
	//
GameEditorLayer::GameEditorLayer()
{
}

GameEditorLayer::~GameEditorLayer()
{
}

bool GameEditorLayer::init()
{
	if (!BaseLayer::init())
		return false;

	this->setVisible(false);
	
	CCNotificationCenter::sharedNotificationCenter()->addObserver(this,callfuncO_selector(GameEditorLayer::ToggleButtonClickedHandler),TOGGLE_GAME_EDITOR,nullptr);
	CCNotificationCenter::sharedNotificationCenter()->addObserver(this,callfuncO_selector(GameEditorLayer::AliveObjectClickedHandler),ALIVE_OBJECT_CLICKED,nullptr);
	
	//CButton* bt_toggleGameEditor = CButton::create("public/btn_tihuanwujiang_01.png", "public/btn_tihuanwujiang_02.png");
	//bt_toggleGameEditor->setPosition(CCPoint(300, 0));
	//bt_toggleGameEditor->setText("HIHI!");
	//bt_toggleGameEditor->setTouchEnabled(true);

	//bt_toggleGameEditor->setTag(CL_OpenGameEditorBtn);
	//bt_toggleGameEditor->setOnClickListener(this,ccw_click_selector(GameEditorLayer::OnClicked));
	//this->addChild(bt_toggleGameEditor);
	
	m_textFields = new CursorTextField*[TextFieldTypes::COUNT];
	CCPoint startPos = CCPoint( 100, 600 );
	CCSize textfieldSize = CCSize(100, 20);
	float labelTextfielddOffsetX = 150;
	int fontSize = 16;

	for( int i = 0; i < TextFieldTypes::COUNT; ++i ) {
		float offsetY = (textfieldSize.height * 1.3);
		CLabel* label = CLabel::create( s_textFieldTexts[i], FONT_NAME, fontSize );
		label->setPosition(ccp(startPos.x,startPos.y - offsetY * i));
		this->addChild(label);

		CCSprite* bg = CCSprite::create("bagUI/jinbikuang.png");
		bg->setPosition(ccp(startPos.x + labelTextfielddOffsetX,startPos.y - offsetY * i));
		bg->setScaleX( textfieldSize.width / bg->getContentSize().width );
		bg->setScaleY( textfieldSize.height / bg->getContentSize().height );
		this->addChild(bg);

		CursorTextField* textfield;
		textfield = CursorTextField::textFieldWithPlaceHolder("", FONT_NAME, fontSize,textfieldSize,ccWHITE); 
		textfield->setTag(i);
		textfield->setColorSpaceHolder(ccWHITE);
		textfield->setPosition(ccp(startPos.x + labelTextfielddOffsetX,startPos.y - offsetY * i));
		textfield->setLimitNum(5);
		//textfield->setHorizontalAlignment(kCCTextAlignmentCenter);
		//textfield->setVerticalAlignment(kCCVerticalTextAlignmentBottom);
		textfield->setPriority(-999);
		//textfield->setAnchorPoint(ccp(0,0));
		addChild(textfield);

		m_textFields[i] = textfield;
	}
	

//	m_ui = LoadComponent("story.xaml");
//	m_ui->retain();
//	m_ui->setPosition(VCENTER);
//	this->setContentSize(m_ui->getContentSize());
//	this->setTouchPriority(StoryPriority);
//	this->setIsShowBlack(false);
//#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
//	this->setTouchEnabled(true);
//	m_isStory = true;
//#else
//	this->setTouchEnabled(false);
//	m_isStory = false;
//#endif
//	m_storyPanel = (CLayout*)m_ui->getChildByTag(storyPanel_tag);
//	CButton* rightJumpBtn = (CButton*)m_ui->getChildByTag(rightJumpBtn_tag);
//	rightJumpBtn->setOnClickListener(this,ccw_click_selector(StoryLayer::PostEnd));
//
//	CLabel* leftName = CLabel::create("LeftName",FONT_NAME,30);
//	leftName->setColor(ccc3(180,159,112));
//	leftName->setPosition(ccpAdd(m_storyPanel->getChildByTag(leftNameBg_tag)->getPosition(),ccp(-60,0)));
//	leftName->setTag(leftName_tag);
//	m_storyPanel->addChild(leftName);
//
//	m_storyPanel->getChildByTag(rightNameBg_tag)->setScaleX(-1);
//	CLabel* rightName = CLabel::create("RightName",FONT_NAME,30);
//	rightName->setColor(ccc3(180,159,112));
//	rightName->setPosition(ccpAdd(m_storyPanel->getChildByTag(rightNameBg_tag)->getPosition(),ccp(60,0)));
//	rightName->setTag(rightName_tag);
//	m_storyPanel->addChild(rightName);
//
//	m_content = CCArray::create();
//	m_content->retain();
//	this->addChild(m_ui);
//	CCNotificationCenter::sharedNotificationCenter()->addObserver(this,callfuncO_selector(StoryLayer::CreateStory),LAYERMOVEEND,nullptr);
	return true;
}

void GameEditorLayer::ToggleButtonClickedHandler(CCObject* obj)
{
	this->setVisible(!this->isVisible());
	if ( this->isVisible() ) {
		this->getParent()->reorderChild(this, 0);
	}

}

void GameEditorLayer::AliveObjectClickedHandler(CCObject* obj)
{
	WarAlive* alive = (WarAlive*)obj;
	
	std::ostringstream os;
	os << "hero id: " << alive->getAliveID();

	
	for( int i = 0; i < TextFieldTypes::COUNT; ++i ) {
		CursorTextField* textField = m_textFields[i];
		if ( textField->getTag() == TextFieldTypes::MOVE_SPEED ) {
			textField->setString("move speed" );
		}
//			
//static char* STRING_MOVE_SPEED = "MoveSpeed";
//static char* STRING_COLD_DOWN = "ColdDown";
//static char* STRING_GUARD_TYPE = "guardType";
//static char* STRING_GUARD_RANGE = "guardRange";
//static char* STRING_INIT_COST = "initCost";
//static char* STRING_USE_COST = "useCost";
//static char* STRING_ADD_COST = "addCost";
//static char* STRING_MOVE_COST = "moveCost";
//static char* STRING_MAX_COST = "maxCost";
//static char* STRING_ATK_INTERVAL = "atkInterval";
//static char* STRING_ROLE_TYPE = "roletype";
//static char* STRING_HP = "hp";
//static char* STRING_ATK = "atk";
//static char* STRING_DEF = "def";
//static char* STRING_CRIT = "crit";
//static char* STRING_HIT = "hit";
//static char* STRING_DODGE = "dodge";
//static char* STRING_DEX = "dex";
//static char* STRING_RENEW = "renew";
	}
	//CursorTextField* textFieldAccount = nullptr;
	//textFieldAccount = CursorTextField::textFieldWithPlaceHolder(os.str().c_str(), FONT_NAME, 30,CCSize(300,300),ccWHITE);
	//textFieldAccount->setPriority(1);
	//textFieldAccount->setPosition(CCPoint(300, 300));
	//textFieldAccount->setTextInput(os.str().c_str());
	//this->addChild(textFieldAccount);
}

void GameEditorLayer::OnClicked(CCObject* ob)
{
	CCLog("OnClicked");
}

//void GameEditorLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
//{
//	CCLog("inside touchend");
//
//	m_textFieldMoveSpeed->attachWithIME();
//}