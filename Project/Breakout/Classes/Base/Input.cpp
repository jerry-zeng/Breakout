#include "Input.h"

Input::Input()
{
	clearKeys();
	clearMouseButtons();
	clearMouseScrolls();
	clearMousePosition();
}

bool Input::getKey( KeyCode key )
{
	if( key != KeyCode::Key_Count )
		return keys[key];
	return false;
}
void Input::setKey( KeyCode key, bool value )
{
	if( key != KeyCode::Key_Count )
		keys[key] = value;
}
void Input::clearKeys()
{
	for( int i = 0; i < (int)KeyCode::Key_Count; i++ )
		keys[i] = false;
}

bool Input::getMouseButton( MouseButton btn )
{
	if( btn != MouseButton::Button_Count )
		return mouseButtons[btn];
	return false;
}
void Input::setMouseButton( MouseButton btn, bool value )
{
	if( btn != MouseButton::Button_Count )
		mouseButtons[btn] = value;
}
void Input::clearMouseButtons()
{
	for( int i = 0; i < (int)MouseButton::Button_Count; i++ )
		mouseButtons[i] = false;
}

