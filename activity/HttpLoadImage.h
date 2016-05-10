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
	
	//�󶨻ص�
	void bindUiTarget(HttpLoadImageDelegate* pTarget);

	//��ȡ����ͼƬ��URL, �����Ϣ
    void requestUrlImage(string sUrl, const char* sTag);
    
	//ͼƬ�������ص�
    void onHttpRequestRptImg(cocos2d::extension::CCHttpClient *sender, cocos2d::extension::CCHttpResponse *response);

    //��ԭ��Ĭ��ͷ�����������ͼƬ��С
    static void setImgSize(int oldhead_w,int oldhead_h,CCSprite* sprite);
    
    //����ͼƬ����ԭ��ͼƬ
    static void CreateImg(CCSprite* tagSprite,CCSprite* oldSprite,CCTexture2D* texture,int newTag);

	//����ͼƬ���þ���
	CCSprite* SetSpriteTextureWithLocalFile( string sLocalFile, CCSprite* pSprite );

	HttpLoadImageDelegate* getTarget();
	std::string getStoragePath(const char* downDir, const char* fileName);
private:
	static HttpLoadImage* m_instance;
	HttpLoadImageDelegate* m_pTarget;
};

#endif /* HttpGetImg */