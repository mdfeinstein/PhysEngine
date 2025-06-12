#pragma once
#include <stdlib.h>

float randRange(float low, float high) {
  return static_cast<float>(std::rand())/ RAND_MAX * (high - low) + low;
};