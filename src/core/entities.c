#include "engine.h"
#include "raymath.h"
#include "../peng.h"
#include <raylib.h>
#include <stdlib.h>

static int32_t ATTRACTOR_ID = 1;

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

AttractorId spawnAnimatedAttractor(const Vector2* animationPath, size_t pathLen, float totalAnimationTime, bool isLooping, float gravity, float rotationCoeff) {
	if (ENGINE.attractorCount >= ENGINE.attractorCap || pathLen < 2) return -1;

	ENGINE.attractors[ENGINE.attractorCount] = (Attractor) {
		.id = ATTRACTOR_ID,
		.pos = animationPath[0],
		.gravity = gravity,
		.rotationCoeff = rotationCoeff,
		.isActive = true,
		.path = animationPath,
		.pathLen = pathLen,
		.totalTime = totalAnimationTime,
		.elapsedTime = 0.0f,
		.isLooping = isLooping,
		.isAnimated = true 
	};

	return ATTRACTOR_ID++;
}

AttractorId spawnStaticAttractor(Vector2 origin, float gravity, float rotationCoeff) {
	if (ENGINE.attractorCount >= ENGINE.attractorCap) return -1;

	ENGINE.attractors[ENGINE.attractorCount] = (Attractor) {
		.id = ATTRACTOR_ID,
		.pos = origin,
		.gravity = gravity,
		.rotationCoeff = rotationCoeff,
		.isActive = true,
		.path = NULL,
		.pathLen = 0,
		.totalTime = 0.0f,
		.elapsedTime = 0.0f,
		.isLooping = false,
		.isAnimated = false 
	};

	return ATTRACTOR_ID++;
}

Attractor* findAttractorById(AttractorId id) {
	for (size_t a = 0; a < ENGINE.attractorCount; ++a) {
		if (ENGINE.attractors[a].id == id) return &ENGINE.attractors[a];
	}
	return NULL;
}

void toggleAttractor(AttractorId id) {
	Attractor* a = findAttractorById(id);
	if (a != NULL) {
		a->isActive = !a->isActive;
	}	
}

AttractorId createMouseAttractor(float gravity, float rotationCoeff) {
	if (ENGINE.attractorCount >= ENGINE.attractorCap || ENGINE.useMouseAttractor) return -1;

	ENGINE.attractors[ENGINE.attractorCount] = (Attractor) {
		.id = ATTRACTOR_ID,
		.pos = GetMousePosition(),
		.gravity = gravity,
		.rotationCoeff = rotationCoeff,
		.isActive = true,
		.path = NULL,
		.pathLen = 0,
		.isLooping = false,
		.isAnimated = false
	};

	ENGINE.mouseAttractor = ENGINE.attractors + ENGINE.attractorCount;

	ENGINE.useMouseAttractor = true;
	++ENGINE.attractorCount;
	return ATTRACTOR_ID++;
}
