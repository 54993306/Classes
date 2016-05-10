#ifndef _COMBATDATASAVEXML_
#define _COMBATDATASAVEXML_

#include "AppUI.h"
/************************************************************************/
//							读写XML文件                                                           
//
/************************************************************************/

class CombatDataSaveXML : public CCObject
{
public:
	CombatDataSaveXML(){};
	virtual ~ CombatDataSaveXML(){};
	virtual bool init();
	CREATE_FUNC(CombatDataSaveXML);
	bool WriteXML();
	void ReadXML(const char* path);
protected:

private:
};
#endif