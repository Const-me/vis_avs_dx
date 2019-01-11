cbuffer FrameGlobalData : register(b0)
{
	// Detect if it is currently a beat
    bool beat : packoffset(c0.x);

	// Zero-based frame index since when the visualization started
    uint currentFrame : packoffset(c0.y);

	// Time in milliseconds since last boot
    uint getTickCount : packoffset(c0.z);

	// Time in seconds since last frame
    float deltaTime : packoffset(c0.w);

	// True if g_line_blend_mode is normal or additive i.e. shaders should output alpha channel. False if it e.g. XOR, so shaders instead should output sharp edges and discard() invisible pixels.
    bool lineModeAllowAlpha : packoffset(c1.x);
}