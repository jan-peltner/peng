#include "engine.h"
#include "raymath.h"
#include <stdlib.h>

void spawnParticleAt(size_t x, size_t y, Color lowVelColor, Color highVelColor) {
	if (ENGINE.particleCount >= ENGINE.particleCap) return;
	
	ENGINE.particles[ENGINE.particleCount] = (Particle) {
		.pos = (Vector2) {
			x,
			y
		},
		.vel = Vector2Zero(),
		.accel = Vector2Zero(),
		.lowVelColor = lowVelColor,
		.highVelColor = highVelColor
	};

	++ENGINE.particleCount;
}

void spawnParticlesRandom(Color* lowVelColors, size_t lowVelColorsCount, Color* highVelColors, size_t highVelColorsCount) {
	size_t left = ENGINE.particleCap - ENGINE.particleCount;

	for (size_t i = 0; i < left; ++i) {
		size_t x = rand() % ENGINE.winWidth;
		size_t y = rand() % ENGINE.winHeight;
		spawnParticleAt(x, y, lowVelColors[rand() % lowVelColorsCount], highVelColors[rand() % highVelColorsCount]);		
		ENGINE.oMap[y * ENGINE.winWidth + x] = 1;
	}
}

void createMouseAttractor(float gravity, float rotationCoeff) {
	if (ENGINE.attractorCount >= ENGINE.attractorCap || ENGINE.useMouseAttractor) return;

	ENGINE.attractors[ENGINE.attractorCount] = (Attractor) {
		.pos = GetMousePosition(),
		.gravity = gravity,
		.rotationCoeff = rotationCoeff,
		.isActive = true
	};

	ENGINE.mouseAttractor = ENGINE.attractors + ENGINE.attractorCount;

	ENGINE.useMouseAttractor = true;
	++ENGINE.attractorCount;
}

