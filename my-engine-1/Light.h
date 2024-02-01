#pragma once
#include <stdint.h>
#include "MathUtils.h"

struct Light {
	MathUtils::Point Pos;
	MathUtils::Point Color;
	float Intensity;
	float _buffer0[3];
	
	uint32_t Type;
	uint32_t _buffer1[3];
};
