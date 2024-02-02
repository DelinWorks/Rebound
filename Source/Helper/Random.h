#pragma once

#include "cocos2d.h"

namespace Random
{
	inline int maxInt(int max) {
		return cocos2d::RandomHelper::random_int(0, max);
	}

	inline int rangeInt(int min, int max) {
		return cocos2d::RandomHelper::random_int(min, max);
	}

	inline float float01() {
		return cocos2d::RandomHelper::random_real<float>(0, 1);
	}

	inline float maxFloat(float max) {
		return cocos2d::RandomHelper::random_real<float>(0, max);
	}

	inline float rangeFloat(float min, float max) {
		return cocos2d::RandomHelper::random_real<float>(min, max);
	}
}
