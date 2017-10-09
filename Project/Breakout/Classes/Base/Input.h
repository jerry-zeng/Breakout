#pragma once
#ifndef Input_H
#define Input_H

#include <glm/glm.hpp>

enum KeyCode
{
	Key_Up = 0,
	Key_Down,
	Key_Left,
	Key_Right,
	Key_W,
	Key_S,
	Key_A,
	Key_D,
	Key_Space,
	Key_Enter,
	Key_Escape,
	Key_Count,
};

enum MouseButton
{
	Button_Left = 0,
	Button_Right,
	Button_Middle,
	Button_Count,
};


class Input
{
public:
	Input();

	bool getKey(KeyCode key);
	void setKey(KeyCode key, bool value);
	void clearKeys();

	bool getMouseButton(MouseButton btn);
	void setMouseButton(MouseButton btn, bool value);
	void clearMouseButtons();

	inline const glm::vec2& getMouseScroll() { return mouseScrolls; }
	inline void setMouseScroll( float x, float y ) { mouseScrolls.x = x; mouseScrolls.y = y; }
	inline void clearMouseScrolls() { mouseScrolls.x = 0; mouseScrolls.y = 0; }

	inline const glm::vec2& getMousePosition() { return mousePosition; }
	inline void setMousePosition( float x, float y ) { mousePosition.x = x; mousePosition.y = y; }
	inline void clearMousePosition() { mousePosition.x = 0; mousePosition.y = 0; }

private:
	bool keys[KeyCode::Key_Count];
	bool mouseButtons[MouseButton::Button_Count];

	glm::vec2 mouseScrolls;
	glm::vec2 mousePosition;
};

#endif // !Input_H