#pragma once

struct ConvolutionKernel
{
	uint8_t radius;
	CStringA values;
};

extern const ConvolutionKernel smallBlurKernel, mediumBlurKernel, largeBlurKernel;