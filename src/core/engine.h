#ifndef ENGINE_H

#define ENGINE_H

#include <pthread.h>
#include <stddef.h>
#include <raylib.h>
#include "entities.h"
#include "scheduler.h"
#include "../peng.h"

#define THREAD_COUNT 12

typedef struct {
	size_t start;
	size_t end;
	float dt;
} ThreadData;

typedef struct {
	// window & system
	int winWidth;
	int winHeight;
	int winArea;
	int winDiag;
	float time;
	// particles
	Particle* particles;
	size_t particleCount;
	size_t particleCap;
	char* oMap;
	// attractors
	Attractor* attractors;
	Attractor* mouseAttractor;
	size_t attractorCount;
	size_t attractorCap;
	// lights 
	Light* lights;
	Light* mouseLight;
	size_t lightCount;
	size_t lightCap;
	// config
	bool useAttractorForce;
	bool useFrictionForce;
	bool useRepellentForce;
	bool useMouseAttractor;
	bool useMouseLight;
	bool particlesFrozen;
	bool renderUi;
	// GPU rendering 
	Color* pixelBuffer;
	Texture2D particleTexture;
	// threads
	pthread_t threads[THREAD_COUNT];
	ThreadData threadData[THREAD_COUNT];
	// scheduler
	Scheduler scheduler;
} ENGINE_t;

extern ENGINE_t ENGINE;

void oMapClear(char* oMap);
void oMapSet(Particle* p, char* oMap);
void toggleAttractorForce();
void toggleFrictionForce();
void toggleRepellentForce();
void toggleParticlesFrozen();

#endif // ENGINE_H
