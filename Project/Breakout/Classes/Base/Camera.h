#ifndef __Camera_H__
#define __Camera_H__

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum MoveDirection
{
	Forward = 0,
	Backward,
	Left,
	Right
};

class Camera
{
public:
	glm::vec3 pos;
	glm::vec3 up;
	glm::vec3 front;
	glm::vec3 right;
	float yaw, pitch;
	float fov;

public:
	Camera();
	Camera( glm::vec3 pos, float yaw, float pitch, float fov );

	glm::mat4 getViewMatrix();

	glm::mat4 getOrtho( float left, float right, float bottom, float top);
	glm::mat4 getPerspective( float ratio );

	void processMove( MoveDirection dir, float deltaTime );

	void processRotate( float deltaPosX, float deltaPosY );

	void processScroll( double offsetX, double offsetY );

private:
	// Calculates the front vector from the Camera's (updated) Eular Angles
	void updateCameraVectors();
};

#endif // !__Camera_H__

