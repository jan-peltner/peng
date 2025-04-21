#ifndef PENG_API_H
	
#define PENG_API_H

#include <raylib.h>
#include <stddef.h>
#include <stdint.h>

#define PENG_ARRAY_LEN(array) (sizeof(array) / sizeof((array)[0]))

typedef int32_t EntityId;
typedef EntityId AttractorId;
typedef EntityId LightId;

typedef void (*EventFn)(void*);

typedef enum {
	LOOP_WRAP,	// p1,p2,p3,p1,p2,p3,p1...
	LOOP_PINGPONG	// p1,p2,p3,p2,p1,p2,p3...
} LoopMode;

typedef struct {
	float dispatchAt;
	EventFn fn;
	void* eventData;
} Event;

// system 

void startPeng(int winW, int winH, size_t particleCap, size_t attractorCap, size_t lightCap);
void stopPeng();
void runUpdate(float dt);

// particles

void spawnParticleAt(size_t x, size_t y, Color lowVelColor, Color highVelColor);
void spawnParticlesRandom(const Color* lowVelColors, size_t lowVelColorsCount, const Color* highVelColors, size_t highVelColorsCount);
void spawnParticlesFromImage(Image* img, Vector2 origin, size_t sampleStride, Color lowVelColor);

// attractors

AttractorId createMouseAttractor(float gravity, float rotationCoeff);
AttractorId spawnStaticAttractor(Vector2 origin, float gravity, float rotationCoeff);
AttractorId spawnAnimatedAttractor(const Vector2* animationPath, size_t pathLen, float totalAnimationTime, bool isLooping, LoopMode loopMode, float gravity, float rotationCoeff);
void toggleAttractor(AttractorId id);

// lights

LightId createMouseLight(float intensity);
LightId spawnStaticLight(Vector2 origin, float intensity);
LightId spawnAnimatedLight(const Vector2* animationPath, size_t pathLen, float totalAnimationTime, bool isLooping, LoopMode loopMode, float gravity);
void toggleLight(LightId id);

// forces 

void toggleAttractorForce();
void toggleFrictionForce();
void toggleRepellentForce();
void toggleParticlesFrozen();

// rendering

void drawParticles();
void drawAttractors();
void drawForcesUi(int x, int y, int gap, int fontSize, Color fontColor);
void toggleRenderUi();

// controls

void handleKeyInputs();

// scheduler

void scheduleEvents(const Event* events, size_t amount);

#endif // PENG_API_H
