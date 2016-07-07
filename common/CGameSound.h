#pragma once

/******************************************************
*文件名称:	CGameSound.h
*编写日期:	2016-6-14-11:37
*编写作者:	YPF
*功能描述:	声音播放管理
*******************************************************/

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "Resources.h"
using namespace cocos2d;
using namespace std;

#define SOUND_PLAYING_COUNT 15

#define GameSound CGameSound::getInstance()
#define PlayEffectSound(x) CGameSound::getInstance()->playEffectSound(x)
#define PlayBackgroundMusic(x, y) CGameSound::getInstance()->playBackgroundMusic(x, y)

class LTSoundPlayer;

struct MusicPlayData
{
	std::string sName;
	bool bLoop;
};

class CGameSound
{
public:
	CGameSound(void);
	~CGameSound(void);
public:

	static CGameSound* getInstance();

	static void destroy();

	void init();

	void playEffectSound(const char *name);

	void playBackgroundMusic(const char *name, bool isLoop =true);

	void resumeMusic();
	void pauseMusic();

	void resumeEffect();
	void pauseEffect();

	void stopAllEffects();
	void stopMusic();

	bool isEffectOn();
	bool isMusicOn();

	void setEffectState(bool isOn);
	void setMusicState(bool isOn);

	void enterBackground();
	void enterForeground();

private:
	bool m_bEffectOn;
	bool m_bMusicOn;
	static CGameSound* m_instance;
	MusicPlayData m_musicPlayData;
};
