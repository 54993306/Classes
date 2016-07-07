#pragma once

/******************************************************
*文件名称:	ActivityData.h
*编写日期:	2016-6-14-11:35
*编写作者:	YPF
*功能描述:	活动数据
*******************************************************/

#include "protos/protocol.h"
#include "protos/act_protocol.pb.h"

#define activity_select_panel "activity/button_event_on.png"
#define activity_unselect_panel "activity/button_event_off.png"
#define default_activity_show_sprite "activity/pattern_64_2.png"
#define default_activity_icon_sprite "activity/eventtitle_001.png"

enum ActivityTabType
{
	ActivityTabBattle = 0,			//战斗活动
	ActivityTabSale,					//新品上架
	ActivityTabMax					
};

enum ActivityStatus
{
	ActivityTabSale_under_way = 1,			//正在进行
	ActivityStatus_will_start,							//即将开始
	ActivityTabSale_end								//结束			
};

enum ActivityLoadImageTag
{
	ActivityLoadImageTagIcon,
	ActivityLoadImageTagInfo
};

const static char* activity_cell[ActivityTabMax] = {"Cell_battle", "Cell_battle"};
const static char* activity_scroll[ActivityTabMax] = {"scroll_battle", "scroll_sale"};

const static char* activity_status_pic[3] = {"activity/word_starting.png", "activity/word_ready.png", "activity/word_end.png"};

struct CActivity
{
	int			iActId	;			//活动ID
	int			iType;				//活动类型
	string		sName ;			//活动名称
	int			iStatus ;			//活动状态
	string		sIconFile;		//活动图标
	string		sInfoFile;		//描述图片
	int			iExtraParam;				//其他参数，可能是价格
	google::protobuf::RepeatedPtrField< ::protos::ExList >	 exlist;				//额外列表数据，用于收集活动
	void read(const Activity& activity);
};