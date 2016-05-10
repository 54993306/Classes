#include "HttpLoadImage.h"

#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
#include <dirent.h>
#include <sys/stat.h>
#endif

HttpLoadImage* HttpLoadImage::m_instance = nullptr;

HttpLoadImage* HttpLoadImage::getInstance()
{
	if(m_instance==nullptr)
	{
		m_instance = new HttpLoadImage;
	}
	return m_instance;
}


HttpLoadImage::HttpLoadImage(void)
{
    m_pTarget = nullptr;
}


HttpLoadImage::~HttpLoadImage(void)
{

}

void HttpLoadImage::requestUrlImage( string sUrl, const char* sTag )
{
	CCHttpRequest* request = new CCHttpRequest();
	// required fields
	request->setUrl(sUrl.c_str());
	request->setRequestType(CCHttpRequest::kHttpGet);
	request->setResponseCallback(this, httpresponse_selector(HttpLoadImage::onHttpRequestRptImg));
	
	// optional fields
	request->setUserData(m_pTarget);
	request->setTag(sTag);
	CCHttpClient::getInstance()->send(request);
	request->release();
}

void HttpLoadImage::onHttpRequestRptImg(CCHttpClient *sender, CCHttpResponse *response)
{
	if (!response)
		return;
    string str_tag = response->getHttpRequest()->getTag();
    // You can get original request type from: response->request->reqType
    if (0 != strlen(response->getHttpRequest()->getTag()))
    {
        //CCLog("%s completed", response->getHttpRequest()->getTag());
    }
    
    if (!response->isSucceed())
    {
        CCLog("response failed");
        CCLog("error buffer: %s", response->getErrorBuffer());
        return;
    } 
    vector<char> *buffer = response->getResponseData();  
	//是否绑定了m_pTarget
	if(m_pTarget != nullptr)
	{
		//只回调当前绑定的UI target
		if(m_pTarget == response->getHttpRequest()->getUserData())
		{
			m_pTarget->imageLoadSuccessCallBack(response->getHttpRequest()->getTag(), buffer);
		}
	}
}

void HttpLoadImage::setImgSize(int oldhead_w,int oldhead_h,CCSprite* sprite)
{
    float s_X=oldhead_w/sprite->getContentSize().width;
    float s_Y=oldhead_h/sprite->getContentSize().height;
    sprite->setScaleX(s_X);
    sprite->setScaleY(s_Y);
}

void HttpLoadImage::CreateImg(CCSprite* tagSprite,CCSprite* oldSprite,CCTexture2D* texture,int newTag)
{
    int oldw =oldSprite->getContentSize().width;
    int oldh =oldSprite->getContentSize().height;
    if(texture!=NULL)
    {
        CCSprite* sprite = CCSprite::createWithTexture(texture);
        //按原比例设置
        setImgSize(oldw,oldh,sprite);
        sprite->setPosition(oldSprite->getPosition());
        tagSprite->addChild(sprite,1,newTag);
        texture->release();
    }
}

CCSprite* HttpLoadImage::SetSpriteTextureWithLocalFile( string sLocalFile, CCSprite* pSprite )
{
	sLocalFile = CCFileUtils::sharedFileUtils()->fullPathForFilename(sLocalFile.c_str());
	bool isFileExist = CCFileUtils::sharedFileUtils()->isFileExist(sLocalFile);
	if(!isFileExist)
	{
		CCLOG("Not exist: %s", sLocalFile.c_str());
		return nullptr;
	}
	CCSprite* sp =CCSprite::create(sLocalFile.c_str());
	if(sp)
	{
		sp->setPosition(pSprite->getPosition());
		sp->setAnchorPoint(pSprite->getAnchorPoint());
		sp->setZOrder(pSprite->getZOrder());
		pSprite->getParent()->addChild(sp);
		pSprite->removeFromParentAndCleanup(true);
		return sp;
	}

	return nullptr;
}

void HttpLoadImage::destroyInstance()
{
	CC_SAFE_DELETE(m_instance);
	m_instance = nullptr;
}

void HttpLoadImage::bindUiTarget( HttpLoadImageDelegate* pTarget )
{
	m_pTarget = pTarget;
}

HttpLoadImageDelegate* HttpLoadImage::getTarget()
{
	return m_pTarget;
}

std::string HttpLoadImage::getStoragePath(const char* downDir, const char* fileName)
{
	std::string pathToSave = CCFileUtils::sharedFileUtils()->getWritablePath() + downDir;
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
	DIR *pDir = NULL;
	pDir = opendir(pathToSave.c_str());
	if (!pDir)
	{
		mkdir(pathToSave.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
	}
#else
	if ((GetFileAttributesA(pathToSave.c_str())) == INVALID_FILE_ATTRIBUTES)
	{
		CreateDirectoryA(pathToSave.c_str(), 0);
	}
#endif
	pathToSave += "/";
	return pathToSave + fileName;
}