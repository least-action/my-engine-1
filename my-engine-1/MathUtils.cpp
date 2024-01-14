#include "MathUtils.h"
#include <directxmath.h>
#include <stdio.h>

namespace MathUtils {
	void PrintXMMATRIX(DirectX::XMMATRIX m)
	{
		printf(
			"%f %f %f %f\n"
			"%f %f %f %f\n"
			"%f %f %f %f\n"
			"%f %f %f %f\n",
			m.r[0].m128_f32[0], m.r[0].m128_f32[1], m.r[0].m128_f32[2], m.r[0].m128_f32[3],
			m.r[1].m128_f32[0], m.r[1].m128_f32[1], m.r[1].m128_f32[2], m.r[1].m128_f32[3],
			m.r[2].m128_f32[0], m.r[2].m128_f32[1], m.r[2].m128_f32[2], m.r[2].m128_f32[3],
			m.r[3].m128_f32[0], m.r[3].m128_f32[1], m.r[3].m128_f32[2], m.r[3].m128_f32[3]
		);
	}
	void PrintFloat3(DirectX::XMFLOAT3 f)
	{
		printf("%f %f %f\n", f.x, f.y, f.z);
	}

	Point::Point() {}

	Point::Point(float x, float y, float z) : x(x), y(y), z(z) {}

	Vector::Vector(float x, float y, float z) : x(x), y(y), z(z) {}

	Vector Vector::Cross(const Vector &a)
	{
		return {
			y * a.z - z * a.y,
			z * a.x - x * a.z,
			x * a.y - y * a.x
		};
	}

	float Vector::Dot(const Vector& a)
	{
		return this->x * a.x + this->y * a.y + this->z * a.z;
	}
	
	Vector Vector::operator*(const Matrix& a)
	{
		return {
			x * a._00 + y * a._10 + z * a._20 + w * a._30,
			x * a._01 + y * a._11 + z * a._21 + w * a._31,
			x * a._02 + y * a._12 + z * a._22 + w * a._32
		};
	}

	Vector Vector::operator*(float a)
	{
		return {this->x * a, this->y * a, this->z * a};
	}

	Vector Vector::operator-(const Vector& v)
	{
		return {x - v.x, y - v.y, z - v.z};
	}

	Vector Vector::operator+(const Vector& v)
	{
		return { x + v.x, y + v.y, z + v.z };
	}

	float Vector::length()
	{
		return sqrt(x * x + y * y + z * z);
	}

	Matrix::Matrix() {}

	Matrix::Matrix(
		float _00, float _01, float _02, float _03,
		float _10, float _11, float _12, float _13,
		float _20, float _21, float _22, float _23,
		float _30, float _31, float _32, float _33
	) : _00(_00), _01(_01), _02(_02), _03(_03),
		_10(_10), _11(_11), _12(_12), _13(_13),
		_20(_20), _21(_21), _22(_22), _23(_23),
		_30(_30), _31(_31), _32(_32), _33(_33)
	{}
	
	Matrix Matrix::operator*(const Matrix &a)
	{
		return {
			_00 * a._00 + _01 * a._10 + _02 * a._20 + _03 * a._30,
			_00 * a._01 + _01 * a._11 + _02 * a._21 + _03 * a._31,
			_00 * a._02 + _01 * a._12 + _02 * a._22 + _03 * a._32,
			_00 * a._03 + _01 * a._13 + _02 * a._23 + _03 * a._33,

			_10 * a._00 + _11 * a._10 + _12 * a._20 + _13 * a._30,
			_10 * a._01 + _11 * a._11 + _12 * a._21 + _13 * a._31,
			_10 * a._02 + _11 * a._12 + _12 * a._22 + _13 * a._32,
			_10 * a._03 + _11 * a._13 + _12 * a._23 + _13 * a._33,

			_20 * a._00 + _21 * a._10 + _22 * a._20 + _23 * a._30,
			_20 * a._01 + _21 * a._11 + _22 * a._21 + _23 * a._31,
			_20 * a._02 + _21 * a._12 + _22 * a._22 + _23 * a._32,
			_20 * a._03 + _21 * a._13 + _22 * a._23 + _23 * a._33,

			_30 * a._00 + _31 * a._10 + _32 * a._20 + _33 * a._30,
			_30 * a._01 + _31 * a._11 + _32 * a._21 + _33 * a._31,
			_30 * a._02 + _31 * a._12 + _32 * a._22 + _33 * a._32,
			_30 * a._03 + _31 * a._13 + _32 * a._23 + _33 * a._33
		};
	}

	Matrix Matrix::Transposed()
	{
		return {
			_00, _10, _20, _30,
			_01, _11, _21, _31,
			_02, _12, _22, _32,
			_03, _13, _23, _33
		};
	}

	Matrix IdendityMatrix()
	{
		return {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	Point Point::operator+(const Vector& v)
	{
		return {x + v.x, y + v.y, z + v.z};
	}

	Matrix MatrixLookAtLH(MathUtils::Point pos, MathUtils::Vector look, MathUtils::Vector up, MathUtils::Vector right)
	{
		Matrix translation = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			-pos.x, -pos.y, -pos.z, 1.0f,
		};

		Matrix rotate = {
			right.x, up.x, look.x, 0.0f,
			right.y, up.y, look.y, 0.0f,
			right.z, up.z, look.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f,
		};

		return translation * rotate;
	}

	Matrix MatrixPerspectiveForLH(float fieldOfViewAngleY, float aspectRatio, float nearZ, float farZ)
	{
		float tanHalf = tan(fieldOfViewAngleY / 2.0f);
		return {
			float(1.0f / (aspectRatio * tanHalf)), 0.0f, 0.0f, 0.0f,
			0.0f, float(1.0f / tanHalf), 0.0f, 0.0f,
			0.0f, 0.0f, farZ / (farZ - nearZ), 1.0f,
			0.0f, 0.0f, -(nearZ * farZ) / (farZ - nearZ), 0.0f,
		};
	}

	Matrix BuildRotation(const Vector &axis, float theta)
	{
		float c = cos(theta);
		float s = sin(theta);

		Point tX = {
			axis.x * axis.x * (1 - c) + c,
			axis.x * axis.y * (1 - c) + axis.z * s,
			axis.x * axis.z * (1 - c) - axis.y * s,
		};

		Point tY = {
			axis.y * axis.x * (1 - c) - axis.z * s,
			axis.y * axis.y * (1 - c) + c,
			axis.y * axis.z * (1 - c) + axis.x * s,
		};

		Point tZ = {
			axis.z * axis.x * (1 - c) + axis.y * s,
			axis.z * axis.y * (1 - c) - axis.x * s,
			axis.z * axis.z * (1 - c) + c,
		};

		return {
			tX.x, tX.y, tX.z, 0.0f,
			tY.x, tY.y, tY.z, 0.0f,
			tZ.x, tZ.y, tZ.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f,
		};
	}

	void PrintMatrix(const Matrix &m)
	{
		printf(
			"%f %f %f %f\n"
			"%f %f %f %f\n"
			"%f %f %f %f\n"
			"%f %f %f %f\n",
			m._00, m._01, m._02, m._03,
			m._10, m._11, m._12, m._13,
			m._20, m._21, m._22, m._23,
			m._30, m._31, m._32, m._33
		);
	}

	void PrintVector(const Vector& v)
	{
		printf("%f %f %f\n", v.x, v.y, v.z);
	}
}
