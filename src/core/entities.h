#ifndef ENTITIES_H

#define ENTITIES_H

#include <raylib.h>
#include "../peng.h"

#define KEYFRAME_MAX_COUNT 10

typedef struct {
	float force;
	float duration;
} Keyframe; 

typedef struct {
	Vector2 pos; 
	Vector2 vel;
	Vector2 accel;
	Vector2 kfPos[KEYFRAME_MAX_COUNT];
	size_t kfIndex;
	float brightness;
	Color lowColor;
	Color highColor;
	Color lerpedColor;
} Particle;

typedef struct {
	AttractorId id;	
	Vector2 pos;
	float gravity;
	float rotationCoeff;
	bool isActive;
	
	// path animation
	const Vector2* path;
	size_t pathLen;
	float totalTime;
	float elapsedTime;
	bool isAnimated;
	bool isLooping;
	LoopMode loopMode;
} Attractor;

typedef struct {
	LightId id;
	Vector2 pos;
	float intensity;
	bool isActive;
	
	// path animation
	const Vector2* path;
	size_t pathLen;
	float totalTime;
	float elapsedTime;
	bool isAnimated;
	bool isLooping;
	LoopMode loopMode;
} Light;

#endif // ENTITIES_H
