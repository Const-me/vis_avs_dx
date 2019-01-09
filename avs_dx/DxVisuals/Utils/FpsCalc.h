#pragma once
#include "DynamicArray.hpp"

class FpsCalc
{
	static constexpr uint8_t measures = 25;
	DynamicArray<uint32_t, measures> m_buffer;
	uint32_t totalSum = 0;
	uint32_t nextIndex = 0;
	uint64_t qpcPrev = 0;

public:

	float update();
};