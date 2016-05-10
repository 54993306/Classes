#ifndef __TASK_LAYER_
#define __TASK_LAYER_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "protos/protocol.h"
#include "task.h"

class CPanelTips;

class CTaskLayer:public BaseLayer
{
public:
	CTaskLayer():m_selectHero(0),m_selectTask(nullptr), m_bFirstShow(true),m_taskType(1),m_pPanelTips(nullptr){}
	~CTaskLayer();
	CREATE_LAYER(CTaskLayer);
	virtual bool init();
	void onEnter();
	void onGetTaskPrize(CCObject* pSender);
	void onExit();
	void processNetMsg(int type, google::protobuf::Message *msg);

	void showTaskTip(TaskListResponse * res);

	void popItemPrize(PrizeResponse* res);
	int  getTaskType();
protected:
	void onClose(CCObject* pSender);
	void onSelectTask(CCObject* pSender);
	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);

	void addTableCell(unsigned int uIdx, CTableViewCell * pCell);

	void onSwitchBtn(CCObject *pSender, bool bChecked);

	void isFinishTask(bool isFinish);

	void showTaskView();
	void updateTaskInfo(CTask* data);
	void toDoTasking(float dt);
	void deleteTaskMsg(const TMessage& tMsg);
	void onGoTask(CCObject* pSender);

private:
	void showNoTaskTip(bool bShow, const char* sInfo);

private:
	CLayout *m_ui;
	int  m_selectHero;
	CLayout *m_cell;
	CTableView *m_tableView;

	CLayout *m_poseLayer;
	CLayout *m_garrsionLayer;

	CTask *m_selectTask;

	vector<CTask> m_taskList;
	vector<CTask> m_taskAllList;
	int m_taskType;
	bool m_bFirstShow;

	CPanelTips* m_pPanelTips;
};



#endif // !__GOLDMINE_LAYER_
