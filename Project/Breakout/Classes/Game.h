#pragma once
#ifndef Game_H
#define Game_H

#include <vector>
#include <map>

#include "Base.h"
#include "GameLevel.h"
#include "Movable.h"
#include "Ball.h"
#include "PowerUp.h"

class Input;
class Camera;
class SpriteRenderer;
class TextRenderer;
class ResourceManager;
class GameLevel;
class Movable;
class Ball;
class ParticleSystem;
class PostProcess;
class PowerUp;

#define PROPERTY(__TYPE__, __NAME__, __MEM_NAME__) \
private:\
	__TYPE__ __MEM_NAME__;\
public:\
	__TYPE__ get##__NAME__##() { return __MEM_NAME__; }\
	void set##__NAME__##(__TYPE__ value) { __MEM_NAME__ = value; }\

#define STATIC_PROPERTY(__TYPE__, __NAME__, __MEM_NAME__) \
private:\
	static __TYPE__ __MEM_NAME__;\
public:\
	static __TYPE__ get##__NAME__##() { return __MEM_NAME__; }\
	static void set##__NAME__##(__TYPE__ value) { __MEM_NAME__ = value; }\


typedef void( *Func0 )();
typedef void( *Func1 )(float);

#define CALLFUNC_SELECTOR0(_SELECTOR) static_cast<Func0>(&_SELECTOR)
#define CALLFUNC_SELECTOR1(_SELECTOR) static_cast<Func1>(&_SELECTOR)


enum class GameState
{
	Lanuch = 0,
	Menu,
	Gaming,
	End
};

class Scheduler
{
public:
	Scheduler::Scheduler() { callFunc = NULL; }

	float delay;
	float interval;
	int invokeCount;
	Func0 callFunc;

	float _elaspeTime;
	int _invokeTimes;
	bool _calledBeforeInterval;

	bool _destroy;
};

class Game
{
	// functions about time
	PROPERTY( double, StartTime, _startTime )
	PROPERTY( double, Time, _currentTime )
	PROPERTY( double, DeltaTime, _deltaTime )

public:
	static Game* getInstance();
	static void DestroyInstance();

public:
	Game();
	virtual ~Game();

	void Clean();
	void SetResolution(unsigned int width, unsigned int height );
	void InitGLConfig();

	void ScheduleOnce( std::string &key, float delay, Func0 callback );
	void Schedule( std::string &key, float delay, float interval, Func0 callback );
	void Schedule( std::string &key, float delay, float interval, int totalInvokeCount, Func0 callback );
	void Unschedule( std::string &key );
	void UpdateSchedulers(float dt);

	void Init();
	void Update(float dt);
	void Render(); 

	void CheckCollision();
	void OnBrickDestroy( GameObject &brick );
	void FixedBallPosition( CollisionInfo &info );
	void ClampPosition(GameObject &go);
	void UpdataPowerUps(float dt);
	void OnPowerUpActivated(PowerUp &power);
	void OnPowerUpDeactivated( PowerUp &power );
	bool IsOtherPowerUpActive(PowerUpType type);

	inline void Pause() { _isPaused = true; }
	inline void Resume() { _isPaused = false; }

	inline void SetUsePowerUp( bool value ) { _usePowerUp = value; }

	void ClearProcessedKey(KeyCode key);

	bool getKey(KeyCode key);
	bool getMouseButton(MouseButton btn);
	const glm::vec2& getMouseScroll();
	const glm::vec2& getMousePosition();

	const glm::mat4& getView();

	inline const glm::vec2& getStageSize() { return _stageSize; }
	inline const glm::mat4& getProjection() { return _projection; }
	inline Input* getInput() { return _input; }
	inline GameState getCurrentState() { return _curState; }

public:
	// game workflow
	void InitGamePlay();
	void PlayerDead();
	bool IsLastLevel();
	bool IsLastLife();
	void Revive();
	void Ending();
	void GoToNextLevel();

protected:
	bool IsKeyProcessed( KeyCode key );
	void SetKeyProcessed( KeyCode key );

	void EnterState( GameState state);
	void ResetLevel();
	void ResetPlayer();
	void AddScore( int value );

protected:
	static Game* _instance;

	Camera* _camera;
	SpriteRenderer* _renderer;
	TextRenderer* _textRender;
	Input* _input;
	ResourceManager* _resManager;
	ParticleSystem* _particleSystem;
	PostProcess* _screenEffect;

	std::vector<GameLevel> levels;
	int curLevel;
	int _remainLifes;
	int _score;
	int _highestScore;

	std::vector<PowerUp> powerUps;
	std::vector<PowerUp>::iterator powerIter;

	Movable* _player;
	Ball* _ball;
	bool _isBallDestroyed;
	bool _isPaused;
	bool _keyProcessed[KeyCode::Key_Count];
	GameState _curState;
	glm::vec2 _stageSize;
	glm::mat4 _projection;

	CollisionInfo _colInfo;

	std::map<std::string, Scheduler> _schedulers;
	std::map<std::string, Scheduler>::iterator schedulerIter;

#pragma region UI
	bool _usePowerUp;

	int menu_select_index;
	int pause_select_index;
	int end_select_index;
#pragma endregion
};

// quit application api
void QuitApplication();

#endif // !Game_H