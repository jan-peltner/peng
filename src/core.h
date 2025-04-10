#ifndef PENG_CORE_H
	
#define PENG_CORE_H

#include <raylib.h>
#include <stddef.h>

#define FRICTION_SCALAR 0.98f
#define REPELLING_RADIUS 10
#define VELOCITY_VEC_MAX_LENGTH 120.0f

typedef struct {
	Vector2 pos; 
	Vector2 vel; 
	Vector2 accel;
	Color color;
} Particle;

typedef struct {
	Vector2 pos;
	float gravity;
	bool isActive;
} Attractor;

static struct {
	int winWidth;
	int winHeight;
	int winArea;
	int winDiag;
	size_t frameCounter;
	Particle* particles;
	size_t particleCount;
	size_t particleCap;
	Attractor* attractors;
	Attractor* mouseAttractor;
	size_t attractorCount;
	size_t attractorCap;
	char* oMap;
	bool useAttractorForce;
	bool useFrictionForce;
	bool useRepellentForce;
	bool useMouseAttractor;
	bool isPhysicsPaused;
} ENGINE;

#endif // PENG_CORE_H
