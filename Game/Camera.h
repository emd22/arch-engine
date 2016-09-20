#pragma once
#include "Math/vector3.h"
#include "Math/matrix4.h"
#include "Math/matrix_util.h"
#include "InputManager.h"
#include "Types.h"

class Camera
{
public:
	Camera(uint width, uint height);
	~Camera();

	inline const Vector3 &GetPosition() const { return position; }

	const Matrix4 &GetViewMatrix() const;
	const Matrix4 &GetProjectionMatrix() const;

	void UpdateMatrices();
	void UpdateMouse(InputManager*input_mgr, double delta_time);
	void UpdateMovement(InputManager *input_mgr, double delta_time);

protected:
	Vector3 position;
	Vector3 direction;
	Vector3 dir_cross_y;
	Vector3 up;

	Matrix4 view_matrix;
	Matrix4 proj_matrix;

	double old_mx;
	double old_my;

	uint width;
	uint height;

	double speed = 0.0;

	bool mouse_attached = true;
};

