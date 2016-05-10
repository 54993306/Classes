#pragma once

class DownloadPackageDelegate
{
public:
	virtual void downloadPackageEnd(bool bAnswer) = 0;
};