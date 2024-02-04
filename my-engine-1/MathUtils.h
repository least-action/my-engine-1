#pragma once

#include <directxmath.h>

namespace MathUtils {
	const float PI = 3.14159265358979323846f;
	const float SCALE = 1.0f;

	void PrintXMMATRIX(DirectX::XMMATRIX m);
	void PrintFloat3(DirectX::XMFLOAT3 f);

	class Matrix {
	public:
		float _00, _01, _02, _03;
		float _10, _11, _12, _13;
		float _20, _21, _22, _23;
		float _30, _31, _32, _33;
		Matrix();
		Matrix(
			float _00, float _01, float _02, float _03,
			float _10, float _11, float _12, float _13,
			float _20, float _21, float _22, float _23,
			float _30, float _31, float _32, float _33
		);

		Matrix operator*(const Matrix &a);
		Matrix Transposed();
	};

	Matrix IdendityMatrix();
	
	class Vector {
	public:
		float x, y, z, w = 0.0f;
		Vector();
		Vector(float x, float y, float z);
		Vector Cross(const Vector& a);
		float Dot(const Vector& a);
		Vector operator*(const Matrix& a);
		Vector operator*(float a);
		Vector operator-(const Vector& v);
		Vector operator+(const Vector& v);
		float length();
		Vector Normalized();
	};

	class Point {
	public:
		float x, y, z, w = 1.0f;
		Point();
		Point(float x, float y, float z);
		Point operator+(const Vector& v);
		Vector operator-(const Point& v);
	};

	Matrix MatrixLookAtLH(Point pos, Vector look, Vector up, Vector right);
	Matrix MatrixPerspectiveForLH(float fieldOfViewAngleY, float aspectRatio, float nearZ, float farZ);
	Matrix InvMatrixPerspectiveForLH(float fieldOfViewAngleY, float aspectRatio, float nearZ, float farZ);

	Matrix BuildRotation(const Vector& axis, float theta);

	void PrintMatrix(const Matrix& m);
	void PrintVector(const Vector& v);

	struct TextCoord {
		float x;
		float y;
		TextCoord();
		TextCoord(float x, float y);
	};
}
