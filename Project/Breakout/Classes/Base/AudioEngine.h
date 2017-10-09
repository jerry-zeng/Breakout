#pragma once
#ifndef AudioEngine_H
#define AudioEngine_H

#define IRRKLANG_STATIC
#include <irrKlang/irrKlang.h>
using namespace irrklang;

#include <map>

class irrklang::ISoundEngine;
class irrklang::ISound;

class AudioEngine
{
public:
	static AudioEngine* getInstance();
	static void DestroyInstance();

	void PreloadSound( const char* path );

	bool PlayBGM( const char* path, bool loop = true );
	void PauseBGM();
	void ResumeBGM();
	void StopBGM();
	void SetBgmVolume(float volume);

	bool PlaySound(const char* path, bool loop = false, float volume = 1.0f );

	bool IsPlaying( const char* path );

	void PauseAllSounds();
	void ResumeAllSounds();
	void StopAllSounds();

	void Clean();

private:
	AudioEngine();
	virtual ~AudioEngine();

private:
	static AudioEngine* _instance;

	ISoundEngine* _soundPlayer;

	std::map<std::string, ISoundSource*> _preloadedSounds;
	ISound* _currentBGM;
	float _bgmVolume;
};

#endif // !AudioEngine_H