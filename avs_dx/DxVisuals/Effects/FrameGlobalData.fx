cbuffer FrameGlobalData : register(b0)
{
	// Detect if it is currently a beat
    bool beat : packoffset(c0.x);

	// Zero-based frame index since when the visualization started
    uint currentFrame : packoffset(c0.y);

	// Time in milliseconds since last boot
    uint getTickCount : packoffset(c0.z);
}

// Texture of size 576 * 4, signed bytes i.e. DXGI_FORMAT_R8_SNORM
// First two lines are spectrum, left/right channel, second two lines are wave.
// Y texture coordinates: 0.125 left channel spectrum, 0.25 center channel spectrum, 0.375 right channel spectrum, 0.625 left channel waveform, 0.75 center channel waveform, 0.875 right channel waveform
Texture2D<float> texVisData : register(t0);

SamplerState sampleBilinear : register(s0);

inline float sampleVisData(float2 location)
{
    return texVisData.Sample(sampleBilinear, location);
}

#ifndef AVS_SHADER
#define EFFECT_STATE 0
#define AVS_SCREEN_WIDTH 1920
#define AVS_SCREEN_HEIGHT 1080
#endif

inline float2 screenSizeInv()
{
    return float2( 1.0 / AVS_SCREEN_WIDTH, 1.0 / AVS_SCREEN_HEIGHT );
}