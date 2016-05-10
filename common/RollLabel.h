#ifndef  __ROLLLABELE_H
#define  __ROLLLABELE_H
#include "AppUI.h"
/*无限滚动label*/
class CRollLabel:public CCNode
{
public:
	CRollLabel(const char* str, char* fontName, float size, CCRect rect);
	~CRollLabel();
	static CRollLabel* create(const char* str, char* fontName, float size, CCRect rect);
	void setColor(ccColor3B& color);
	virtual void visit();
	void setIsRoll(bool isRoll);
	void setRollTime(int rollTime);
	void removeSelf(float dt);
private:
	CCRect m_rect;
	CLabel* m_label;
	float m_offsetRoll; 
	int  m_rollTime;  //滚动次数
	int  m_currRollTime;
	bool m_isRoll;
};



#endif // ! __ROLLLABELE_
