#ifndef _CSV_UTILS_H_
#define _CSV_UTILS_H_
#include "cocos2d.h"
#include <string>
#include <map>
using namespace std;
USING_NS_CC;
class FileObject : public CCObject
{
public:
	FileObject()
		:m_data(nullptr)
		,m_size(0)
		,m_fileName(""){};
	virtual ~FileObject();
public:
	inline string getFileName(){return m_fileName;};
	inline unsigned long getSize(){return m_size;}
protected:
	unsigned char* m_data;
	unsigned long m_size;
	string m_fileName;
};
class CSVFile : public FileObject
{
public:
	CSVFile()
		:m_arr(nullptr){};
	virtual ~CSVFile();
public:
	//获取行列值
	const char* get(int nRow, int nCol);
	static CSVFile* create(const char* fileName,unsigned char* data,unsigned long size);
	//获取配置文件数据的行和列数量
	void getFileRowColNum(int& row,int& col);
	int getRowNum();
	int getColNum();
protected:
	CCArray* m_arr;
};
class FileUtils : public CCObject
{
public:
	FileUtils(){};
	virtual ~FileUtils();
public:
	//获取工具对象
	static FileUtils* sharedFileUtils();
	//加载配置文件
	CSVFile* loadCSVFile(const char* strPath);
	//释放配置文件
	void releaseFile(const char* strPath);
	FileObject* getFile(const char* strPath);
protected:
	void addFile(FileObject* file);
protected:
	map<string,FileObject*> m_fileList;
};
#endif // !_CSV_UTILS_H_
