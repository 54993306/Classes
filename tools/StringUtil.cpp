#include "StringUtil.h"
#include <functional>
#include <algorithm>
//用分割符分割字符串，结果存放到一个列表中，列表中的对象为CCString
CCArray* strSplit(const char* srcStr, const char* strSymbol)
{
	CCArray* stringList = CCArray::create();

	int size = strlen(srcStr);

	/* 将数据转换为字符串对象 */
	CCString* str = CCString::create(srcStr);

	int startIndex = 0;
	int endIndex = 0;
	endIndex = str->m_sString.find(strSymbol);

	CCString* lineStr = NULL;

	/* 根据换行符拆分字符串，并添加到列表中 */
	while(endIndex!=-1) 
	{
		lineStr = CCString::create("");

		/* 截取一行字符串 */
		lineStr->m_sString = str->m_sString.substr(startIndex, endIndex);

		/* 添加到列表 */
		stringList->addObject(lineStr);

		/* 截取剩下的字符串 */
		str->m_sString = str->m_sString.substr(endIndex + 1, size);

		endIndex = str->m_sString.find(strSymbol);
	}

	/* 剩下的字符串也添加进列表 */
	//if(str->m_sString.compare("") != 0)
	//{
		stringList->addObject(CCString::create(str->m_sString));
	//}

	return stringList;
}
string& strReplace(string& str,const string&old_value,const string& new_value)
{
	for(int pos=0;pos!=-1;pos+=new_value.length())
		if((pos=str.find(old_value,pos))!=-1)
			str.replace(pos,old_value.length(),new_value);
		else break;
		return str;
}
string& strRemoveSpace(string& str)
{
	string::iterator new_end = remove_if(str.begin(), str.end(), bind2nd(equal_to<char>(), ' '));
	str.erase(new_end, str.end());
	return str;
}
