#ifndef __PARSEFILEDATA__
#define __PARSEFILEDATA__
#include "AppUI.h"


/************************************************************************/


//用于解析在战斗中使用到的一些简单的数据文件


                                                                    
/************************************************************************/

void ParseBattleGrid(int pStage,vector<int> &pMoveGrids ,vector<int> &pUndefinedGrids,vector<int> &pEnterArea,vector<int> &pOtherEnter);

void ParseAddCostGrid(int pStage,vector<int> & pAddCostArea);

void parseSpineModelFile(vector<int>& pVector);
#endif // !__PARSEFILEDATA__
