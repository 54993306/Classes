#include "RollLabel.h"

CRollLabel::CRollLabel(const char* str, char* fontName, float size, CCRect rect)
	:m_offsetRoll(0.0f),m_isRoll(true),m_rollTime(0),m_currRollTime(0)
{
    m_label = CLabel::create(str,fontName,size);
	m_label->setPosition(ccp(rect.origin.x+ rect.size.width,rect.origin.y));
	m_label->setAnchorPoint(ccp(0,0));
	m_rect = rect;
	this->addChild(m_label);
}


CRollLabel::~CRollLabel()
{

}

CRollLabel* CRollLabel::create(const char* str, char* fontName, float size, CCRect rect)
{
	CRollLabel* rollLable = new CRollLabel(str,fontName, size, rect);
	if (rollLable)
	{
		rollLable->autorelease();
		return rollLable;
	}
	CC_SAFE_DELETE(rollLable);
	return nullptr;
}

void CRollLabel::setIsRoll(bool isRoll)
{
	m_isRoll = isRoll;
}

void CRollLabel::setColor(ccColor3B& color)
{
	m_label->setColor(color);
}

void CRollLabel::visit()
{
	CCPoint orig = convertToWorldSpace(m_rect.origin);
	float scale = this->getScale();
	glEnable(GL_SCISSOR_TEST);
	CCEGLView::sharedOpenGLView()->setScissorInPoints(orig.x, orig.y, m_rect.size.width, m_rect.size.height);
	CCNode::visit();
	glDisable(GL_SCISSOR_TEST);
	if (m_isRoll)
	{
		m_offsetRoll += 0.8f;
		m_label->setPositionX(m_rect.origin.x + m_rect.size.width - m_offsetRoll);
		if (m_offsetRoll>= m_rect.size.width + m_label->getContentSize().width)//&&(m_label->getContentSize().width - m_rect.size.width>0)) m_label->getContentSize().width
		{
			m_offsetRoll = 0.0f;
			m_currRollTime++;
		}	

		if (m_rollTime>0&&m_rollTime==m_currRollTime)
		{
			m_isRoll = false;
			this->schedule(schedule_selector(CRollLabel::removeSelf));
		}
	}
}

void CRollLabel::removeSelf(float dt)
{
	this->getParent()->removeFromParent();
	this->unscheduleAllSelectors();
}

void CRollLabel::setRollTime(int rollTime)
{
	m_rollTime = rollTime;
}
