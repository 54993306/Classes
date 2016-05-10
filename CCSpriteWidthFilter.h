
#ifndef __CCspriteWithFilter__
#define __CCspriteWithFilter__

#include "cocos2d.h"
#include "filter/CCFilter.h"

class CCFilter;

USING_NS_CC;

class CCSpriteWidthFilter : public CCSprite
{
public:
	CCSpriteWidthFilter();
	~CCSpriteWidthFilter();

	static CCSpriteWidthFilter* create();
	static CCSpriteWidthFilter* create(const char* $pszFileName);
	static CCSpriteWidthFilter* create(const char* $pszFileName, const CCRect& $rect);

	static CCSpriteWidthFilter* createWithTexture(CCTexture2D* $pTexture);
	static CCSpriteWidthFilter* createWithTexture(CCTexture2D* $pTexture, const CCRect& rect);

	static CCSpriteWidthFilter* createWithSpriteFrame(CCSpriteFrame* $pSpriteFrame);

	static CCSpriteWidthFilter* createWithSpriteFrameName(const char* $pszSpriteFrameName);


	virtual void draw(void);
	virtual CCFilter* getFilter(unsigned int index = 0);
	virtual void setFilter(CCFilter* filter);

	virtual CCArray* getFilters();
	virtual void setFilters(CCArray* filters);

	virtual void clearFilter();

protected:
	virtual void updateDefaultShader();
	virtual void drawFilter();
	virtual bool updateFilters();

	CCArray* m_filters;
};

#endif
