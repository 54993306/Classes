#include "common/CGameSound.h"
#include "UserDefaultData.h"

using namespace CocosDenshion;

USING_NS_CC;

using namespace  std;

CGameSound* CGameSound::m_instance = nullptr;

CGameSound::CGameSound(void)
	:m_bEffectOn(true)
	,m_bMusicOn(true)
{
	m_musicPlayData.sName="";
	m_musicPlayData.bLoop = true;
}


CGameSound::~CGameSound(void)
{

}

void CGameSound::init()
{

	bool isEffectOn = CCUserDefault::sharedUserDefault()->getBoolForKey(EFF_MUSIC, true);
	bool isMusicOn = CCUserDefault::sharedUserDefault()->getBoolForKey(BG_MUSIC, true);

	setEffectState(isEffectOn);
	setMusicState(isMusicOn);
}

void CGameSound::stopAllEffects()
{
	SimpleAudioEngine::sharedEngine()->stopAllEffects();
}

void CGameSound::stopMusic()
{
	SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
}

bool CGameSound::isEffectOn()
{
	return m_bEffectOn;
}

bool CGameSound::isMusicOn()
{
	return m_bMusicOn;
}

void CGameSound::setEffectState( bool isOn )
{
	CCUserDefault::sharedUserDefault()->setBoolForKey(EFF_MUSIC, isOn);
	CCUserDefault::sharedUserDefault()->flush();

	m_bEffectOn = isOn;
	if(m_bEffectOn)
	{
		resumeEffect();
		CocosDenshion::SimpleAudioEngine::sharedEngine()->setEffectsVolume(1.0f);
	}
	else
	{
		pauseEffect();
		CocosDenshion::SimpleAudioEngine::sharedEngine()->setEffectsVolume(0.0f);
	}
}

void CGameSound::setMusicState( bool isOn )
{
	CCUserDefault::sharedUserDefault()->setBoolForKey(BG_MUSIC, isOn);
	CCUserDefault::sharedUserDefault()->flush();

	m_bMusicOn = isOn;
	if(m_bMusicOn)
	{
		resumeMusic();
		CocosDenshion::SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(1.0f);
	}
	else
	{
		pauseMusic();
		CocosDenshion::SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(0.0f);
	}
}

void CGameSound::playEffectSound( const char *name)
{
	if(!m_bEffectOn) return;

	SimpleAudioEngine::sharedEngine()->playEffect(name);
}


void CGameSound::playBackgroundMusic( const char *name, bool isLoop /*=true*/ )
{
	SimpleAudioEngine::sharedEngine()->playBackgroundMusic(name, isLoop);
	m_musicPlayData.sName = name;
	m_musicPlayData.bLoop = isLoop;

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	if(!m_bMusicOn)
	{
		SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
	}
#endif
}

void CGameSound::resumeMusic()
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}

void CGameSound::pauseMusic()
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

void CGameSound::resumeEffect()
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->resumeAllEffects();
}

void CGameSound::pauseEffect()
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->pauseAllEffects();
}

CGameSound* CGameSound::getInstance()
{
	if(m_instance==nullptr)
	{
		m_instance = new CGameSound;
	}
	return m_instance;
}

void CGameSound::destroy()
{
	CC_SAFE_DELETE(m_instance);
	m_instance = nullptr;
}

void CGameSound::enterBackground()
{
	//��������п�
	if(m_bMusicOn)
	{
		pauseMusic();
	}

	//�����Ч�п�
	if(m_bEffectOn)
	{
		//ֹͣ���е���Ч
		pauseEffect();
	}
}

void CGameSound::enterForeground()
{
	//��������п�
	if(m_bMusicOn)
	{
		resumeMusic();
		//��Ч�ļ��п��ܱ��ͷţ��������²���һ�Σ�������main.cppԤ���أ�����ʱ��̫����
		SimpleAudioEngine::sharedEngine()->playBackgroundMusic(m_musicPlayData.sName.c_str(), m_musicPlayData.bLoop);
	}

	//�����Ч�п�
	if(m_bEffectOn)
	{
		//�ָ����е���Ч
		resumeEffect();
	}
}
