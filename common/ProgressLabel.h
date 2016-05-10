#ifndef __PROGRESS_LABEL_H__
#define __PROGRESS_LABEL_H__

#include "AppUI.h"

class CProgressLabel:public CCNode
{
public:
	CREATE_FUNC(CProgressLabel);
	CProgressLabel();
	void changeValueFromTo(float fromValue, float toValue, float duration);
	void changeValueTo(float toValue, float duration);
	void setLabel(CLabel* pLabel);
	void setInterval(float val);
protected:
	void performChange(float delt);

	void updateValue(float value);
	
private:
	CLabel *m_label;
	float  m_duration;
	float  m_fromValue;
	float  m_toValue;
	float  m_value;
	float  m_lapsed;

	float  m_interValTime;
	float  m_interLapse;
};


#endif