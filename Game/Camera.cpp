#include "Camera.h"

Camera::Camera(uint width, uint height)
	: position(Vector3::Zero()),
	direction(Vector3::UnitZ()),
	up(Vector3::UnitY()),
	old_mx(0.0), old_my(0.0),
	width(width), height(height)
{
}

Camera::~Camera()
{
}

const Matrix4 &Camera::GetViewMatrix() const
{
	return view_matrix;
}

const Matrix4 &Camera::GetProjectionMatrix() const
{
	return proj_matrix;
}

void Camera::UpdateMatrices()
{
	MatrixUtil::ToLookAt(view_matrix, position, position + direction, up);
	MatrixUtil::ToPerspective(proj_matrix, 45, width, height, 0.1f, 50.0f);
}

void Camera::UpdateMouse(InputManager *input_mgr, double delta_time)
{
	const double sensitivity = 0.8 * delta_time;

	mouse_attached = !input_mgr->keys[342];
	

	if (!mouse_attached) {
		return;
	}

	// if not resetting mouse:
	// direction.x = -1 * ((input_mgr->mx / 1080) * 2 - 1);
	// direction.y = -1 * ((input_mgr->my / 720) * 2 - 1);

	double x_diff = input_mgr->mx - (width / 2);
	double y_diff = input_mgr->my - (height / 2);

	dir_cross_y = direction;
	dir_cross_y.Cross(up);

	direction.Rotate(up, MathUtil::DegToRad(x_diff * sensitivity));
	direction.Normalize();
	direction.Rotate(dir_cross_y, MathUtil::DegToRad(y_diff * sensitivity));
	direction.Normalize(); 
	
	if (direction.y > 0.97f || direction.y < -0.97f) {
		y_diff *= -1.0f;
		direction.Rotate(dir_cross_y, MathUtil::DegToRad(y_diff * sensitivity));
		direction.Normalize();
	}

	input_mgr->MoveMouse(width / 2, height / 2);
}

void Camera::UpdateMovement(InputManager *input_mgr, double delta_time)
{
	const double max_speed = 2;
	const double default_speed = 0.5;

	// shift speeds things up a notch
	if (input_mgr->keys[340]) {
		speed = max_speed * delta_time;
	} else {
		speed = default_speed * delta_time;
	}

	if (input_mgr->keys['W']) {
		position += direction * speed;
	} else if (input_mgr->keys['S']) {
		position -= direction * speed;
	}

	if (input_mgr->keys['A']) {
		position -= dir_cross_y * speed;
	} else if (input_mgr->keys['D']) {
		position += dir_cross_y * speed;
	}

}