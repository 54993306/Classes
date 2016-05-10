

#ifndef __FLASHFILETE_H__
#define __FLASHFILETE_H__

#include "CCFilter.h"

class CCSpriteWidthFilter;

class CCFlashFilter : public CCFilter
{
public:
	static CCFlashFilter* create();
	static CCFlashFilter* create(int intervalFrame);

	CCFlashFilter();

	void setIntervalFrame(int intervalFrame);
	void initSprite(CCSpriteWidthFilter* sprite);
	void draw();
protected:
	virtual CCGLProgram* loadShader();
	virtual void setAttributes(CCGLProgram* program);
	virtual void setUniforms(CCGLProgram* program);

private:
	int m_currentFrameCount;
	int m_frameInterval;
	float m_alpha;
};

#endif