#ifndef PENG_API_H
	
#define PENG_API_H

#include <raylib.h>
#include <stddef.h>

typedef struct {
	bool useAttractorForce;
	bool useFrictionForce;
	bool useRepellentForce;
} ForcesCfg;

// init & destroy global context 

void startPeng(int winWidth, int winHeight, size_t particlesCount, size_t attractorCount, ForcesCfg cfg);
void stopPeng();

// particles

void spawnParticleAt(size_t x, size_t y, Color color);
void spawnParticlesRandom();

// attractors

void createMouseAttractor(float gravity);
void toggleAttractors();

// updates

void runPhysicsUpdate(float dt); 

// rendering

void drawParticles();

#endif // PENG_API_H
