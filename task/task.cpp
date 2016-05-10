#include "task.h"

void CTask::read(const Task& task)
{
	this->taskId = task.taskid();
	this->name = task.name();
	this->level = task.level();
	this->process = task.process();
	this->target = task.target();
	this->exp = task.exp();
	this->coin = task.coin();
	this->food = task.food();
	this->note = task.note();
	this->isNew = task.isnew();
	this->finish = task.finish();

	for (int i = 0; i < task.prize_size(); i++)
	{
		CPrize prize;
		prize.read(task.prize(i));
		this->prizeList.push_back(prize);
	}
	this->tindex = task.tindex();

	this->action = task.action();
	this->gold = task.gold();
	this->getTime = task.gettime();
	this->end = task.end();
}
