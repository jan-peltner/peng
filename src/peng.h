#ifndef PENG_API_H
	
#define PENG_API_H

#include <raylib.h>
#include <stddef.h>

#define PENG_ARRAY_LEN(array) (sizeof(array) / sizeof((array)[0]))

// init & destroy global context 

void startPeng(int winWidth, int winHeight, size_t particlesCount, size_t attractorCount);
void stopPeng();

// particles

void spawnParticleAt(size_t x, size_t y, Color lowVelColor, Color highVelColor);
void spawnParticlesRandom(const Color* lowVelColors, size_t lowVelColorsCount, const Color* highVelColors, size_t highVelColorsCount);
void spawnParticlesFromImage(const Image* img, Vector2 origin, size_t sampleStride);

// attractors

void createMouseAttractor(float gravity, float rotationCoeff);

// physics 

void toggleAttractorForce();
void toggleFrictionForce();
void toggleRepellentForce();
void toggleParticlesFrozen();
void runUpdate(float dt);

// rendering

void drawParticles();
void drawForcesUi(int x, int y, int gap, int fontSize, Color fontColor);

// controls

void handleKeyInputs();

#endif // PENG_API_H
