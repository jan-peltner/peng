#ifndef PENG_API_H
	
#define PENG_API_H

#include <raylib.h>
#include <stddef.h>
#include <stdint.h>

#define PENG_ARRAY_LEN(array) (sizeof(array) / sizeof((array)[0]))

typedef int32_t AttractorId;
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

// init & destroy global context 

void startPeng(int winWidth, int winHeight, size_t particlesCount, size_t attractorCount);
void stopPeng();

// particles

void spawnParticleAt(size_t x, size_t y, Color lowVelColor, Color highVelColor);
void spawnParticlesRandom(const Color* lowVelColors, size_t lowVelColorsCount, const Color* highVelColors, size_t highVelColorsCount);
void spawnParticlesFromImage(Image* img, Vector2 origin, size_t sampleStride, Color lowVelColor);

// attractors

AttractorId createMouseAttractor(float gravity, float rotationCoeff);
AttractorId spawnStaticAttractor(Vector2 origin, float gravity, float rotationCoeff);
AttractorId spawnAnimatedAttractor(const Vector2* animationPath, size_t pathLen, float totalAnimationTime, bool isLooping, LoopMode loopMode, float gravity, float rotationCoeff);
void toggleAttractor(AttractorId id);

// physics 

void toggleAttractorForce();
void toggleFrictionForce();
void toggleRepellentForce();
void toggleParticlesFrozen();
void runUpdate(float dt);

// rendering

void drawParticles();
void drawAttractors();
void drawForcesUi(int x, int y, int gap, int fontSize, Color fontColor);

// controls

void handleKeyInputs();

// scheduler

void scheduleEvents(const Event* events, size_t amount);

#endif // PENG_API_H
