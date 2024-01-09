#include <directxmath.h>
#include <stdio.h>

namespace MathUtils {
	DirectX::XMMATRIX MatrixLookAtLH(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 look, DirectX::XMFLOAT3 up, DirectX::XMFLOAT3 right)
	{
		DirectX::XMMATRIX i = DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX translation = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			-pos.x, -pos.y, -pos.z, 1.0f,
		};
		
		DirectX::XMMATRIX rotate = {
			right.x, up.x, look.x, 0.0f,
			right.y, up.y, look.y, 0.0f,
			right.z, up.z, look.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f,
		};
		
		return i * translation * rotate;
	}

	DirectX::XMMATRIX BuildRotation(DirectX::XMFLOAT3 axis, float theta)
	{
		float c = cos(theta);
		float s = sin(theta);

		DirectX::XMFLOAT3 tX = {
			axis.x* axis.x* (1 - c) + c,
			axis.x* axis.y* (1 - c) + axis.z * s,
			axis.x* axis.z* (1 - c) - axis.y * s,
		};

		DirectX::XMFLOAT3 tY = {
			axis.y* axis.x* (1 - c) - axis.z * s,
			axis.y* axis.y* (1 - c) + c,
			axis.y* axis.z* (1 - c) + axis.x * s,
		};

		DirectX::XMFLOAT3 tZ = {
			axis.z* axis.x* (1 - c) + axis.y * s,
			axis.z* axis.y* (1 - c) - axis.x * s,
			axis.z* axis.z* (1 - c) + c,
		};

		return {
			tX.x, tX.y, tX.z, 0.0f,
			tY.x, tY.y, tY.z, 0.0f,
			tZ.x, tZ.y, tZ.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f,
		};
	}

	void PrintMatrix(DirectX::XMMATRIX m)
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
}
