#ifndef ENTITIES_H

#define ENTITIES_H

#include <raylib.h>
#include "../peng.h"

typedef struct {
	Vector2 pos; 
	Vector2 vel;
	Vector2 accel;
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
