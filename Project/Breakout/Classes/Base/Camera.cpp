#include "Camera.h"

const glm::vec3 DefaultPos = glm::vec3( 0.0f, 0.0f, 0.0f );
const glm::vec3 DefaultWorldUp = glm::vec3( 0.0f, 1.0f, 0.0f );
const float DefaultYaw = -90.0f;
const float DefaultPitch = 0.0f;
const float DefaultFov = 60.0f;

static const float CameraSpeed = 100.0f;
static const float Sensitivity = 0.05f;


Camera::Camera()
{
	this->pos = DefaultPos;
	this->yaw = DefaultYaw;
	this->pitch = DefaultPitch;
	this->fov = DefaultFov;

	updateCameraVectors();
}

Camera::Camera( glm::vec3 pos, float yaw, float pitch, float fov )
{
	this->pos = pos;
	this->yaw = yaw;
	this->pitch = pitch;
	this->fov = fov;

	updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt( pos, pos + front, up );
}

glm::mat4 Camera::getOrtho( float left, float right, float bottom, float top )
{
	return glm::ortho<float>( left, right, bottom, top, -1.0, 10.0 );
}

glm::mat4 Camera::getPerspective( float ratio )
{
	return glm::perspective( glm::radians( fov ), ratio, 0.1f, 100.0f );
}

void Camera::processMove( MoveDirection dir, float deltaTime )
{
	if( dir == MoveDirection::Forward )
		pos += front * CameraSpeed * deltaTime;
	if( dir == MoveDirection::Backward )
		pos += -front * CameraSpeed * deltaTime;
	if( dir == MoveDirection::Left )
		pos += -glm::normalize( glm::cross( front, up ) ) * CameraSpeed * deltaTime;
	if( dir == MoveDirection::Right )
		pos += glm::normalize( glm::cross( front, up ) ) * CameraSpeed * deltaTime;
}

void Camera::processRotate( float deltaPosX, float deltaPosY )
{
	yaw += deltaPosX * Sensitivity;
	pitch += -deltaPosY * Sensitivity;

	if( pitch > 89.0f )
		pitch = 89.0f;
	if( pitch < -89.0f )
		pitch = -89.0f;

	updateCameraVectors();
}

void Camera::processScroll( double offsetX, double offsetY )
{
	fov += -offsetY;

	if( fov < 20.0f )
		fov = 20.0f;
	if( fov > 80.0f )
		fov = 80.0f;
}

void Camera::updateCameraVectors()
{
	// Calculate the new Front vector
	front.x = cos( glm::radians( yaw ) ) * cos( glm::radians( pitch ) );
	front.y = sin( glm::radians( pitch ) );
	front.z = sin( glm::radians( yaw ) ) * cos( glm::radians( pitch ) );
	front = glm::normalize( front );

	// Also re-calculate the Right and Up vector
	right = glm::normalize( glm::cross( front, DefaultWorldUp ) );  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	up = glm::normalize( glm::cross( right, front ) );
}