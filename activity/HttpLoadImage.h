#ifndef __HttpGetImg__
#define __HttpGetImg__

#include "cocos2d.h"
#include "network/HttpRequest.h"
#include "network/HttpClient.h"

USING_NS_CC_EXT;
using namespace std;
USING_NS_CC;

class HttpLoadImageDelegate
{
public:
	virtual void imageLoadSuccessCallBack(string sTag, vector<char>* pBuffer) = 0;
};

class HttpLoadImage : CCObject
{
public:
    HttpLoadImage();
    ~HttpLoadImage();
    
	static HttpLoadImage* getInstance();

	static void destroyInstance();
	
	//绑定回调
	void bindUiTarget(HttpLoadImageDelegate* pTarget);

	//获取网络图片，URL, 标记信息
    void requestUrlImage(string sUrl, const char* sTag);
    
	//图片数据流回调
    void onHttpRequestRptImg(cocos2d::extension::CCHttpClient *sender, cocos2d::extension::CCHttpResponse *response);

    //按原来默认头像比例设置新图片大小
    static void setImgSize(int oldhead_w,int oldhead_h,CCSprite* sprite);
    
    //创建图片覆盖原有图片
    static void CreateImg(CCSprite* tagSprite,CCSprite* oldSprite,CCTexture2D* texture,int newTag);

	//根据图片设置精灵
	CCSprite* SetSpriteTextureWithLocalFile( string sLocalFile, CCSprite* pSprite );

	HttpLoadImageDelegate* getTarget();
	std::string getStoragePath(const char* downDir, const char* fileName);
private:
	static HttpLoadImage* m_instance;
	HttpLoadImageDelegate* m_pTarget;
};

#endif /* HttpGetImg */