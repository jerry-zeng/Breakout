#include "AudioEngine.h"

AudioEngine* AudioEngine::_instance = NULL;

AudioEngine* AudioEngine::getInstance()
{
	if( !_instance )
		_instance = new AudioEngine();
	return _instance;
}

void AudioEngine::DestroyInstance()
{
	if( _instance ){
		_instance->Clean();
		delete _instance;
	}
}

AudioEngine::AudioEngine()
	:_currentBGM(NULL)
	, _bgmVolume(1.0f)
{
	_soundPlayer = createIrrKlangDevice();

	_preloadedSounds = std::map<std::string, ISoundSource*>();
}

AudioEngine::~AudioEngine()
{
	_currentBGM = NULL;

	if( _soundPlayer ){
		_soundPlayer->drop();
		_soundPlayer = NULL;
	}
}

void AudioEngine::Clean()
{
	StopBGM();

	std::map<std::string, ISoundSource*>::iterator iter;
	
	for( iter = _preloadedSounds.begin(); iter != _preloadedSounds.end(); ++iter )
	{
		_soundPlayer->removeSoundSource( iter->first.c_str() );
	}
	_preloadedSounds.clear();
}

void AudioEngine::PreloadSound( const char* path )
{
	ISoundSource* source = _soundPlayer->addSoundSourceFromFile( path );
	if( source ){
		_preloadedSounds.insert( std::pair<std::string, ISoundSource*>( std::string( path ), source ) );
	}
}

bool AudioEngine::PlayBGM( const char* path, bool loop )
{
	StopBGM();

	if( _preloadedSounds.find( path ) != _preloadedSounds.end() )
		_currentBGM = _soundPlayer->play2D( _preloadedSounds.at(path), loop );
	else
		_currentBGM = _soundPlayer->play2D( path, loop );

	if( _currentBGM ){
		_currentBGM->setVolume( _bgmVolume );
		return true;
	}
	else{
		return false;
	}
}

void AudioEngine::PauseBGM()
{
	if( _currentBGM )
		_currentBGM->setIsPaused(true);
}
void AudioEngine::ResumeBGM()
{
	if( _currentBGM )
		_currentBGM->setIsPaused( false );
}
void AudioEngine::StopBGM()
{
	if( _currentBGM )
		_currentBGM->stop();
}
void AudioEngine::SetBgmVolume( float volume )
{
	_bgmVolume = volume;
	if( _bgmVolume > 1.0 )
		_bgmVolume = 1.0;
	if( _bgmVolume < 0.0 )
		_bgmVolume = 0.0;

	if( _currentBGM )
		_currentBGM->setVolume( _bgmVolume );
}


bool AudioEngine::PlaySound( const char* path, bool loop, float volume )
{
	ISound* sound = NULL;
	if( _preloadedSounds.find( path ) != _preloadedSounds.end() )
		sound = _soundPlayer->play2D( _preloadedSounds.at( path ), loop );
	else
		sound = _soundPlayer->play2D( path, loop );

	if( sound ){
		sound->setVolume( volume );
		return true;
	}
	else{
		return false;
	}
}

void AudioEngine::PauseAllSounds()
{
	_soundPlayer->setAllSoundsPaused( true );
}

void AudioEngine::ResumeAllSounds()
{
	_soundPlayer->setAllSoundsPaused(false);
}

void AudioEngine::StopAllSounds()
{
	_soundPlayer->stopAllSounds();
}

bool AudioEngine::IsPlaying( const char* path )
{
	return _soundPlayer->isCurrentlyPlaying(path);
}