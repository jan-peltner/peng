#include "engine.h"
#include "raymath.h"
#include <raylib.h>
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

void spawnParticlesRandom(const Color* lowVelColors, size_t lowVelColorsCount, const Color* highVelColors, size_t highVelColorsCount) {
	size_t left = ENGINE.particleCap - ENGINE.particleCount;

	for (size_t i = 0; i < left; ++i) {
		size_t x = rand() % ENGINE.winWidth;
		size_t y = rand() % ENGINE.winHeight;
		spawnParticleAt(x, y, lowVelColors[rand() % lowVelColorsCount], highVelColors[rand() % highVelColorsCount]);		
		ENGINE.oMap[y * ENGINE.winWidth + x] = 1;
	}
}

void spawnParticlesFromImage(Image* img, Vector2 origin, size_t sampleStride) {
	Color* imgColors = LoadImageColors(*img);
	int imgCols = img->width;
	int imgRows = img->height;

	for (size_t i = 0; i < imgRows; i += sampleStride) {
		for (size_t j = 0; j < imgCols; j += sampleStride) {
			Color highVelColor = imgColors[i * imgCols + j];
			// if(highVelColor.a < 128) continue;
			spawnParticleAt((int)origin.x + j, (int)origin.y + i, BLACK, highVelColor);
		}
	}	

	UnloadImageColors(imgColors);
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
