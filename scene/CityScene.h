#ifndef _CITY_SCENE_H_
#define _CITY_SCENE_H_
#include "AppUI.h"
//#include "alive/AliveDefine.h"
#include<map>
using namespace std;
namespace BattleSpace{
	class ActObject;
}
class CityScene : public CScene,public CCTargetedTouchDelegate 
{
public:
	CityScene();
	virtual ~CityScene();
	DEFINE_SCENE_CLASS(CityScene);
	virtual void onCreate();
	virtual void onEnter();
	virtual void onExit();
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void update(float delta);
	void initScene();

	void roleMoveToPos(int roleId, float x, float y);
	BattleSpace::ActObject* addAlive(unsigned int aliveID);//增加角色对象
	void removeAlive(unsigned int aliveID);//删除角色
	void updateRoleProperty(const TMessage& tMsg);//更新角色属性
	void ProcessMsg(int type, google::protobuf::Message *msg);
protected:
	void createMap();
	void onNpcClick(CCObject* target);
	void onMainClick(CCObject* target);
	void onSetClick(CCObject* target);
	void onBattleClick(CCObject* ob);
	void onBagClick(CCObject* target);

	void relocation(CCNode* pSender);

	void runCloudAction();

	void showHeroList(const CHero& hero,int index);

protected:
	//CCTMXTiledMap* m_map;
	CCSprite* m_map;
	BattleSpace::ActObject* m_hero;
	CCPoint m_desPos;
	CCPoint m_real;
	float m_fdetal;
	map<int,CCPoint> m_actDesPos;
	map<string,CCPoint> m_actRealPos;
	bool m_aiStart;
	bool m_isInit;
	int m_maxY;
	//SetPanel* m_setPanel;
	CCPoint m_cloudPos;
	CCPoint m_cloudAPos;

};
#endif // !_CITY_SCENE_H_
