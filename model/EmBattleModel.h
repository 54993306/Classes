#ifndef EMBATTLE_MODEL_H_
#define EMBATTLE_MODEL_H_
#include "AppUI.h"
#include <vector>
using namespace std;
class EmBattleModel
{
public:
	EmBattleModel();
	~EmBattleModel();
	void setMainWuJiang(unsigned int uid);
	inline unsigned int getMainWuJiang(){return m_main;}
	void addSubWuJiang(unsigned int uid);
	vector<unsigned int> getSubWuJiang();
	inline void setOpenNum(char val){m_openNum = val;};
	inline char getOpenNum(){return m_openNum;}
	inline unsigned int getSelectID(){return m_selectID;}
	void setSelectID(unsigned int id);
	void addWuJiang(unsigned int id);
	void removeSubWuJiang(unsigned int id);
	bool hasSubWuJiang(unsigned int id);
protected:
	unsigned int m_main;
	vector<unsigned int> m_sub;
	char m_openNum;
	unsigned int m_selectID;//默认选中武将
};
#endif // !EMBATTLE_MODEL_H_
