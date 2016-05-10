/******************************************************************
            Copyright (c) 2013, dreamfairy
                   All rights reserved
         
    �������ڣ�  2014��7��1��   15ʱ44��
    �ļ����ƣ�  CCColorFlashFilter.h
    ˵    ����  ��ɫ��˸
    
    ��ǰ�汾��  1.00
    ��    �ߣ�  �԰׵ļ�
    ��    ����      

*******************************************************************/

#ifndef __COLORFLASHFILTER_H__
#define __COLORFLASHFILTER_H__

#include "CCFilter.h"

class CCSpriteWidthFilter;

class CCColorFlashFilter : public CCFilter
{
public:
	static CCColorFlashFilter* create();
	static CCColorFlashFilter* create(float interval, float r, float g, float b);

	CCColorFlashFilter();

	void setColor(float r, float g, float b);
	void setIntervalFrame(float interval);
	void initSprite(CCSpriteWidthFilter* sprite);
	void draw();
protected:
	virtual CCGLProgram* loadShader();
	virtual void setAttributes(CCGLProgram* program);
	virtual void setUniforms(CCGLProgram* program);
private:
	float m_count;
	float m_interval;
	ccColor4F m_color; 
	float m_percent;
};

#endif // !__COLORFLASHFILTER_H__
