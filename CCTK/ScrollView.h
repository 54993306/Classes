/****************************************************************************
Copyright (c) 2014 Lijunlin - Jason lee

Created by Lijunlin - Jason lee on 2014

jason.lee.c@foxmail.com
http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#ifndef __CCWIDGET_SCROLLVIEW_H__
#define __CCWIDGET_SCROLLVIEW_H__

#include "cocos2d.h"
#include "WidgetMacros.h"
#include "Widget.h"
#include "Layout.h"
#include "WidgetProtocol.h"

NS_CC_WIDGET_BEGIN

class CScrollView;
class CScrollViewContainer;

/**
 * enum   : CScrollViewDirection
 * author : Jason lee
 * email  : jason.lee.c@foxmail.com
 * descpt : direction of scroll view
 */
enum CScrollViewDirection
{
	eScrollViewDirectionHorizontal,
	eScrollViewDirectionVertical,
	eScrollViewDirectionBoth
};

/**
 * class  : CScrollViewContainer
 * author : Jason lee
 * email  : jason.lee.c@foxmail.com
 * descpt : scroll view container define
 */
class CScrollViewContainer : public CLayout
{
public:
	virtual void reset();
	friend class CScrollView;
};

/**
 * class  : CScrollView
 * author : Jason lee
 * email  : jason.lee.c@foxmail.com
 * descpt : scroll view define
 */
class CScrollView : public CLayout, public CScrollableProtocol
{
public:
	CScrollView();
	virtual ~CScrollView();
	virtual bool init();
	static CScrollView* create(const CCSize& contentSize);			//创建
	virtual bool initWithSize(const CCSize& tSize);					//根据size初始化控件大小
	void setContainerSize(CCSize tSize);							//内容大小
	const CCSize& getContainerSize() const;							
	CScrollViewDirection getDirection();							//得到方向(类型)
	void setDirection(CScrollViewDirection eDirection);				
	CScrollViewContainer* getContainer();							//得到内容节点

	virtual void setContentSize(const CCSize& contentSize);			//显示区域
	const CCPoint& getContentOffset() const;						//得到内容的偏移量
	void setBounceable(bool bBounceable);							//设置反弹
	bool isBounceable();
	void setDeaccelerateable(bool bDeaccelerateable);				//重力感应
	bool isDeaccelerateable();
	void setDragable(bool bDragable);								//是否可以拖动
	bool isDragable();
    bool isTouchMoved();											//是否在移动
    void relocateContainer();										//重置
	void stopContainerAnimation();									//停止内容动画

	const CCPoint& getMaxOffset() const;							//得到偏移量的边界值
	const CCPoint& getMinOffset() const;
	void setContentOffsetToTop();									//上
	void setContentOffsetToBottom();								//下
	void setContentOffsetToLeft();									//左
	void setContentOffsetToRight();									//右
	void setContentOffset(CCPoint tOffset);							//设置偏移tOffset
	void setContentOffsetToTopInDuration(float fDuration);			//经过fDuration时间移动到顶端
	void setContentOffsetToTopEaseIn(float fDuration, float fRate);	//经过fDuration以fRate比率移动到顶端
	void setContentOffsetInDuration(CCPoint tOffset, float fDuration);			//fDuration时间偏移到tOffset
	void setContentOffsetEaseIn(CCPoint tOffset, float fDuration, float fRate); //经过fDuration时间以fRate比率偏移到tOffset

public:
	virtual void visit();
    virtual void onExit();
	virtual CWidgetTouchModel onTouchBegan(CCTouch *pTouch);
    virtual void onTouchMoved(CCTouch *pTouch, float fDuration);
    virtual void onTouchEnded(CCTouch *pTouch, float fDuration);
    virtual void onTouchCancelled(CCTouch *pTouch, float fDuration);

protected:
	void relocateContainerWithoutCheck(const CCPoint& tOffset);			//重置
	void setContentOffsetWithoutCheck(const CCPoint& tOffset);			//偏移至tOffset不进行边界检查
	void setContentOffsetInDurationWithoutCheck(const CCPoint& tOffset, float fDuration);	//fDuration时间偏移至tOffset不进行边界检查
	void setContentOffsetEaseInWithoutCheck(const CCPoint& tOffset, float fDuration, float fRate);

	bool m_bDeaccelerateScrolling;
	void perpareDeaccelerateScroll();				//准备减速
	void performedDeaccelerateScrolling(float dt);	//执行减速卷动
	void stoppedDeaccelerateScroll();

	bool m_bAnimatedScrolling;
	void perpareAnimatedScroll();
    void performedAnimatedScrolling(float dt);
    void stoppedAnimatedScroll();

    virtual void onScrolling(){};
	virtual void onDeaccelerateScrollEnded(){};
	virtual void onDraggingScrollEnded(){};
	void updateLimitOffset();							//更新边界偏移量
	bool validateOffset(CCPoint& tPoint);				//检查是否超出边界

protected:
	CScrollViewContainer *m_pContainer;
	CScrollViewDirection m_eDirection;
	CCPoint m_tLastMovePoint;
	bool m_bDeaccelerateable;
	bool m_bBounceable;
	bool m_bDragging;
	bool m_bDragable;
	float m_fDragSpeed;
	CCPoint m_tScrollDistance;
	CCPoint m_tTouchBeganPoint;
    bool m_bTouchMoved;
	CCPoint m_tMinOffset;
	CCPoint m_tMaxOffset;
};

NS_CC_WIDGET_END

#endif //__CCWIDGET_SCROLLVIEW_H__