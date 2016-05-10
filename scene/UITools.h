#ifndef __CCTOOLS_H__
#define __CCTOOLS_H__

#include <stdio.h>
#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;
#include "SimpleAudioEngine.h"
#include "VirtualScreen.h"

CCRenderTexture* CaptureScreen(CCNode* target);
CCSprite* CaptureScreenSprite(CCNode* target);

int HexToDec(const char* s);
ccColor3B GetColorFromXML( const char* xml );

void SetNodePosition( CCNode* node, float x, float y );
void SetNodePosition( CCNode* node, CCPoint pos );
CCPoint UI2GL( float x, float y, float w, float h, float ax = 0.5f, float ay = 0.5f );
CCPoint CCSizeCenter(CCSize size);
const CCPoint CCPointCenter = CCPointMake(0.5f,0.5f);

void AddDebugPoint(CCNode* parent, CCPoint pos);

char* ToString(int i);
char* ToString(unsigned int i); 
char* ToString(float f);
char* ToString(long long ll);
std::string& toStrString(int i);
const char* FormatChip(long long ll, bool dollar = false);
const char* FormatChipAtlas(long long ll);
const char* ShortName(const char* src);
const char* SubString(const char* s, int offset, const char* startString, const char* endString );
void SetSpirteTexture(const char* image, CCSprite* sprite, float width, float height);

void Url2Filename(char* url);

const char* U8(const char* inbuf);
size_t strlen_utf8(const char* s);

char* b5tu(const char *inbuf); 

#endif  // __CCTOOLS_H__