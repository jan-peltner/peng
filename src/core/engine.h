#ifndef ENGINE_H

#define ENGINE_H

#include <pthread.h>
#include <stddef.h>
#include <raylib.h>
#include "../peng.h"

#define THREAD_COUNT 12

typedef struct {
	Vector2 pos; 
	Vector2 vel;
	float velLen;
	Vector2 accel;
	Color lowVelColor;
	Color highVelColor;
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
} Attractor;

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
	size_t frameCounter;
	// particles
	Particle* particles;
	size_t particleCount;
	size_t particleCap;
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
	bool particlesFrozen;
	bool isPhysicsPaused;
	// GPU rendering 
	Color* pixelBuffer;
	Texture2D particleTexture;
	// threads
	pthread_t threads[THREAD_COUNT];
	ThreadData threadData[THREAD_COUNT];
} ENGINE_t;

extern ENGINE_t ENGINE;

void oMapClear(char* oMap);
void oMapSet(Particle* p, char* oMap);
void toggleAttractorForce();
void toggleFrictionForce();
void toggleRepellentForce();
void toggleParticlesFrozen();


#endif // ENGINE_H
