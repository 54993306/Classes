#include "Battle/GuardArea.h"
#include "Battle/BattleRole.h"
#include "Battle/RoleBaseData.h"
#include "warscene/ConstNum.h"
#include "warscene/BattleTools.h"
#include "model/WarManager.h"
GuardArea::GuardArea(WarManager* pManage)
:mManage(pManage)
{}

GuardArea* GuardArea::create( WarManager* pManage )
{
	GuardArea* tGuardArea = new GuardArea(pManage);
	if (tGuardArea)
	{
		tGuardArea->autorelease();
		return tGuardArea;
	}else{
		delete tGuardArea; 
		tGuardArea = NULL; 
		return NULL; 
	}
}
//��������ı仯���ֵ���佫��λ�ñ仯�й�ϵ,�������佫���ƶ�������ӱ仯������ÿ�ζ�ȥˢ�´���
int GuardArea::getAliveGuard( WarAlive* pAlive )
{
	vector<int> Vecguard;
	initAliveGuard(pAlive,Vecguard);
	for (auto i:Vecguard)
	{
		for (auto tAlive:*mManage->getVecMonsters())
		{
			vector<int>::reverse_iterator iter= tAlive->mStandGrids.rbegin();
			for (;iter != tAlive->mStandGrids.rend();iter++)			//�ҷ��Թ���Ӧ�ô����ĸ��ӿ�ʼ�����ж�
			{
				if ( i != *iter )										//��ǰ��������Χ�ڴ��ڵз�Ŀ��
					continue;
				int tMoveGrid = i;
				int tRow = i%C_GRID_ROW;
				if ( tRow+pAlive->getBaseData()->getRoleRow() > C_GRID_ROW )				//�ҷ��佫ռ3��,�ط�������row=3��λ�ã�������ͼ��Χ��
					tMoveGrid = i-((pAlive->getBaseData()->getRoleRow()+tRow)-C_GRID_ROW);//�ѳ����Ĳ��ּ���
				if ( tMoveGrid < C_BEGINGRID || tMoveGrid > C_ENDGRID )
					break;
				return tMoveGrid;
			}
		}
	}
	return INVALID_GRID;												//û�о��������Զ�������
}

void GuardArea::initAliveGuard( WarAlive* pAlive,vector<int>& pGuards )
{
	switch (pAlive->getBaseData()->getAlertType())
	{
	case eFrontGuard:
		{
			guardFront(pAlive,pGuards);
		}break;
	case eBackGuard:
		{
			guradBack(pAlive,pGuards);
		}break;
	case eFrontAndBackGuard:
		{
			guradFrontAndBack(pAlive,pGuards);
		}break;
	case eUpDownGuard:
		{
			guradUpAndDown(pAlive,pGuards);							//û�г��ȵ�����
		}break;
	default:
		{
			if (pAlive->getBaseData()->getAlertType())
				CCLOG("[ *ERROR ] SkillRange::initAliveGuard Type = %d",pAlive->getBaseData()->getAlertType());
			return;
		}
	}
	VectorRemoveRepeat(pGuards);
	sort(pGuards.begin(),pGuards.end(),greater<int>());				//�������������жϺ����Ϸ������Ƿ�����
}

void GuardArea::initAliveCurrGrids( WarAlive* pAlive,vector<int>& pVector )
{
	if (pAlive->getTouchState())
	{
		pVector.assign(pAlive->mTouchGrids.begin(),pAlive->mTouchGrids.end());
	}else{
		pVector.assign(pAlive->mStandGrids.begin(),pAlive->mStandGrids.end());
	}
}

void GuardArea::guradUpAndDown( WarAlive* pAlive,vector<int>& pGuards )
{
	vector<int>tGrids;
	initAliveCurrGrids(pAlive,tGrids);
	for (auto tGrid:tGrids)
	{
		int aliveCol = tGrid / C_GRID_ROW;
		int aliveRow = tGrid % C_GRID_ROW;
		for (int i=aliveRow-1;i<=aliveRow+1;i++)
		{
			if (i>=C_GRID_ROW||i<0)continue;
			pGuards.push_back(aliveCol*C_GRID_ROW+i);						
		}
	}
}

void GuardArea::guardFront( WarAlive* pAlive,vector<int>& pGuards )
{
	vector<int>tGrids;
	initAliveCurrGrids(pAlive,tGrids);
	for (auto tGrid:tGrids)
	{
		for (int i=1;i<=pAlive->getBaseData()->getAlertRange();i++)		//��2��ʼ��ԭ������Ϊ1������Ȧ(��������Ȧȥ���˴�1��ʼ)
			pGuards.push_back(tGrid - i*C_GRID_ROW);
	}
}

void GuardArea::guradBack( WarAlive* pAlive,vector<int>& pGuards )
{
	vector<int>tGrids;
	initAliveCurrGrids(pAlive,tGrids);
	for (auto tGrid:tGrids)
	{
		for (int i=1;i<=pAlive->getBaseData()->getAlertRange();i++)		//��2��ʼ��ԭ������Ϊ1������Ȧ(��������Ȧȥ���˴�1��ʼ)
			pGuards.push_back(tGrid + i*C_GRID_ROW);
	}
}

void GuardArea::guradFrontAndBack( WarAlive* pAlive,vector<int>& pGuards )
{
	vector<int>tGrids;
	initAliveCurrGrids(pAlive,tGrids);
	for (auto tGrid:tGrids)
	{
		for (int i=1;i<=pAlive->getBaseData()->getAlertRange();i++)		//��2��ʼ��ԭ������Ϊ1������Ȧ(��������Ȧȥ���˴�1��ʼ)
		{
			pGuards.push_back(tGrid - i*C_GRID_ROW);
			pGuards.push_back(tGrid + i*C_GRID_ROW);
		}
	}
}
