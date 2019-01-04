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
}