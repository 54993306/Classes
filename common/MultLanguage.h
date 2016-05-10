#ifndef _MULTLANGUAGE_H_
#define _MULTLANGUAGE_H_

#include "AppUI.h"
#include <map>
using namespace std;

#define GETLANGSTR(strid) CMultLanguage::getInstance()->getString(strid).c_str()

class CMultLanguage
{
public:
	static CMultLanguage* getInstance();
	void parseFromJson(const char* strFile);
	string getString(int strId);
	void setLangType(int type);
	int getLangType();
protected:
private:
	CMultLanguage();
	map<int,string> m_chineseSimMap;
	map<int,string> m_chineseComMap;
	map<int,string> m_englishMap;
	map<int,string> m_thailandMap;
	static CMultLanguage *m_instance;
	int m_langType;
};


#endif