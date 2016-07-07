#pragma once

/******************************************************
*文件名称:	CDownloadPackageDelegate.h
*编写日期:	2016-6-14-11:50
*编写作者:	YPF
*功能描述:	拆分包下载完后的回调接口
*******************************************************/

class DownloadPackageDelegate
{
public:
	virtual void downloadPackageEnd(bool bAnswer) = 0;
};