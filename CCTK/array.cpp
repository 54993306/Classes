/******************************************************************************
文件名: array.cpp
编写者: csdn_viva@foxmail.com ccbase1.0
编写日期: 2013-5-2
功能描述: 在传递消息的过程中用到的消息容器。
******************************************************************************/
#include "array.h"

/******************************************************************************
函数名称: ~CArray
功能描述: 析构时，释放容器内所有指针和其他数组
******************************************************************************/
CArray::~CArray(void)
{
	if(!m_vParamList.empty())
	{
		for(vector<TParameter>::iterator iter = m_vParamList.begin(); iter != m_vParamList.end(); iter++)
		{
			if(iter->pValue != NULL)
			{
				switch(iter->nType)
				{
				case PT_INT:
					delete (int*)iter->pValue;
					break;
				case PT_SHORT:
					delete (short*)iter->pValue;
					break;
				case PT_FLOAT:
					delete (float*)iter->pValue;
					break;
				case PT_STR:
					delete (char*)iter->pValue;
					break;
				case PT_ARRAY:
					delete (CArray*)iter->pValue;
					break;
				}
			} else {
				CCLOGERROR("## [ERROR] CArray::~CArray(void) if(iter->pValue != NULL)");
			}
		}
		m_vParamList.clear();
	}
}


/******************************************************************************
函数名称: Add(重载)
功能描述: 添加一个INT型变量
输入: num - value
返回值:
******************************************************************************/
void CArray::Add(int num)
{
	TParameter p;
	p.nType  = PT_INT;
	p.pValue = new int;
	*((int*)p.pValue) = num;

	m_vParamList.push_back(p);
}

/******************************************************************************
函数名称: Add(重载)
功能描述: 添加一个SHORT型变量
输入: num - value
返回值:
******************************************************************************/
void CArray::Add(short num)
{
	TParameter p;
	p.nType  = PT_SHORT;
	p.pValue = new short;
	*((short*)p.pValue) = num;

	m_vParamList.push_back(p);
}

/******************************************************************************
函数名称: Add(重载)
功能描述: 添加一个FLOAT型变量
输入: num - value
返回值:
******************************************************************************/
void CArray::Add(float num)
{
	TParameter p;
	p.nType  = PT_FLOAT;
	p.pValue = new float;
	*((float*)p.pValue) = num;

	m_vParamList.push_back(p);
}

/******************************************************************************
函数名称: Add(重载)
功能描述: 添加一个char*型变量
输入: str - value
返回值:
******************************************************************************/
void CArray::Add(char* str)
{
	TParameter p;
	p.nType  = PT_STR;
	p.pValue = new char[strlen(str) + 1];
	strcpy((char*)p.pValue, str);
	
	m_vParamList.push_back(p);
}

/******************************************************************************
函数名称: Add(重载)
功能描述: 添加一个CArray*型变量
输入: pArray - value
返回值:
******************************************************************************/
void CArray::Add(CArray* pArray)
{
	TParameter p;
	p.nType  = PT_ARRAY;
	p.pValue = pArray;

	m_vParamList.push_back(p);
}

/******************************************************************************
函数名称: GetInt
功能描述: 按下标取出一个INT型
输入: nIndex - 下标
返回值: 对应值
******************************************************************************/
int CArray::GetInt(int nIndex)
{
	if(nIndex >= 0 && nIndex < (int)m_vParamList.size())
	{
		if(m_vParamList[nIndex].nType == PT_INT )
		{
			return *((int*)m_vParamList[nIndex].pValue);
		}
		else
		{
			CCLOGERROR("## [ERROR] CArray::GetInt if(m_vParamList[nIndex].nType == PT_INT )");
		}
	}
	else
	{
		CCLOGERROR("## [ERROR] CArray::GetInt if(nIndex >= 0 && nIndex < (int)m_vParamList.size())");
	}
	return 0;
}

/******************************************************************************
函数名称: GetShort
功能描述: 按下标取出一个SHORT型
输入: nIndex - 下标
返回值: 对应值
******************************************************************************/
short CArray::GetShort(int nIndex)
{
	if(nIndex >= 0 && nIndex < (int)m_vParamList.size())
	{
		if(m_vParamList[nIndex].nType == PT_SHORT )
		{
			return *((short*)m_vParamList[nIndex].pValue);
		}
		else
		{
			CCLOGERROR("## [ERROR] CArray::GetShort if(m_vParamList[nIndex].nType == PT_SHORT )");
		}
	}
	else
	{
		CCLOGERROR("## [ERROR] CArray::GetShort if(nIndex >= 0 && nIndex < (int)m_vParamList.size())");
	}
	return 0;
}

/******************************************************************************
函数名称: GetFloat
功能描述: 按下标取出一个FLOAT型
输入: nIndex - 下标
返回值: 对应值
******************************************************************************/
float CArray::GetFloat(int nIndex)
{
	if(nIndex >= 0 && nIndex < (int)m_vParamList.size())
	{
		if(m_vParamList[nIndex].nType == PT_FLOAT )
		{
			return *((float*)m_vParamList[nIndex].pValue);
		}
		else
		{
			CCLOGERROR("## [ERROR] CArray::GetFloat if(m_vParamList[nIndex].nType == PT_FLOAT )");
		}
	}
	else
	{
		CCLOGERROR("## [ERROR] CArray::GetFloat if(nIndex >= 0 && nIndex < (int)m_vParamList.size())");
	}
	return 0;
}

/******************************************************************************
函数名称: GetString
功能描述: 按下标取出一个CHAR*型
输入: nIndex - 下标
返回值: 对应值
******************************************************************************/
char* CArray::GetString(int nIndex)
{
	if(nIndex >= 0 && nIndex < (int)m_vParamList.size())
	{
		if(m_vParamList[nIndex].nType == PT_STR )
		{
			return (char*)(m_vParamList[nIndex].pValue);
		}
		else
		{
			CCLOGERROR("## [ERROR] CArray::GetString if(m_vParamList[nIndex].nType == PT_STR )");
		}
	}
	else
	{
		CCLOGERROR("## [ERROR] CArray::GetString if(nIndex >= 0 && nIndex < (int)m_vParamList.size())");
	}
	return NULL;
}

/******************************************************************************
函数名称: GetArray
功能描述: 按下标取出一个CArray*型
输入: nIndex - 下标
返回值: 对应值
******************************************************************************/
CArray* CArray::GetArray(int nIndex)
{
	if(nIndex >= 0 && nIndex < (int)m_vParamList.size())
	{
		if(m_vParamList[nIndex].nType == PT_ARRAY )
		{
			return (CArray*)(m_vParamList[nIndex].pValue);
		}
		else
		{
			CCLOGERROR("## [ERROR] CArray::GetArray if(m_vParamList[nIndex].nType == PT_ARRAY )");
		}
	}
	else
	{
		CCLOGERROR("## [ERROR] CArray::GetArray if(nIndex >= 0 && nIndex < (int)m_vParamList.size())");
	}
	return NULL;
}