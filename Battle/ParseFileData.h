#ifndef __PARSEFILEDATA__
#define __PARSEFILEDATA__
#include "AppUI.h"


/************************************************************************/


//���ڽ�����ս����ʹ�õ���һЩ�򵥵������ļ�


                                                                    
/************************************************************************/

void ParseMoveGrid(int stageid,vector<int> & pMoveGrids ,vector<int>& pUndefinedGrids);

void ParseAddCostGrid(int stageid,vector<int> & pAddCostArea);

void parseSpineModelFile(vector<int>& pVector);
#endif // !__PARSEFILEDATA__
