/******************************************************************
»Ò¶ÈÂË¾µ
*******************************************************************/

#ifndef __CCFILTER_GRAY__H__
#define __CCFILTER_GRAY__H__

#include "CCFilter.h"

class CCSpriteWidthFilter;

class CCGrayFilter : public CCFilter
{
public:
	static CCGrayFilter* create();
	static CCGrayFilter* create(ccColor4F color);
	static CCGrayFilter* create(float r, float g, float b, float a = 1.0f);

	CCGrayFilter();

	void setColor(ccColor4F color);
	void setColor(float r, float g, float b, float a);
	void initSprite(CCSpriteWidthFilter* sprite);
	void draw();
protected:
	virtual CCGLProgram* loadShader();
	virtual void setAttributes(CCGLProgram* program);
	virtual void setUniforms(CCGLProgram* program);

private:
	ccColor4F m_param;
};

#endif