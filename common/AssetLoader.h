//
//  AssetLoader.h
//
//

#ifndef __ResMgr__AssetLoader__
#define __ResMgr__AssetLoader__

#include "cocos2d.h"
#include "Resources.h"
using namespace cocos2d;

class AssetLoader: public CCObject{
public:
    static CCSprite* createSprite(const std::string& name);
    
private:
    static AssetLoader* getInstance();
    bool init();
    
    bool fileExists(const std::string& filename);
    std::string getTexturePlist(const std::string& name);
    std::string getTextureImage(const std::string& name);
    
private:
    CCDictionary* _fileDict;              // 文件列表
    CCDictionary* _texturePlistDict;           // 打包资源到文件的映射
    CCDictionary* _textureImageDict;       // 打包资源plist 到图片的映射
};

#endif /* defined(__ResMgr__AssetLoader__) */
