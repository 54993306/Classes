//
//  AssetLoader.cpp
//  ResMgr
//
//
//

#include "AssetLoader.h"
#include "Resources.h"
//#include "CocoStudio/Json/rapidjson/reader.h"
//#include "cocostudio/Json/DictionaryHelper.h"
#include "jsonCpp/json.h"

//using namespace rapidjson;
using namespace Json;

CCSprite* AssetLoader::createSprite(const std::string& name){
    if (AssetLoader::getInstance()->fileExists(name)){
        return CCSprite::create(name.c_str());
    }
    CCLOG("create sprite: %s", name.c_str());
    std::string plistfile = AssetLoader::getInstance()->getTexturePlist(name);
    std::string imagefile = AssetLoader::getInstance()->getTextureImage(name);
    CCLOG("plist: %s, image: %s", plistfile.c_str(), imagefile.c_str());
    if (plistfile != "" && imagefile != ""){
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(plistfile.c_str(), imagefile.c_str());
        return CCSprite::createWithSpriteFrameName(name.c_str());
    }
    return nullptr;
}

AssetLoader* AssetLoader::getInstance(){
    static AssetLoader* m_assetLoader = nullptr;
    if (m_assetLoader == nullptr){
        m_assetLoader = new AssetLoader();
        m_assetLoader->init();
    }
    return m_assetLoader;
}

bool AssetLoader::init()
{
    // 初始化 文件字典
   // CCDictionary *jsonDict = new CCDictionary();
	//JsonCCDictionary *jsonDict = new JsonCCDictionary();
/*
	unsigned long size;
	const char* data = (const char *)CCFileUtils::sharedFileUtils()->getFileData(sjs_file_list,"r",&size);
	/*
	Reader reader;
	Value jsonDict;
	reader.Parse<kParseDefaultFlags,unsigned char*,Value>(data,jsonDict);
	
	Document doc;
	doc.Parse<kParseDefaultFlags>(data);
	

    //DictionaryHelper* dicHelper = DICTOOL;

    _fileDict = CCDictionary::create();

	Reader reader;
	Value value;
	reader.parse(data,value);
	
	Value valName = value["file_name"];
	Value valIndex = value["file_index"];
	for (int i=0; i<valName.size(); i++)
	{
		std::string name = valName[i].asString();
		std::string index = valIndex[i].asString();
		_fileDict->setObject(CCString::create(index), name);
	}
	CCLOG("file count: %d", valName.size());
	

	_texturePlistDict = CCDictionary::create();

	Value valTexture = value["texture_name"];
	Value valPlist = value["texture_plist"];
	int texture_idx = valTexture.size();
	for (int i = 0; i < texture_idx; i++){
		std::string name = valTexture[i].asString();
		std::string plist = valPlist[i].asString();
		_texturePlistDict->setObject(CCString::create(plist), name);
	}
	CCLOG("texture count: %d", texture_idx);

	_textureImageDict = CCDictionary::create();
	Value valImage = value["texture_image"];
	for (int i = 0; i < texture_idx; i++){
		std::string name = valTexture[i].asString();
		std::string image = valImage[i].asString();
		_textureImageDict->setObject(CCString::create(image), name);
	}  
    */
    return true;
}

bool AssetLoader::fileExists(const std::string &filename){
    return _fileDict->objectForKey(filename) != nullptr;
}

std::string AssetLoader::getTexturePlist(const std::string &name){
    CCObject* objPd = _texturePlistDict->objectForKey(name);
    if (objPd != nullptr){
        std::string tt_idx = dynamic_cast<CCString*>(objPd)->getCString();
        CCDictElement* de = nullptr;
        CCDICT_FOREACH(_fileDict, de){
            if (de != nullptr){
                CCString* str = dynamic_cast<CCString*>(de->getObject());
                if (str->getCString() == tt_idx){
                    return de->getStrKey();
                }
            }
        }
    }
    return "";
}

std::string AssetLoader::getTextureImage(const std::string &name){
    CCObject* obj = _textureImageDict->objectForKey(name);
    if (obj != nullptr){
        std::string image = dynamic_cast<CCString*>(obj)->getCString();
        CCDictElement* de = nullptr;
        CCDICT_FOREACH(_fileDict, de){
            if (de != nullptr){
                CCString* str = dynamic_cast<CCString*>(de->getObject());
                if (str->getCString() == image){
                    return de->getStrKey();
                }
            }
        }
    }
    return "";
}
