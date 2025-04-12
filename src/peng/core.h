#ifndef PENG_CORE_H
	
#define PENG_CORE_H

#include <pthread.h>
#include <raylib.h>
#include <stddef.h>

#define FRICTION_SCALAR 0.98f
#define REPELLING_RADIUS 10
#define VELOCITY_VEC_MAX_LENGTH 120.0f
#define THREAD_COUNT 12

typedef struct {
	Vector2 pos; 
	Vector2 vel;
	float velLen;
	Vector2 accel;
	Color color;
} Particle;

typedef struct {
	Vector2 pos;
	float gravity;
	float rotationCoeff;
	bool isActive;
} Attractor;

typedef struct {
	size_t start;
	size_t end;
	float dt;
} ThreadData;

static struct {
	// window & system
	int winWidth;
	int winHeight;
	int winArea;
	int winDiag;
	size_t frameCounter;
	// particles
	Particle* particles;
	size_t particleCount;
	size_t particleCap;
	Color slowColor;
	Color fastColor;
	// attractors
	Attractor* attractors;
	Attractor* mouseAttractor;
	size_t attractorCount;
	size_t attractorCap;
	char* oMap;
	// config
	bool useAttractorForce;
	bool useFrictionForce;
	bool useRepellentForce;
	bool useMouseAttractor;
	bool isPhysicsPaused;
	// threads
	pthread_t threads[THREAD_COUNT];
	ThreadData threadData[THREAD_COUNT];
} ENGINE;

#endif // PENG_CORE_H
