#include "ProgressLabel.h"

CProgressLabel::CProgressLabel():m_fromValue(0.0f),m_lapsed(0.0f),m_value(0.0f)
	,m_toValue(0.0f),m_duration(0.0f),m_interValTime(0.02f),m_interLapse(0.0f)
{

}

void CProgressLabel::setLabel(CLabel* pLabel)
{
	m_label = pLabel;
}

void CProgressLabel::changeValueFromTo(float fromValue, float toValue, float duration)
{
	m_fromValue = fromValue;
	m_toValue = toValue;
	m_value = fromValue;
	m_duration = duration;
	m_lapsed = 0.0f;
	m_interLapse = 0.0f;
	
	this->schedule(schedule_selector(CProgressLabel::performChange));
}



void CProgressLabel::performChange(float delt)
{
	if ((m_value< m_toValue&&m_toValue>m_fromValue)||(m_value>m_toValue&&m_toValue<m_fromValue))
	{
		m_lapsed += CCDirector::sharedDirector()->getDeltaTime();
		m_interLapse += CCDirector::sharedDirector()->getDeltaTime();

		float fTime = MAX(0, MIN(1, m_lapsed / m_duration));
		updateValue(m_fromValue + (m_toValue - m_fromValue) * fTime);
	}
	else
	{
		m_label->setString(ToString((int)m_toValue));
		this->unschedule(schedule_selector(CProgressLabel::performChange));
	}
}

void CProgressLabel::updateValue(float value)
{
	m_value = value;
	if (m_label&&m_interLapse>m_interValTime)
	{
		m_interLapse = 0.0f;
		if (m_value>0)
		{
			m_label->setString(ToString((int)value));
		}
		else
		{
			m_label->setString(ToString((int)value));
		}
	}
	
}

void CProgressLabel::changeValueTo(float toValue, float duration)
{
	if (m_label)
	{
		changeValueFromTo(atoi(m_label->getString()),toValue,duration);
	}
}

void CProgressLabel::setInterval(float val)
{
	m_interValTime = val;
}


