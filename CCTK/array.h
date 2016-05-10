/******************************************************************************
文件名: array.h
编写者: csdn_viva@foxmail.com ccbase1.0
编写日期: 2013-5-2
功能描述: 在传递消息的过程中用到的消息容器。
******************************************************************************/
#ifndef __CCBASE_ARRAY_H__
#define __CCBASE_ARRAY_H__

#include <vector>
#include "cocos2d.h"
using namespace std;
using namespace cocos2d;

//通用类型描述
typedef struct {
	char	nType;		//类型 对应PT_XXX
	void*	pValue;		//实例指针
} TParameter;

//参数类型定义
#define	PT_INT		1
#define PT_SHORT	2
#define PT_FLOAT	3
#define PT_STR		4
#define PT_ARRAY	5


class CArray
{
public:
	~CArray(void);

public:
	//增加一个INT型到数组中
	void	Add(int num);
	//增加一个SHORT型到数组中
	void	Add(short num);
	//增加一个FLOAT型到数组中
	void	Add(float num);
	//增加一个CHAR*型到数组中
	void	Add(char* str);
	//增加一个CArray*型到数组中
	void	Add(CArray* pArray);

	//按下标取出一个INT型
	int		GetInt(int nIndex);
	//按下标取出一个SHORT型
	short	GetShort(int nIndex);
	//按下标取出一个FLOAT型
	float	GetFloat(int nIndex);
	//按下标取出一个CHAR*型
	char*	GetString(int nIndex);
	//按下标取出一个CArray*型
	CArray* GetArray(int nIndex);

private:
	//数组元素容器
	vector<TParameter> m_vParamList;
};

#endif //__CCBASE_ARRAY_H__