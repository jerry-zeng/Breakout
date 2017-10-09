// GLFW
#include <GLFW/glfw3.h>

#include "Game.h"

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

//input
void key_callback( GLFWwindow* window, int key, int scancode, int action, int mode );
void mouse_button_callback( GLFWwindow* window, int id, int action, int mod );
void cursor_callback( GLFWwindow* window, double posX, double posY );
void scroll_callback( GLFWwindow* window, double offsetX, double offsetY );

static double startTime = 0.0;
static double currentFrame = 0.0;
static double lastFrame = 0.0;
static double deltaTime = 0.0;

int main( int argc, char *argv[] )
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
	
	// anti alias
	glfwWindowHint( GLFW_SAMPLES, 4 );

	// Create a GLFWwindow object that we can use for GLFW's functions
	char* title;
	if( argc >= 2 )
		title = argv[1];
	else
		title = "Breakout";
	GLFWwindow* window = glfwCreateWindow( SCREEN_WIDTH, SCREEN_HEIGHT, title, nullptr, nullptr );
	glfwMakeContextCurrent( window );

	//glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
	
	// Set the required callback functions
	glfwSetKeyCallback( window, key_callback );
	glfwSetMouseButtonCallback( window, mouse_button_callback );
	glfwSetCursorPosCallback( window, cursor_callback );
	glfwSetScrollCallback( window, scroll_callback );

	// time
	startTime = glfwGetTime();
	currentFrame = startTime;
	lastFrame = startTime;
	deltaTime = 0.0f;

	// init random seed
	srand( (unsigned int)startTime );

	Game* game = Game::getInstance();
	game->SetResolution( SCREEN_WIDTH, SCREEN_HEIGHT );
	game->InitGLConfig();
	game->Init();

	game->setStartTime( startTime );

	glEnable(GL_BLEND);

	while( !glfwWindowShouldClose( window ) )
	{
		// Calculate delta time
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		game->setTime(currentFrame);
		game->setDeltaTime(deltaTime);

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		game->Update( deltaTime );

		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		game->Render();

		// Swap the screen buffers
		glfwSwapBuffers( window );
	}

	if( game )
		game->Clean();
	Game::DestroyInstance();

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();

	return 0;
}


void key_callback( GLFWwindow* window, int key, int scancode, int action, int mode )
{
	//if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
	//	glfwSetWindowShouldClose( window, GL_TRUE );

	if( Game::getInstance() && Game::getInstance()->getInput() )
	{
		if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
			Game::getInstance()->getInput()->setKey( KeyCode::Key_Escape, true );
		else if( key == GLFW_KEY_ESCAPE && (action == GLFW_RELEASE || action == GLFW_REPEAT) ){
			Game::getInstance()->getInput()->setKey( KeyCode::Key_Escape, false );
			Game::getInstance()->ClearProcessedKey( KeyCode::Key_Escape );
		}

		if( key == GLFW_KEY_SPACE && action == GLFW_PRESS )
			Game::getInstance()->getInput()->setKey( KeyCode::Key_Space, true );
		else if( key == GLFW_KEY_SPACE && (action == GLFW_RELEASE || action == GLFW_REPEAT) ){
			Game::getInstance()->getInput()->setKey( KeyCode::Key_Space, false );
			Game::getInstance()->ClearProcessedKey( KeyCode::Key_Space );
		}

		if( key == GLFW_KEY_ENTER && action == GLFW_PRESS )
			Game::getInstance()->getInput()->setKey( KeyCode::Key_Enter, true );
		else if( key == GLFW_KEY_ENTER && (action == GLFW_RELEASE || action == GLFW_REPEAT) ){
			Game::getInstance()->getInput()->setKey( KeyCode::Key_Enter, false );
			Game::getInstance()->ClearProcessedKey( KeyCode::Key_Enter );
		}

		// directions
		if( key == GLFW_KEY_W && action == GLFW_PRESS )
			Game::getInstance()->getInput()->setKey( KeyCode::Key_W, true );
		else if( key == GLFW_KEY_W && action == GLFW_RELEASE ){
			Game::getInstance()->getInput()->setKey( KeyCode::Key_W, false );
			Game::getInstance()->ClearProcessedKey( KeyCode::Key_W );
		}

		if( key == GLFW_KEY_A && action == GLFW_PRESS )
			Game::getInstance()->getInput()->setKey( KeyCode::Key_A, true );
		else if( key == GLFW_KEY_A && action == GLFW_RELEASE ){
			Game::getInstance()->getInput()->setKey( KeyCode::Key_A, false );
			Game::getInstance()->ClearProcessedKey( KeyCode::Key_A );
		}

		if( key == GLFW_KEY_S && action == GLFW_PRESS )
			Game::getInstance()->getInput()->setKey( KeyCode::Key_S, true );
		else if( key == GLFW_KEY_S && action == GLFW_RELEASE ){
			Game::getInstance()->getInput()->setKey( KeyCode::Key_S, false );
			Game::getInstance()->ClearProcessedKey( KeyCode::Key_S );
		}

		if( key == GLFW_KEY_D && action == GLFW_PRESS )
			Game::getInstance()->getInput()->setKey( KeyCode::Key_D, true );
		else if( key == GLFW_KEY_D && action == GLFW_RELEASE ){
			Game::getInstance()->getInput()->setKey( KeyCode::Key_D, false );
			Game::getInstance()->ClearProcessedKey( KeyCode::Key_D );
		}

		if( key == GLFW_KEY_UP && action == GLFW_PRESS )
			Game::getInstance()->getInput()->setKey( KeyCode::Key_Up, true );
		else if( key == GLFW_KEY_UP && action == GLFW_RELEASE ){
			Game::getInstance()->getInput()->setKey( KeyCode::Key_Up, false );
			Game::getInstance()->ClearProcessedKey( KeyCode::Key_Up );
		}

		if( key == GLFW_KEY_DOWN && action == GLFW_PRESS )
			Game::getInstance()->getInput()->setKey( KeyCode::Key_Down, true );
		else if( key == GLFW_KEY_DOWN && action == GLFW_RELEASE ){
			Game::getInstance()->getInput()->setKey( KeyCode::Key_Down, false );
			Game::getInstance()->ClearProcessedKey( KeyCode::Key_Down );
		}

		if( key == GLFW_KEY_LEFT && action == GLFW_PRESS )
			Game::getInstance()->getInput()->setKey( KeyCode::Key_Left, true );
		else if( key == GLFW_KEY_LEFT && action == GLFW_RELEASE ){
			Game::getInstance()->getInput()->setKey( KeyCode::Key_Left, false );
			Game::getInstance()->ClearProcessedKey( KeyCode::Key_Left );
		}

		if( key == GLFW_KEY_RIGHT && action == GLFW_PRESS )
			Game::getInstance()->getInput()->setKey( KeyCode::Key_Right, true );
		else if( key == GLFW_KEY_RIGHT && action == GLFW_RELEASE ){
			Game::getInstance()->getInput()->setKey( KeyCode::Key_Right, false );
			Game::getInstance()->ClearProcessedKey( KeyCode::Key_Right );
		}
	}
}

void mouse_button_callback( GLFWwindow * window, int id, int action, int mod )
{
	if( Game::getInstance() && Game::getInstance()->getInput() )
	{
		if( id == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS )
			Game::getInstance()->getInput()->setMouseButton( MouseButton::Button_Left, true );
		else if( id == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE )
			Game::getInstance()->getInput()->setMouseButton( MouseButton::Button_Left, false );

		if( id == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS )
			Game::getInstance()->getInput()->setMouseButton( MouseButton::Button_Right, true );
		else if( id == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE )
			Game::getInstance()->getInput()->setMouseButton( MouseButton::Button_Right, false );

		if( id == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS )
			Game::getInstance()->getInput()->setMouseButton( MouseButton::Button_Middle, true );
		else if( id == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE )
			Game::getInstance()->getInput()->setMouseButton( MouseButton::Button_Middle, false );
	}
}

void cursor_callback( GLFWwindow * window, double posX, double posY )
{
	if( Game::getInstance() && Game::getInstance()->getInput() )
	{
		Game::getInstance()->getInput()->setMousePosition( posX, posY );
	}
}

void scroll_callback( GLFWwindow * window, double offsetX, double offsetY )
{
	if( Game::getInstance() && Game::getInstance()->getInput() )
	{
		Game::getInstance()->getInput()->setMouseScroll( offsetX, offsetY );
	}
}

void QuitApplication()
{
	glfwSetWindowShouldClose( glfwGetCurrentContext(), GL_TRUE );
}