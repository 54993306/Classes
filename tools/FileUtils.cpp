#include "FileUtils.h"
#include "StringUtil.h"
FileObject::~FileObject()
{
	if(m_data)
	{
		delete m_data;
	}
	m_data = nullptr;
}
CSVFile::~CSVFile()
{
	CC_SAFE_RELEASE(m_arr);
	m_arr = nullptr;
}
//获取行列值
const char* CSVFile::get(int nRow, int nCol)
{

	//数据行数
	int nRowNum = {0}, nColNum = {0};
	getFileRowColNum(nRowNum,nColNum);
	//下标越界
	if (nRow >= nRowNum || nCol >= nColNum || nRow >= int(m_arr->count()))
	{
		return "";
	}

	//获取第nRow行数据
	CCObject* pRowObj = m_arr->objectAtIndex(nRow);
	if (pRowObj != NULL)
	{
		CCArray* rowArr = (CCArray*)pRowObj;

		//获取第nCol列数据
		CCObject* pColObj = rowArr->objectAtIndex(nCol);
		if (pColObj != NULL)
		{
			CCString* colStr = (CCString*)pColObj;
			return colStr->getCString();
		}
	}
	return "";
}
CSVFile* CSVFile::create(const char* fileName,unsigned char* data,unsigned long size)
{
	CSVFile* file = nullptr;
	do 
	{
		if(!fileName || fileName == "" || !data || size <=0 ) break;
		file = new CSVFile();
		if(!file) break;
		CCString* pStr = CCString::createWithData(data, size);
		CCArray* linesList = strSplit(pStr->getCString(),"\n");

		CCObject* pObj = NULL;
		CCString* tString = NULL;
		CCArray* csvStrList = CCArray::create();

		CCARRAY_FOREACH(linesList, pObj)
		{
			tString = (CCString*)pObj;
			if (tString && tString->m_sString != "")
			{
				CCArray* arr = strSplit(tString->getCString(), ",");
				csvStrList->addObject(arr);
			}
		}
		file->m_fileName = fileName;
		file->m_size = size;
		csvStrList->retain();
		file->m_arr = csvStrList;
		file->autorelease();
	} while (0);
	return file;
}
//获取配置文件数据的行和列数量
void CSVFile::getFileRowColNum(int& row,int& col)
{
	//数据行数
	row = m_arr->count(),col = 0;
	int nColNum = 0;
	if (row > 0)
	{
		//获取第0行数据
		CCObject *pRowObj = m_arr->objectAtIndex(0);
		if (pRowObj != NULL)
		{
			CCArray* rowArr = (CCArray*)pRowObj;
			//数据列数
			col = rowArr->count();
		}
	}
}
int CSVFile::getRowNum()
{
	int row,col;
	getFileRowColNum(row,col);
	return row;
}
int CSVFile::getColNum()
{
	int row,col;
	getFileRowColNum(row,col);
	return col;
}
FileUtils::~FileUtils()
{
	for(map<string,FileObject*>::iterator iter = m_fileList.begin(); iter != m_fileList.end();++iter)
	{
		iter->second->release();
	}
	m_fileList.clear();
}
//获取工具对象
FileUtils* FileUtils::sharedFileUtils()
{
	static FileUtils* m_fileUtils = nullptr;
	if(!m_fileUtils) m_fileUtils = new FileUtils();
	return m_fileUtils;
}
//加载配置文件
CSVFile* FileUtils::loadCSVFile(const char* strPath)
{
	CSVFile* file = nullptr;
	do 
	{
		if(!strPath || strPath == "") break;
		unsigned long pSize = 0;
		unsigned char* chDatas = CCFileUtils::sharedFileUtils()->getFileData(strPath, "r", &pSize);
		if(!chDatas) break;
		file = CSVFile::create(strPath,chDatas,pSize);
		delete[] chDatas;
		if(!file) break;
		addFile(file);
	} while (0);
	return file;
}
//释放配置文件
void FileUtils::releaseFile(const char* strPath)
{
	if(!strPath || strPath == "" ) return;
	map<string,FileObject*>::iterator iter = m_fileList.find(strPath);
	if(iter != m_fileList.end())
	{
		iter->second->release();
		m_fileList.erase(iter);
	}
}
void FileUtils::addFile(FileObject* file)
{
	if(!file) return;
	if(getFile(file->getFileName().c_str()))
	{
		return;
	}
	file->retain();
	m_fileList[file->getFileName()] = file;
}
FileObject* FileUtils::getFile(const char* strPath)
{
	if(!strPath || strPath == "" ) return nullptr;
	map<string,FileObject*>::iterator iter = m_fileList.find(strPath);
	if(iter != m_fileList.end())
	{
		return iter->second;
	}
	return nullptr;
}