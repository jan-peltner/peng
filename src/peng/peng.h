#ifndef PENG_API_H
	
#define PENG_API_H

#include <raylib.h>
#include <stddef.h>

// init & destroy global context 

void startPeng(int winWidth, int winHeight, size_t particlesCount, size_t attractorCount);
void stopPeng();

// particles

void spawnParticleAt(size_t x, size_t y);
void spawnParticlesRandom();
void setSlowParticleColors(Color* colors, size_t count);
void setFastParticleColors(Color* colors, size_t count);

// attractors

void createMouseAttractor(float gravity, float rotationCoeff);
void toggleAttractors();

// physics 

void toggleAttractorForce();
void toggleFrictionForce();
void toggleRepellentForce();
void runUpdate(float dt);

// rendering

void drawParticles();

#endif // PENG_API_H
