#include "Game.h"
#include "Base/GL.h"

#pragma region Constant Values
const int INIT_LIFE = 3;

const glm::vec2 PLAYER_SIZE = glm::vec2(100, 20);
const float PLAYER_VELOCITY( 500.0f );

const glm::vec2 INITIAL_BALL_VELOCITY( 200.0f, 400.0f );
const float BALL_RADIUS = 12.5f;

const glm::vec2 POWERUP_SIZE( 60, 20 );
const glm::vec2 POWERUP_VELOCITY( 0.0f, -150.0f );

bool clickConfirmKey = false;
#pragma endregion

#pragma region UI
const int Menu_Total_Count = 2;
const int Menu_Selection_Start = 0;
const int Menu_Selection_Quit = 1;

const int Pause_Total_Count = 2;
const int Pause_Selection_Resume = 0;
const int Pause_Selection_Quit = 1;

const int End_Total_Count = 2;
const int End_Selection_Continue = 0;
const int End_Selection_Quit = 1;

const glm::vec3 DEFAULT_TEXT_COLOR( 1.0, 1.0, 1.0 );
const glm::vec3 Menu_Normal_Color( 1.0, 1.0, 1.0 );
const glm::vec3 Menu_Selected_Color( 1.0, 0.0, 0.0 );
#pragma endregion

void Game_Revive()
{
	Game::getInstance()->Revive();
}

void Game_Ending()
{
	Game::getInstance()->Ending();
}

Game* Game::_instance = NULL;

Game* Game::getInstance()
{
	if( _instance == NULL )
		_instance = new Game();
	return _instance;
}

void Game::DestroyInstance()
{
	if( _instance )
		delete _instance;
	_instance = NULL;

	ResourceManager::destroyInstance();
}

Game::Game()
	:_curState(GameState::Lanuch)
	, _isBallDestroyed(true)
	, _isPaused(false)
	, _remainLifes(0)
	, _score(0)
	, _highestScore(0)
	, _usePowerUp(true)
{
	_camera = new Camera();
	_input = new Input();

	_schedulers = std::map<std::string, Scheduler>();

#pragma region UI
	menu_select_index = 0;
	pause_select_index = 0;
	end_select_index = 0;
#pragma endregion
}

Game::~Game()
{
	if(_camera)
		delete _camera;
	if( _input )
		delete _input;
	if( _renderer )
		delete _renderer;
	if( _textRender )
		delete _textRender;
	if( _player )
		delete _player;
	if( _ball )
		delete _ball;
	if( _particleSystem )
		delete _particleSystem;
	if( _screenEffect )
		delete _screenEffect;

	levels.clear();
	powerUps.clear();
	_schedulers.clear();
}

void Game::SetResolution( unsigned int width, unsigned int height )
{
	if( _stageSize.x != width || _stageSize.y != height ){
		_stageSize.x = width;
		_stageSize.y = height;

		_projection = _camera->getOrtho( 0.0, (float)width, 0.0, (float)height );
	}
}

const glm::mat4& Game::getView()
{
	return _camera->getViewMatrix();
}

void Game::InitGLConfig()
{
	glewExperimental = GL_TRUE;
	glewInit();
	glGetError();
}

void Game::Clean()
{
	if( _resManager )
		_resManager->clean();
}

void Game::Init()
{
	//Debug::Log("Game Lanuched");

	_resManager = ResourceManager::getInstance();

	// init renderer
	if( _renderer ) delete _renderer;
	Shader shader = _resManager->loadShader("Resources/Shaders/spriteShader.vsh", "Resources/Shaders/spriteShader.fsh", nullptr, "SpriteShader" );
	_renderer = new SpriteRenderer( shader );

	if( _textRender ) delete _textRender;
	Shader textShader = _resManager->loadShader( "Resources/Shaders/textShader.vsh", "Resources/Shaders/textShader.fsh", nullptr, "TextShader" );
	_textRender = new TextRenderer(textShader);
	_textRender->LoadFont( "Resources/Fonts/Arial.ttf", 40);

	// pre-load textures
	_resManager->loadTexture( "Resources/Textures/background.jpg", "background" );
	_resManager->loadTexture( "Resources/Textures/paddle.png", "paddle" );
	_resManager->loadTexture( "Resources/Textures/block_solid.png", "block_solid" );
	_resManager->loadTexture( "Resources/Textures/block.png", "block" );
	_resManager->loadTexture( "Resources/Textures/ball.png", "ball" );

	// load levels
	levels = std::vector<GameLevel>();
	glm::vec2 posOffset(0.0, _stageSize.y*0.5);
	for( int i = 1; i <= 99; i++ )
	{
		GameLevel level;
		if( level.Load( ("Resources/Levels/level" + Utils::ToString( i ) + ".txt").c_str(), _stageSize.x, _stageSize.y / 2, posOffset ) )
			levels.push_back( level );
		else
			break;
	}
	Debug::Log( " Total levels: " + Utils::ToString( (int)levels.size() ) );

	curLevel = 1;


	// init powerup list
	powerUps = std::vector<PowerUp>();
	_resManager->loadTexture( "Resources/Textures/powerup_sticky.png", "tex_powerup_Sticky");
	_resManager->loadTexture( "Resources/Textures/powerup_passthrough.png", "tex_powerup_PassThrough" );
	_resManager->loadTexture( "Resources/Textures/powerup_speed.png", "tex_powerup_SpeedUp" );
	_resManager->loadTexture( "Resources/Textures/powerup_increase.png", "tex_powerup_PadSizeUp" );
	_resManager->loadTexture( "Resources/Textures/powerup_separate.png", "tex_powerup_Separate" );
	_resManager->loadTexture( "Resources/Textures/powerup_bullet.png", "tex_powerup_Bullet" );

	// create player
	glm::vec2 ppos( _stageSize.x /2 - PLAYER_SIZE.x/2, 5.0);
	_player = new Movable( ppos, PLAYER_SIZE, _resManager->getTexture("paddle"));
	_player->velocity = glm::vec2(PLAYER_VELOCITY, 0.0);

	// create ball
	glm::vec2 bpos = ppos + glm::vec2(PLAYER_SIZE.x/2-BALL_RADIUS, PLAYER_SIZE.y+1);
	_ball = new Ball( bpos, BALL_RADIUS, _resManager->getTexture( "ball" ) );
	_ball->velocity = INITIAL_BALL_VELOCITY;
	_ball->isStuck = true;
	_ball->shape = Shape::Circle;

	_isBallDestroyed = false;
	_remainLifes = INIT_LIFE;

	_colInfo = CollisionInfo();

	// _particleSystem
	Shader particleShader = _resManager->loadShader( "Resources/Shaders/particleShader.vsh", "Resources/Shaders/particleShader.fsh", nullptr, "ParticleShader" );
	Texture2D particleTex = _resManager->loadTexture( "Resources/Textures/particle.png", "particle" );
	_particleSystem = new ParticleSystem( particleShader, particleTex, 150.0 );

	// post process effect
	Shader postProcessShader = _resManager->loadShader( "Resources/Shaders/postProcessShader.vsh", "Resources/Shaders/postProcessShader.fsh", nullptr, "PostProcessShader" );
	_screenEffect = new PostProcess( postProcessShader, _stageSize.x, _stageSize.y);

	// preload sounds
	AudioEngine::getInstance()->PreloadSound("Resources/Sounds/BGM/breakout.mp3");
	AudioEngine::getInstance()->PlayBGM( "Resources/Sounds/BGM/breakout.mp3" );

	// start state
	EnterState( GameState::Menu );
}

#pragma region Schedulers
void Game::Unschedule( std::string &key )
{
	if( _schedulers.find( key ) != _schedulers.end() )
		_schedulers.erase( key );
}

void Game::ScheduleOnce( std::string &key, float delay, Func0 callback )
{
	Schedule( key, delay, 0.0f, 1, callback );
}

void Game::Schedule( std::string &key, float delay, float interval, Func0 callback )
{
	Schedule(key, delay, interval, 9999, callback);
}

void Game::Schedule( std::string &key, float delay, float interval, int totalInvokeCount, Func0 callback )
{
	Unschedule( key );

	Scheduler scheduler = Scheduler();
	scheduler.delay = delay;
	scheduler.interval = interval;
	scheduler.invokeCount = totalInvokeCount;
	scheduler.callFunc = callback;

	scheduler._calledBeforeInterval = false;
	scheduler._destroy = false;
	scheduler._elaspeTime = 0.0f;
	scheduler._invokeTimes = 0;

	_schedulers.insert( std::pair<std::string, Scheduler>( key, scheduler ) );
}

void Game::UpdateSchedulers( float dt )
{
	for( schedulerIter = _schedulers.begin(); schedulerIter != _schedulers.end(); schedulerIter++ )
	{
		Scheduler &scheduler = (*schedulerIter).second;

		if( !scheduler._destroy )
		{
			scheduler._elaspeTime += dt;

			if( scheduler._elaspeTime >= scheduler.delay )
			{
				if( !scheduler._calledBeforeInterval )
				{
					scheduler._calledBeforeInterval = true;

					if( scheduler.callFunc )
						scheduler.callFunc();

					scheduler._invokeTimes++;
				}
				else if( scheduler._elaspeTime >= scheduler.interval + scheduler.delay )
				{
					scheduler._elaspeTime -= scheduler.interval;

					if( scheduler.callFunc )
						scheduler.callFunc();

					scheduler._invokeTimes++;
				}

				if( scheduler._invokeTimes >= scheduler.invokeCount )
				{
					scheduler._destroy = true;
				}
			}
		}
		else{
			Debug::LogError("Destroyed scheduler in map");
		}
	}

	// remove
	for( schedulerIter = _schedulers.begin(); schedulerIter != _schedulers.end(); )
	{
		if( (*schedulerIter).second._destroy ){
			//Debug::Log( " remove scheduler: " + (*schedulerIter).first );
			schedulerIter = _schedulers.erase( schedulerIter );			
		}
		else{
			schedulerIter++;
		}
	}
	
}
#pragma endregion

#pragma region Gaming Logic
void Game::ClampPosition( GameObject &go )
{
	if( go.position.x < 0 )
		go.position.x = 0;
	if( go.position.x > _stageSize.x - go.size.x )
		go.position.x = _stageSize.x - go.size.x;

	if( go.position.y < 0 )
		go.position.y = 0;
	if( go.position.y > _stageSize.y - go.size.y )
		go.position.y = _stageSize.y - go.size.y;
}

void Game::CheckCollision()
{
	// power up with player
	for( PowerUp &power : powerUps )
	{
		if( !power.isDestroyed )
		{
			if( Math::AABBIntersectAABB( power.position, power.size, _player->position, _player->size ) )
			{
				power.isDestroyed = true;

				if( !IsOtherPowerUpActive( power.getType() ) && !_ball->isDestroyed )
				{
					power._isActivated = true;

					OnPowerUpActivated(power);
					AudioEngine::getInstance()->PlaySound("Resources/Sounds/SE/powerup.wav");
				}
			}
		}
	}

	// ball with bricks and player
	if( _ball && !_ball->isDestroyed )
	{
		// 1. check ball with bricks
		for( GameObject &brick : levels[curLevel-1].bricks )
		{
			if( brick.isDestroyed ) continue;

			if( Math::CircleIntersectAABB( glm::vec2( _ball->position.x + _ball->radius, _ball->position.y + _ball->radius ), _ball->radius,
				brick.position, brick.size, _colInfo ) )
			{
				if( !brick.isSolid ){
					OnBrickDestroy( brick );
					brick.isDestroyed = true;
					AudioEngine::getInstance()->PlaySound( "Resources/Sounds/SE/bleep1.wav" );
					
					// add score
					AddScore( 50 );

					// check end
					if( levels[curLevel-1].IsCompleted() )
					{
						//AudioEngine::getInstance()->PlaySound( "Resources/Sounds/SE/stage_clear.wav" );
						Ending();
					}
				}
				else{
					_screenEffect->DoShake(0.1f);
					AudioEngine::getInstance()->PlaySound( "Resources/Sounds/SE/solid.wav" );
					
					AddScore( 10 );
				}

				// implement PowerUpType::PassThrough
				if( _ball->passThrough && !brick.isSolid ){
					// pass through
				}
				else{
					FixedBallPosition( _colInfo );
				}
				
				return; //if ball collide to bricks, it won't collide to player 
			}
		} // end for


		// 2. check ball with player
		if( !_player || _ball->isStuck ) return;

		if( Math::CircleIntersectAABB( glm::vec2( _ball->position.x + _ball->radius, _ball->position.y + _ball->radius ), _ball->radius,
			_player->position, _player->size, _colInfo ) )
		{
			if( _colInfo.direction != Direction::Down )
			{
				float fixDist = _ball->radius - abs( _colInfo.difference.y );
				_ball->position.y += fixDist + 5;
				
				float playerCenterX = _player->position.x + _player->size.x / 2;
				float distX = _ball->position.x + _ball->radius - playerCenterX;
				float percent = distX / (_player->size.x / 2);

				glm::vec2 oldVelocity = _ball->velocity;

				// change velocity direction
				float strength = 2.0;
				_ball->velocity.y = abs(_ball->velocity.y);
				_ball->velocity.x = INITIAL_BALL_VELOCITY.x * percent * strength;
				// use old velocity value and new velocity direction
				_ball->velocity = glm::normalize( _ball->velocity ) * glm::length(oldVelocity);
			
				AudioEngine::getInstance()->PlaySound( "Resources/Sounds/SE/bleep.wav" );
			
				// implement PowerUpType::Sticky
				_ball->isStuck = _ball->sticky;
			}
		}
	}
}

void Game::FixedBallPosition( CollisionInfo &info )
{
	// 该方法有缺陷，速度快时，小球进入砖块里面，偏移不正确.
	if( _ball && !_ball->isDestroyed )
	{
		Direction dir = info.direction;
		glm::vec2 diff = info.difference;
		if( dir == Direction::Left || dir == Direction::Right )
		{
			_ball->velocity.x = -_ball->velocity.x;
			
			float fixDist = _ball->radius - abs( diff.x );
			if( dir == Direction::Left )
				_ball->position.x += fixDist;
			else
				_ball->position.x -= fixDist;
		}
		else
		{
			_ball->velocity.y = -_ball->velocity.y;

			float fixDist = _ball->radius - abs( diff.y );
			if( dir == Direction::Up )
				_ball->position.y += fixDist;
			else
				_ball->position.y -= fixDist;
		}
	}
}

void Game::OnBrickDestroy( GameObject &brick )
{
	if( !_usePowerUp ) return;

	int randValue = rand() % 100; // 0~99
	//Debug::Log("random value: " + Utils::ToString(randValue));
	if( randValue < 20 ) //20% chance to drop power
	{
		glm::vec2 size = POWERUP_SIZE;
		glm::vec2 pos = glm::vec2( brick.position.x + (brick.size.x-size.x)/2, brick.position.y);

		PowerUp power;
		if( randValue < 5 ){
			power = PowerUp( PowerUpType::Sticky, 20.0, pos, size, _resManager->getTexture("tex_powerup_Sticky"));
		}
		else if( randValue < 10 ){
			power = PowerUp( PowerUpType::PassThrough, 10.0, pos, size, _resManager->getTexture( "tex_powerup_PassThrough" ) );
		}
		else if( randValue < 15 ){
			power = PowerUp( PowerUpType::SpeedUp, 30.0, pos, size, _resManager->getTexture( "tex_powerup_SpeedUp" ) );
		}
		else{
			power = PowerUp( PowerUpType::PadSizeUp, 30.0, pos, size, _resManager->getTexture( "tex_powerup_PadSizeUp" ) );
		}
		power.velocity = POWERUP_VELOCITY;

		powerUps.push_back( power );
	}
}

bool Game::IsOtherPowerUpActive( PowerUpType type )
{
	for( PowerUp &power : powerUps )
	{
		if( power._isActivated && power.getType() == type )
			return true;
	}
	return false;
}

void Game::OnPowerUpActivated( PowerUp &power )
{
	if( power.getType() == PowerUpType::Sticky ){
		_ball->sticky = true;
		_ball->color = glm::vec3( 1.0f, 0.5f, 1.0f );
		_player->color = glm::vec3( 1.0f, 0.5f, 1.0f );
	}
	else if( power.getType() == PowerUpType::PassThrough ){
		_ball->passThrough = true;
		_ball->color = glm::vec3( 1.0f, 0.5f, 0.5f );
		_player->color = glm::vec3( 1.0f, 0.5f, 0.5f );
	}
	else if( power.getType() == PowerUpType::SpeedUp ){
		_ball->velocity = glm::vec2(_ball->velocity.x*1.5, _ball->velocity.y*1.5);
	}
	else if( power.getType() == PowerUpType::PadSizeUp ){
		_player->size.x += 50;
	}
}

void Game::OnPowerUpDeactivated( PowerUp &power )
{
	if( power.getType() == PowerUpType::Sticky ){
		_ball->sticky = false;
		_ball->color = glm::vec3( 1.0 );
		_player->color = glm::vec3(1.0);
	}
	else if( power.getType() == PowerUpType::PassThrough ){
		_ball->passThrough = false;
		_ball->color = glm::vec3( 1.0 );
		_player->color = glm::vec3( 1.0 );
	}
	else if( power.getType() == PowerUpType::SpeedUp ){
		_ball->velocity = glm::vec2( _ball->velocity.x/1.5, _ball->velocity.y/1.5 );
	}
	else if( power.getType() == PowerUpType::PadSizeUp ){
		_player->size.x -= 50;
	}
}

void Game::UpdataPowerUps( float dt )
{
	for( PowerUp &power : powerUps )
	{
		if(!power.isDestroyed)
			power.Update( dt );

		if( power._isActivated )
		{
			power._duration -= dt;

			if( power._duration <= 0.0 )
			{
				power._isActivated = false;

				OnPowerUpDeactivated( power );
			}
		}
	}

	// remove
	for( powerIter = powerUps.begin(); powerIter != powerUps.end(); )
	{
		if( (*powerIter).isDestroyed && !(*powerIter)._isActivated ){
			powerIter = powerUps.erase( powerIter );
		}
		else{
			++powerIter;
		}
	}
}

void Game::ResetLevel()
{
	powerUps.clear();

	_screenEffect->Reset();

	if( curLevel >= 1 && curLevel <= levels.size() )
		levels[curLevel - 1].Reset();
}

void Game::ResetPlayer()
{
	glm::vec2 ppos = glm::vec2( _stageSize.x / 2 - PLAYER_SIZE.x / 2, 5.0 );
	_player->Reset( ppos, glm::vec2( PLAYER_VELOCITY, 0 ) );
	_player->size = PLAYER_SIZE;
	_player->color = glm::vec3( 1.0 );

	glm::vec2 bpos = ppos + glm::vec2( PLAYER_SIZE.x / 2 - BALL_RADIUS, PLAYER_SIZE.y + 1 );
	_ball->Reset( bpos, INITIAL_BALL_VELOCITY );
	_ball->color = glm::vec3( 1.0 );
	_ball->isDestroyed = false;

	_isBallDestroyed = false;
}
#pragma endregion

#pragma region Workflow
void Game::AddScore( int value )
{
	_score += value;
}

void Game::InitGamePlay()
{
	_remainLifes = INIT_LIFE;
	curLevel = 1;
}

void Game::PlayerDead()
{
	if( IsLastLife() ){		
		Debug::Log( "wait for ending..." );
		ScheduleOnce( std::string( "wait_for_ending_lost" ), 3.0, CALLFUNC_SELECTOR0( Game_Ending ) );
	}
	else{
		Debug::Log("wait for revive...");
		ScheduleOnce( std::string( "wait_for_revive" ), 3.0, CALLFUNC_SELECTOR0( Game_Revive ) );
	}
}

void Game::Ending()
{
	EnterState( GameState::End );
}

void Game::Revive()
{
	_remainLifes--;

	ResetPlayer();
	_ball->isDestroyed = false;

	AudioEngine::getInstance()->PlaySound("Resources/Sounds/SE/prepare.wav");
}

bool Game::IsLastLevel()
{
	return curLevel >= levels.size();
}

bool Game::IsLastLife()
{
	return _remainLifes <= 0;
}

void Game::GoToNextLevel()
{
	if( IsLastLevel() ){
		curLevel = 1;

		ResetLevel();
		ResetPlayer();
	}
	else{
		curLevel++;

		ResetLevel();
		ResetPlayer();
	}

	AudioEngine::getInstance()->PlaySound( "Resources/Sounds/SE/prepare.wav" );
}
#pragma endregion

void Game::EnterState( GameState state )
{
	switch( state )
	{
		default:
		case GameState::Lanuch:
		{
			Debug::LogError( "No need to enter GameState::Lanuch" );
		}
		return;

		case GameState::Menu:
		{
			menu_select_index = 0;
		}
		break;
		case GameState::Gaming:
		{
			ResetLevel();
			ResetPlayer();

			AudioEngine::getInstance()->PlaySound( "Resources/Sounds/SE/prepare.wav" );
		}
		break;
		case GameState::End:
		{
			end_select_index = 0;


		}
		break;
	}
	_curState = state;
}

void Game::Update(float dt)
{
	UpdateSchedulers(dt);

	if( _curState == GameState::Menu )
	{
#pragma region GameState::Menu
		if( getKey( KeyCode::Key_Up ) && !IsKeyProcessed( KeyCode::Key_Up ) )
		{
			menu_select_index -= 1;
			SetKeyProcessed( KeyCode::Key_Up );
		}
		else if( getKey( KeyCode::Key_W ) && !IsKeyProcessed( KeyCode::Key_W ) )
		{
			menu_select_index -= 1;
			SetKeyProcessed( KeyCode::Key_W );
		}
		else if( getKey( KeyCode::Key_Down ) && !IsKeyProcessed( KeyCode::Key_Down ) )
		{
			menu_select_index += 1;
			SetKeyProcessed( KeyCode::Key_Down );
		}
		else if( getKey( KeyCode::Key_S ) && !IsKeyProcessed( KeyCode::Key_S ) )
		{
			menu_select_index += 1;
			SetKeyProcessed( KeyCode::Key_S );
		}

		if( menu_select_index < 0 )
			menu_select_index = Menu_Total_Count - 1;
		if( menu_select_index > Menu_Total_Count - 1 )
			menu_select_index = 0;

		// confirm
		clickConfirmKey = false;
		if( getKey( KeyCode::Key_Space ) && !IsKeyProcessed( KeyCode::Key_Space ) ){
			clickConfirmKey = true;
			SetKeyProcessed( KeyCode::Key_Space );
		}
		else if( getKey( KeyCode::Key_Enter ) && !IsKeyProcessed( KeyCode::Key_Enter ) ){
			clickConfirmKey = true;
			SetKeyProcessed( KeyCode::Key_Enter );
		}
		if( clickConfirmKey )
		{
			if( menu_select_index == Menu_Selection_Start ){
				InitGamePlay();
				_remainLifes--;
				EnterState( GameState::Gaming );
			}
			else if( menu_select_index == Menu_Selection_Quit ){
				Debug::Log( "Quit game" );
				extern void QuitApplication();
				QuitApplication();
			}
		}
#pragma endregion
	}
	else if( _curState == GameState::Gaming )
	{
		if( !_isPaused && getKey( KeyCode::Key_Escape ) )
		{
			Pause();
			pause_select_index = 0;
		}

#pragma region Pause input
		if( _isPaused )
		{
			if( getKey( KeyCode::Key_Up ) && !IsKeyProcessed( KeyCode::Key_Up ) )
			{
				pause_select_index -= 1;
				SetKeyProcessed( KeyCode::Key_Up );
			}
			else if( getKey( KeyCode::Key_W ) && !IsKeyProcessed( KeyCode::Key_W ) )
			{
				pause_select_index -= 1;
				SetKeyProcessed( KeyCode::Key_W );
			}
			else if( getKey( KeyCode::Key_Down ) && !IsKeyProcessed( KeyCode::Key_Down ) )
			{
				pause_select_index += 1;
				SetKeyProcessed( KeyCode::Key_Down );
			}
			else if( getKey( KeyCode::Key_S ) && !IsKeyProcessed( KeyCode::Key_S ) )
			{
				pause_select_index += 1;
				SetKeyProcessed( KeyCode::Key_S );
			}

			if( pause_select_index < 0 )
				pause_select_index = Pause_Total_Count - 1;
			if( pause_select_index > Pause_Total_Count - 1 )
				pause_select_index = 0;

			// confirm
			clickConfirmKey = false;
			if( getKey( KeyCode::Key_Space ) && !IsKeyProcessed( KeyCode::Key_Space ) ){
				clickConfirmKey = true;
				SetKeyProcessed( KeyCode::Key_Space );
			}
			else if( getKey( KeyCode::Key_Enter ) && !IsKeyProcessed( KeyCode::Key_Enter ) ){
				clickConfirmKey = true;
				SetKeyProcessed( KeyCode::Key_Enter );
			}
			if( clickConfirmKey )
			{
				if( pause_select_index == Pause_Selection_Resume ){
					Resume();
				}
				else if( pause_select_index == Pause_Selection_Quit ){
					EnterState( GameState::Menu );
				}
			}

			return; // never update on game paused
		}
#pragma endregion

#pragma region GameState::Gaming
		if( _player && _ball && !_ball->isDestroyed )
		{
			if( getKey( KeyCode::Key_Left ) || getKey( KeyCode::Key_A ) )
			{
				float posX = _player->position.x;

				_player->MoveTowards( Direction::Left, dt );

				if( _ball && _ball->isStuck )
					_ball->position.x += _player->position.x - posX;
			}
			else if( getKey( KeyCode::Key_Right ) || getKey( KeyCode::Key_D ) )
			{
				float posX = _player->position.x;

				_player->MoveTowards( Direction::Right, dt );

				if( _ball && _ball->isStuck )
					_ball->position.x += _player->position.x - posX;
			}
		}

		if( _ball )
		{
			_ball->Update( dt );

			if( !_ball->isDestroyed )
			{
				if( _ball->isStuck && getKey( KeyCode::Key_Space ) && !IsKeyProcessed( KeyCode::Key_Space ) ){
					_ball->isStuck = false;
					SetKeyProcessed(KeyCode::Key_Space);
				}
			}
			else{
				if( !_isBallDestroyed ){
					AudioEngine::getInstance()->PlaySound( "Resources/Sounds/SE/die.wav" );
					PlayerDead();
					_isBallDestroyed = true;
				}
			}
		}

		UpdataPowerUps( dt );

		CheckCollision();

		if( _particleSystem && _ball )
			_particleSystem->Update( dt, *_ball );

		if( _screenEffect )
			_screenEffect->Update( dt );
#pragma endregion
	}
	else if( _curState == GameState::End )
	{
#pragma region GameState::End
		if( getKey( KeyCode::Key_Up ) && !IsKeyProcessed( KeyCode::Key_Up ) )
		{
			end_select_index -= 1;
			SetKeyProcessed( KeyCode::Key_Up );
		}
		else if( getKey( KeyCode::Key_W ) && !IsKeyProcessed( KeyCode::Key_W ) )
		{
			end_select_index -= 1;
			SetKeyProcessed( KeyCode::Key_W );
		}
		else if( getKey( KeyCode::Key_Down ) && !IsKeyProcessed( KeyCode::Key_Down ) )
		{
			end_select_index += 1;
			SetKeyProcessed( KeyCode::Key_Down );
		}
		else if( getKey( KeyCode::Key_S ) && !IsKeyProcessed( KeyCode::Key_S ) )
		{
			end_select_index += 1;
			SetKeyProcessed( KeyCode::Key_S );
		}

		if( end_select_index < 0 )
			end_select_index = End_Total_Count - 1;
		if( end_select_index > End_Total_Count - 1 )
			end_select_index = 0;

		clickConfirmKey = false;
		if( getKey( KeyCode::Key_Space ) && !IsKeyProcessed( KeyCode::Key_Space ) ){
			clickConfirmKey = true;
			SetKeyProcessed( KeyCode::Key_Space );
		}
		else if( getKey( KeyCode::Key_Enter ) && !IsKeyProcessed( KeyCode::Key_Enter ) ){
			clickConfirmKey = true;
			SetKeyProcessed( KeyCode::Key_Enter );
		}
		if( clickConfirmKey )
		{
			if( end_select_index == End_Selection_Continue ){
				EnterState(GameState::Gaming);
				GoToNextLevel();
			}
			else if( end_select_index == End_Selection_Quit ){
				EnterState( GameState::Menu );
			}
		}
#pragma endregion
	}
}

void Game::Render()
{
	if( _curState == GameState::Menu )
	{
#pragma region GameState::Menu
		// draw background
		_renderer->DrawSprite( _resManager->getTexture( "background" ),
			glm::vec2( 0.0, 0.0 ),
			glm::vec2( _stageSize.x, _stageSize.y )
		);

		// draw text
		if( _textRender ){
			_textRender->RenderText( "Breakout", glm::vec2( _stageSize.x/2 - 120, _stageSize.y - 160 ), glm::vec3(1.0, 0.3, 0.5), 1.5f );
			
			if( menu_select_index == Menu_Selection_Start )
				_textRender->RenderText( "Start Game", glm::vec2( _stageSize.x / 2 - 90, _stageSize.y/2 ), Menu_Selected_Color, 0.9 );
			else
				_textRender->RenderText( "Start Game", glm::vec2( _stageSize.x / 2 - 80, _stageSize.y / 2 ), Menu_Normal_Color, 0.8 );
			
			if( menu_select_index == Menu_Selection_Quit )
				_textRender->RenderText( "Quit Game", glm::vec2( _stageSize.x / 2 - 90, _stageSize.y/2 - 50 ), Menu_Selected_Color, 0.9 );
			else
				_textRender->RenderText( "Quit Game", glm::vec2( _stageSize.x / 2 - 80, _stageSize.y / 2 - 50 ), Menu_Normal_Color, 0.8 );

			_textRender->RenderText( "press ENTER or SPACE to select", glm::vec2( 10, 10 ), glm::vec3(0.3,0.5,1.0), 0.5 );
		}
#pragma endregion
	}
	else if( _curState == GameState::Gaming )
	{
#pragma region GameState::Gaming
		_screenEffect->BeginRender();
		{
			// draw background
			_renderer->DrawSprite( _resManager->getTexture( "background" ),
				glm::vec2( 0.0, 0.0 ),
				glm::vec2( _stageSize.x, _stageSize.y )
			);

			// draw level
			if( curLevel >= 1 && curLevel <= levels.size() )
			{
				levels[curLevel - 1].Draw( _renderer );
			}

			// draw player
			if( _player )
				_player->Draw( _renderer );

			// draw particles
			if( _particleSystem )
				_particleSystem->Draw();

			// draw power up
			for( PowerUp &power : powerUps )
			{
				if( !power.isDestroyed )
				{
					power.Draw( _renderer );
				}
			}

			// draw ball
			if( _ball )
				_ball->Draw( _renderer );

			// draw text
			if( _textRender ){
				_textRender->RenderText( "Score: " + Utils::ToString(_score), glm::vec2( 5.0, _stageSize.y - _textRender->getFontSize()*0.8 ), DEFAULT_TEXT_COLOR, 0.8 );
				_textRender->RenderText( "Life: "+Utils::ToString(_remainLifes), glm::vec2( 5.0, _stageSize.y - _textRender->getFontSize()*0.8*2 ), glm::vec3( 0.3, 1.0, 0.3 ), 0.8 );
			}
		}
		_screenEffect->EndRender();

		_screenEffect->Render();
#pragma endregion

#pragma region Pause menu
		// pause menu
		if( _isPaused )
		{
			// draw mask
			_renderer->DrawSprite( _resManager->getTexture( "background" ),
				glm::vec2( 0.0, 0.0 ),
				glm::vec2( _stageSize.x, _stageSize.y ),
				0.0,
				glm::vec3(0.0),
				0.5
			);

			// menu items
			if( _textRender )
			{
				if( pause_select_index == Pause_Selection_Resume )
					_textRender->RenderText( "Resume Game", glm::vec2( _stageSize.x / 2 - 120, _stageSize.y / 2+50 ), Menu_Selected_Color, 0.9 );
				else
					_textRender->RenderText( "Resume Game", glm::vec2( _stageSize.x / 2 - 110, _stageSize.y / 2 + 50 ), Menu_Normal_Color, 0.8 );

				if( pause_select_index == Pause_Selection_Quit )
					_textRender->RenderText( "Main Menu", glm::vec2( _stageSize.x / 2 - 90, _stageSize.y / 2 ), Menu_Selected_Color, 0.9 );
				else
					_textRender->RenderText( "Main Menu", glm::vec2( _stageSize.x / 2 - 80, _stageSize.y / 2 ), Menu_Normal_Color, 0.8 );
			}
		}
#pragma endregion
	}
	else if( _curState == GameState::End )
	{
#pragma region GameState::End
		// draw background
		_renderer->DrawSprite( _resManager->getTexture( "background" ),
			glm::vec2( 0.0, 0.0 ),
			glm::vec2( _stageSize.x, _stageSize.y )
		);

		// draw mask
		_renderer->DrawSprite( _resManager->getTexture( "background" ),
			glm::vec2( 0.0, 0.0 ),
			glm::vec2( _stageSize.x, _stageSize.y ),
			0.0,
			glm::vec3( 0.0 ),
			0.5
		);

		// menu items
		if( _textRender )
		{
			_textRender->RenderText( "Score: "+Utils::ToString(_score), glm::vec2( _stageSize.x / 2 - 110, _stageSize.y / 2 + 150 ), glm::vec3(1.0, 0.8, 0.0) );
			_textRender->RenderText( "Highest Score: " + Utils::ToString( _highestScore ), glm::vec2( _stageSize.x / 2 - 110, _stageSize.y / 2 + 100 ), glm::vec3( 0.8, 0.8, 0.0 ), 0.5 );

			if( end_select_index == End_Selection_Continue )
			{
				if( IsLastLevel() || IsLastLife() )
					_textRender->RenderText("Restart", glm::vec2( _stageSize.x / 2 - 70, _stageSize.y / 2 - 50 ), Menu_Selected_Color, 0.9 );
				else
					_textRender->RenderText("Next Level", glm::vec2( _stageSize.x / 2 - 80, _stageSize.y / 2 - 50 ), Menu_Selected_Color, 0.9 );
			}
			else
			{
				if( IsLastLevel() || IsLastLife() )
					_textRender->RenderText("Restart", glm::vec2( _stageSize.x / 2 - 70, _stageSize.y / 2 - 50 ), Menu_Normal_Color, 0.8 );
				else
					_textRender->RenderText("Next Level", glm::vec2( _stageSize.x / 2 - 80, _stageSize.y / 2 - 50 ), Menu_Normal_Color, 0.8 );
			}

			if( end_select_index == End_Selection_Quit )
				_textRender->RenderText( "Main Menu", glm::vec2( _stageSize.x / 2 - 90, _stageSize.y / 2-100 ), Menu_Selected_Color, 0.9 );
			else
				_textRender->RenderText( "Main Menu", glm::vec2( _stageSize.x / 2 - 80, _stageSize.y / 2-100 ), Menu_Normal_Color, 0.8 );
		}
#pragma endregion
	}
}

#pragma region Input
void Game::ClearProcessedKey( KeyCode key )
{
	if( key != KeyCode::Key_Count )
		_keyProcessed[key] = false;
}
bool Game::IsKeyProcessed( KeyCode key )
{
	if( key != KeyCode::Key_Count )
		return _keyProcessed[key];
	return false;
}
void Game::SetKeyProcessed( KeyCode key )
{
	if( key != KeyCode::Key_Count )
		_keyProcessed[key] = true;
}

bool Game::getKey( KeyCode key )
{
	return _input->getKey(key);
}
bool Game::getMouseButton( MouseButton btn )
{
	return _input->getMouseButton( btn );
}
const glm::vec2& Game::getMouseScroll()
{
	return _input->getMouseScroll();
}
const glm::vec2& Game::getMousePosition()
{
	return _input->getMousePosition();
}
#pragma endregion
