struct FountainPoint
{
	// Normalized XY direction, never changes
    float2 xyDir;
	// Particle color, set at birth and never changes
    float3 color;
	// Position
    float3 position;
	// Speed
    float3 speed;
};

// Radial jets in the fountain
#define NUM_ROT_DIV 32

// Generations count. Each frame, single generation is recycled into a new one, i.e. the total lifetime of a particle is NUM_ROT_HEIGHT / FPS seconds.
#define NUM_ROT_HEIGHT 256