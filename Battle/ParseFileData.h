#ifndef __PARSEFILEDATA__
#define __PARSEFILEDATA__
#include "AppUI.h"


/************************************************************************/


//���ڽ�����ս����ʹ�õ���һЩ�򵥵������ļ�


                                                                    
/************************************************************************/

void ParseBattleGrid(int pStage,vector<int> &pMoveGrids ,vector<int> &pUndefinedGrids,vector<int> &pEnterArea,vector<int> &pOtherEnter);

void ParseAddCostGrid(int pStage,vector<int> & pAddCostArea);

void parseSpineModelFile(vector<int>& pVector);
#endif // !__PARSEFILEDATA__
