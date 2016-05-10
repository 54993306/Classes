#ifndef _AUTO_RESOURCES_H_
#define _AUTO_RESOURCES_H_
/************************************************************************/
/* 
*
*
*				//  [12/22/2015 Lin_Xiancheng]
*					定义常用资源
*				//应该封装成一个配置文件来进行处理的。
*
*
*/
/************************************************************************/

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32				//在win32下用MP3格式进行调试
//const char SFX_[] = "SFX/.ogg";//
const char MainSkill[]          = "music/mainskill.mp3";	//播放主动技能时的音效缺失

const char BGM_Arena[]			= "BGM/Test/100.mp3";			//竞技场背景音乐	
const char BGM_Battle[]			= "BGM/Test/101.mp3";			//战斗
const char BGM_LoadWar[]		= "BGM/Test/102.mp3";			//资源加载背景音乐
const char BGM_Login[]			= "BGM/Test/103.mp3";			//登陆界面
const char BGM_MainCity[]		= "BGM/Test/104.mp3";			//主城背景音乐
const char BGM_Talk1[]          = "BGM/Test/105.mp3";			//剧情
const char BGM_Talk2[]          = "BGM/Test/106.mp3";			//剧情2
const char BGM_Talk3[]          = "BGM/Test/107.mp3";			//剧情3
const char BGM_Boss1[]			= "BGM/Test/108.mp3";			//boss批次背景音乐
const char BGM_Boss2[]          = "BGM/Test/109.mp3";			//普通BOSS音乐
const char BGM_Boss3[]          = "BGM/Test/110.mp3";			//章节BOSS音乐
const char BGM_Boss_Warning[]          = "BGM/Test/111.mp3";			//BOSS-WARNING
const char BGM_Prologue[]          = "BGM/Test/112.mp3";			//序章

const char SFX_Button[]			= "SFX/Test/200.mp3";			//按钮触摸
const char SFX_LoginClick[]     = "SFX/Test/201.mp3";			//登陆成功
const char SFX_Win[]			= "SFX/Test/202.mp3";			//战斗胜利
const char SFX_Cancel[]			= "SFX/Test/203.mp3";			//取消音效
const char SFX_Ensure[]			= "SFX/Test/204.mp3";			//确定音效
const char SFX_Fail[]			= "SFX/Test/205.mp3";			//战斗失败

const char SFX_401[] = "SFX/Test/401.mp3";//寶箱跌落
const char SFX_402[] = "SFX/Test/402.mp3";//掉道具
const char SFX_403[] = "SFX/Test/403.mp3";//功能開放
const char SFX_404[] = "SFX/Test/404.mp3";//经验增加1
const char SFX_405[] = "SFX/Test/405.mp3";//经验增加2
const char SFX_406[] = "SFX/Test/406.mp3";//结算星星
const char SFX_407[] = "SFX/Test/407.mp3";//獲得道具
const char SFX_408[] = "SFX/Test/408.mp3";//獲得金錢1
const char SFX_409[] = "SFX/Test/409.mp3";//獲得金錢2
const char SFX_410[] = "SFX/Test/410.mp3";//獲得靈魂
const char SFX_411[] = "SFX/Test/411.mp3";//僵屍聲音1
const char SFX_412[] = "SFX/Test/412.mp3";//僵屍聲音2
const char SFX_413[] = "SFX/Test/413.mp3";//接受任務
const char SFX_414[] = "SFX/Test/414.mp3";//金錢掉落
const char SFX_415[] = "SFX/Test/415.mp3";//金錢連續結算
const char SFX_416[] = "SFX/Test/416.mp3";//進入界面音效1
const char SFX_417[] = "SFX/Test/417.mp3";//進入界面音效2
const char SFX_418[] = "SFX/Test/418.mp3";//進入界面音效3
const char SFX_419[] = "SFX/Test/419.mp3";//警告音效1
const char SFX_420[] = "SFX/Test/420.mp3";//警告音效2
const char SFX_421[] = "SFX/Test/421.mp3";//酒馆抽奖(拉门）
const char SFX_422[] = "SFX/Test/422.mp3";//酒馆抽奖（灵魂飞出）
const char SFX_423[] = "SFX/Test/423.mp3";//靈魂掉落
const char SFX_424[] = "SFX/Test/424.mp3";//生產物品
const char SFX_425[] = "SFX/Test/425.mp3";//石版
const char SFX_426[] = "SFX/Test/426.mp3";//提升等級
const char SFX_427[] = "SFX/Test/427.mp3";//铁匠铺强化
const char SFX_428[] = "SFX/Test/428.mp3";//铁匠铺洗练
const char SFX_429[] = "SFX/Test/429.mp3";//拖動音效
const char SFX_430[] = "SFX/Test/430.mp3";//完成任務
const char SFX_431[] = "SFX/Test/431.mp3";//武将进阶
const char SFX_432[] = "SFX/Test/432.mp3";//武将升级
const char SFX_433[] = "SFX/Test/433.mp3";//武将突破
const char SFX_434[] = "SFX/Test/434.mp3";//武将突破
const char SFX_435[] = "SFX/Test/435.mp3";//武将突破
const char SFX_436[] = "SFX/Test/436.mp3";//武将突破

const char SFX_500[] = "SFX/Test/500.mp3";//拳击(呯)
const char SFX_501[] = "SFX/Test/501.mp3";//爆炸(坠落)
const char SFX_502[] = "SFX/Test/502.mp3";//枪声1(击中多层铁板声)
const char SFX_503[] = "SFX/Test/503.mp3";//必杀技闪光
const char SFX_504[] = "SFX/Test/504.mp3";//必殺過場音效
const char SFX_505[] = "SFX/Test/505.mp3";//风吹
const char SFX_506[] = "SFX/Test/506.mp3";//火枪
const char SFX_507[] = "SFX/Test/507.mp3";//砍击(微重)
const char SFX_508[] = "SFX/Test/508.mp3";//砍擊(重)
const char SFX_509[] = "SFX/Test/509.mp3";//受擊(啪)
const char SFX_510[] = "SFX/Test/510.mp3";//咬击
const char SFX_511[] = "SFX/Test/511.mp3";//咬擊(浊)
const char SFX_512[] = "SFX/Test/512.mp3";//隕石掉落
const char SFX_513[] = "SFX/Test/513.mp3";//增益BUFF(補血)
const char SFX_514[] = "SFX/Test/514.mp3";//子彈命中聲音
const char SFX_515[] = "SFX/Test/515.mp3";
const char SFX_516[] = "SFX/Test/516.mp3";
const char SFX_517[] = "SFX/Test/517.mp3";
const char SFX_518[] = "SFX/Test/518.mp3";
const char SFX_519[] = "SFX/Test/519.mp3";
const char SFX_520[] = "SFX/Test/520.mp3";
const char SFX_521[] = "SFX/Test/521.mp3";
const char SFX_522[] = "SFX/Test/522.mp3";
const char SFX_523[] = "SFX/Test/523.mp3";
const char SFX_524[] = "SFX/Test/524.mp3";
const char SFX_525[] = "SFX/Test/525.mp3";
const char SFX_526[] = "SFX/Test/526.mp3";
const char SFX_527[] = "SFX/Test/527.mp3";
const char SFX_528[] = "SFX/Test/528.mp3";
const char SFX_529[] = "SFX/Test/529.mp3";

const char SFX_530[] = "SFX/Test/530.mp3";
const char SFX_531[] = "SFX/Test/531.mp3"; //粗犷男2
const char SFX_532[] = "SFX/Test/532.mp3"; //萝莉1
const char SFX_533[] = "SFX/Test/533.mp3"; //萝莉2
const char SFX_534[] = "SFX/Test/534.mp3"; //少年1
const char SFX_535[] = "SFX/Test/535.mp3"; //少年2
const char SFX_536[] = "SFX/Test/536.mp3"; //少女1
const char SFX_537[] = "SFX/Test/537.mp3"; //少女2
const char SFX_538[] = "SFX/Test/538.mp3"; //粗犷男1

const char SFX_539[] = "SFX/Test/539.mp3"; //登入界面，点击登入后两手撕开的音效
const char SFX_540[] = "SFX/Test/540.mp3"; //抽英雄时開門拉到最大時的撞牆的聲音2
const char SFX_541[] = "SFX/Test/541.mp3"; //登入界面，僵尸升上来
const char SFX_542[] = "SFX/Test/542.mp3"; //战士上场，领命
const char SFX_543[] = "SFX/Test/543.mp3"; //教学任务中出现杀杀杀时的音效
const char SFX_544[] = "SFX/Test/544.mp3"; //教学任务中敌方首脑半身像出现的声效
const char SFX_545[] = "SFX/Test/545.mp3";
const char SFX_546[] = "SFX/Test/546.mp3";

const char SFX_600[] = "SFX/Test/600.mp3";
const char SFX_601[] = "SFX/Test/601.mp3";
const char SFX_602[] = "SFX/Test/602.mp3";
const char SFX_603[] = "SFX/Test/603.mp3";
const char SFX_604[] = "SFX/Test/604.mp3";
const char SFX_605[] = "SFX/Test/605.mp3";
const char SFX_606[] = "SFX/Test/606.mp3";
const char SFX_607[] = "SFX/Test/607.mp3";
const char SFX_608[] = "SFX/Test/608.mp3";
const char SFX_609[] = "SFX/Test/609.mp3";
const char SFX_610[] = "SFX/Test/610.mp3";
const char SFX_611[] = "SFX/Test/611.mp3";
const char SFX_612[] = "SFX/Test/612.mp3";
const char SFX_613[] = "SFX/Test/613.mp3";
const char SFX_614[] = "SFX/Test/614.mp3";
const char SFX_615[] = "SFX/Test/615.mp3";
const char SFX_616[] = "SFX/Test/616.mp3";
const char SFX_617[] = "SFX/Test/617.mp3";
const char SFX_618[] = "SFX/Test/618.mp3";
const char SFX_619[] = "SFX/Test/619.mp3";
#else
//const char SFX_[] = "SFX/.ogg";//
const char MainSkill[]          = "music/mainskill.mp3";	//播放主动技能时的音效缺失

const char BGM_Arena[]			= "BGM/100.ogg";			//竞技场背景音乐	
const char BGM_Battle[]			= "BGM/101.ogg";			//战斗
const char BGM_LoadWar[]		= "BGM/102.ogg";			//资源加载背景音乐
const char BGM_Login[]			= "BGM/103.ogg";			//登陆界面
const char BGM_MainCity[]		= "BGM/104.ogg";			//主城背景音乐
const char BGM_Talk1[]          = "BGM/105.ogg";			//剧情
const char BGM_Talk2[]          = "BGM/106.ogg";			//剧情2
const char BGM_Talk3[]          = "BGM/107.ogg";			//剧情3
const char BGM_Boss1[]			= "BGM/108.ogg";			//boss批次背景音乐
const char BGM_Boss2[]          = "BGM/109.ogg";			//普通BOSS音乐
const char BGM_Boss3[]          = "BGM/110.ogg";			//章节BOSS音乐

const char BGM_Boss_Warning[]          = "BGM/111.ogg";			//BOSS-WARNING
const char BGM_Prologue[]          = "BGM/112.ogg";			//序章


const char SFX_Button[]			= "SFX/200.ogg";			//按钮触摸
const char SFX_LoginClick[]     = "SFX/201.ogg";			//登陆成功
const char SFX_Win[]			= "SFX/202.ogg";			//战斗胜利
const char SFX_Cancel[]			= "SFX/203.ogg";			//取消音效
const char SFX_Ensure[]			= "SFX/204.ogg";			//确定音效
const char SFX_Fail[]			= "SFX/205.ogg";			//战斗失败

const char SFX_401[] = "SFX/401.ogg";//寶箱跌落
const char SFX_402[] = "SFX/402.ogg";//掉道具
const char SFX_403[] = "SFX/403.ogg";//功能開放
const char SFX_404[] = "SFX/404.ogg";//经验增加1
const char SFX_405[] = "SFX/405.ogg";//经验增加2
const char SFX_406[] = "SFX/406.ogg";//结算星星
const char SFX_407[] = "SFX/407.ogg";//獲得道具
const char SFX_408[] = "SFX/408.ogg";//獲得金錢1
const char SFX_409[] = "SFX/409.ogg";//獲得金錢2
const char SFX_410[] = "SFX/410.ogg";//獲得靈魂
const char SFX_411[] = "SFX/411.ogg";//僵屍聲音1
const char SFX_412[] = "SFX/412.ogg";//僵屍聲音2
const char SFX_413[] = "SFX/413.ogg";//接受任務
const char SFX_414[] = "SFX/414.ogg";//金錢掉落
const char SFX_415[] = "SFX/415.ogg";//金錢連續結算
const char SFX_416[] = "SFX/416.ogg";//進入界面音效1
const char SFX_417[] = "SFX/417.ogg";//進入界面音效2
const char SFX_418[] = "SFX/418.ogg";//進入界面音效3
const char SFX_419[] = "SFX/419.ogg";//警告音效1
const char SFX_420[] = "SFX/420.ogg";//警告音效2
const char SFX_421[] = "SFX/421.ogg";//酒馆抽奖(拉门）
const char SFX_422[] = "SFX/422.ogg";//酒馆抽奖（灵魂飞出）
const char SFX_423[] = "SFX/423.ogg";//靈魂掉落
const char SFX_424[] = "SFX/424.ogg";//生產物品
const char SFX_425[] = "SFX/425.ogg";//石版
const char SFX_426[] = "SFX/426.ogg";//提升等級
const char SFX_427[] = "SFX/427.ogg";//铁匠铺强化
const char SFX_428[] = "SFX/428.ogg";//铁匠铺洗练
const char SFX_429[] = "SFX/429.ogg";//拖動音效
const char SFX_430[] = "SFX/430.ogg";//完成任務
const char SFX_431[] = "SFX/431.ogg";//武将进阶
const char SFX_432[] = "SFX/432.ogg";//武将升级
const char SFX_433[] = "SFX/433.ogg";//武将突破
const char SFX_434[] = "SFX/434.ogg";//武将突破
const char SFX_435[] = "SFX/435.ogg";//武将突破
const char SFX_436[] = "SFX/436.ogg";//武将突破

const char SFX_500[] = "SFX/500.ogg";//拳击(呯)
const char SFX_501[] = "SFX/501.ogg";//爆炸(坠落)
const char SFX_502[] = "SFX/502.ogg";//枪声1(击中多层铁板声)
const char SFX_503[] = "SFX/503.ogg";//必杀技闪光
const char SFX_504[] = "SFX/504.ogg";//必殺過場音效
const char SFX_505[] = "SFX/505.ogg";//风吹
const char SFX_506[] = "SFX/506.ogg";//火枪
const char SFX_507[] = "SFX/507.ogg";//砍击(微重)
const char SFX_508[] = "SFX/508.ogg";//砍擊(重)
const char SFX_509[] = "SFX/509.ogg";//受擊(啪)
const char SFX_510[] = "SFX/510.ogg";//咬击
const char SFX_511[] = "SFX/511.ogg";//咬擊(浊)
const char SFX_512[] = "SFX/512.ogg";//隕石掉落
const char SFX_513[] = "SFX/513.ogg";//增益BUFF(補血)
const char SFX_514[] = "SFX/514.ogg";//子彈命中聲音
const char SFX_515[] = "SFX/515.ogg"; 
const char SFX_516[] = "SFX/516.ogg"; 
const char SFX_517[] = "SFX/517.ogg"; 
const char SFX_518[] = "SFX/518.ogg"; 
const char SFX_519[] = "SFX/519.ogg"; 
const char SFX_520[] = "SFX/520.ogg"; 
const char SFX_521[] = "SFX/521.ogg"; 
const char SFX_522[] = "SFX/522.ogg"; 
const char SFX_523[] = "SFX/523.ogg"; 
const char SFX_524[] = "SFX/524.ogg"; 
const char SFX_525[] = "SFX/525.ogg"; 
const char SFX_526[] = "SFX/526.ogg"; 
const char SFX_527[] = "SFX/527.ogg"; 
const char SFX_528[] = "SFX/528.ogg"; 
const char SFX_529[] = "SFX/529.ogg"; 
const char SFX_530[] = "SFX/530.ogg"; 
const char SFX_531[] = "SFX/531.ogg"; //粗犷男2
const char SFX_532[] = "SFX/532.ogg"; //萝莉1
const char SFX_533[] = "SFX/533.ogg"; //萝莉2
const char SFX_534[] = "SFX/534.ogg"; //少年1
const char SFX_535[] = "SFX/535.ogg"; //少年2
const char SFX_536[] = "SFX/536.ogg"; //少女1
const char SFX_537[] = "SFX/537.ogg"; //少女2
const char SFX_538[] = "SFX/538.ogg"; //粗犷男1
const char SFX_539[] = "SFX/539.ogg"; //登入界面，点击登入后两手撕开的音效
const char SFX_540[] = "SFX/540.ogg"; //抽英雄时開門拉到最大時的撞牆的聲音2
const char SFX_541[] = "SFX/541.ogg"; //登入界面，僵尸升上来
const char SFX_542[] = "SFX/542.ogg"; //战士上场，领命
const char SFX_543[] = "SFX/543.ogg"; //教学任务中出现杀杀杀时的音效
const char SFX_544[] = "SFX/544.ogg"; //教学任务中敌方首脑半身像出现的声效
const char SFX_545[] = "SFX/545.ogg"; 
const char SFX_546[] = "SFX/546.ogg"; 

const char SFX_600[] = "SFX/600.ogg";
const char SFX_601[] = "SFX/601.ogg";
const char SFX_602[] = "SFX/602.ogg";
const char SFX_603[] = "SFX/603.ogg";
const char SFX_604[] = "SFX/604.ogg";
const char SFX_605[] = "SFX/605.ogg";
const char SFX_606[] = "SFX/606.ogg";
const char SFX_607[] = "SFX/607.ogg";
const char SFX_608[] = "SFX/608.ogg";
const char SFX_609[] = "SFX/609.ogg";
const char SFX_610[] = "SFX/610.ogg";
const char SFX_611[] = "SFX/611.ogg";
const char SFX_612[] = "SFX/612.ogg";
const char SFX_613[] = "SFX/613.ogg";
const char SFX_614[] = "SFX/614.ogg";
const char SFX_615[] = "SFX/615.ogg"; 
const char SFX_616[] = "SFX/616.ogg"; 
const char SFX_617[] = "SFX/617.ogg"; 
const char SFX_618[] = "SFX/618.ogg"; 
const char SFX_619[] = "SFX/619.ogg"; 
#endif

#endif // _AUTO_RESOURCES_H_
