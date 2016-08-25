/************************************************************* 
 *
 *
 *		Data : 2016.4.26
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description 统一化处理png加载: 
 *
 *
 *************************************************************/
 
 #ifndef __BATTLE_LOADPNGDATA__
 #define __BATTLE_LOADPNGDATA__
 
 #include "cocos2d.h"
 using namespace std;
 
 class LoadPngData : public cocos2d::CCObject
 {
 public:
 	LoadPngData();
 	~LoadPngData();
 	virtual bool init(){return true;}
 	CREATE_FUNC(LoadPngData);
 protected:
 	
 private:
 };
 
 #endif