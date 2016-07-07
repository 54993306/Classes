#ifndef __PARSEFILEDATA__
#define __PARSEFILEDATA__
#include "AppUI.h"


/************************************************************************/


//用于解析在战斗中使用到的一些简单的数据文件


                                                                    
/************************************************************************/

void ParseMoveGrid(int stageid,vector<int> & pMoveGrids ,vector<int>& pUndefinedGrids);

void ParseAddCostGrid(int stageid,vector<int> & pAddCostArea);

void parseSpineModelFile(vector<int>& pVector);
#endif // !__PARSEFILEDATA__
