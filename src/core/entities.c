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

void spawnParticlesFromImage(Image* img, Vector2 origin, size_t sampleStride, Color lowVelColor) {
	Color* imgColors = LoadImageColors(*img);
	int imgCols = img->width;
	int imgRows = img->height;

	for (size_t i = 0; i < imgRows; i += sampleStride) {
		for (size_t j = 0; j < imgCols; j += sampleStride) {
			Color highVelColor = imgColors[i * imgCols + j];
			// if(highVelColor.a < 128) continue;
			spawnParticleAt((int)origin.x + j, (int)origin.y + i, lowVelColor, highVelColor);
		}
	}	

	UnloadImageColors(imgColors);
}

AttractorId spawnAnimatedAttractor(const Vector2* animationPath, size_t pathLen, float totalAnimationTime, bool isLooping, float gravity, float rotationCoeff) {
	if (ENGINE.attractorCount >= ENGINE.attractorCap) {
		TraceLog(LOG_WARNING, "[PENG] Attractor capacity maxed out");
		return -1;
	}

	if (pathLen < 2) {
		TraceLog(LOG_WARNING, "[PENG] Animation path has to contain at least 2 points");
		return -1;
	}

	// validate path
	for (size_t i = 0; i < pathLen; ++i) {
		if (animationPath[i].x < 0 || animationPath[i].x >= ENGINE.winWidth || animationPath[i].y < 0 || animationPath[i].y >= ENGINE.winHeight) {
			TraceLog(LOG_WARNING, "[PENG] Animation path point at index %d is out of bounds", i);
			return -1;
		}
	}

	ENGINE.attractors[ENGINE.attractorCount++] = (Attractor) {
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
	if (ENGINE.attractorCount >= ENGINE.attractorCap) {
		TraceLog(LOG_WARNING, "[PENG] Attractor capacity maxed out");
		return -1;
	}

	ENGINE.attractors[ENGINE.attractorCount++] = (Attractor) {
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
	if (ENGINE.attractorCount >= ENGINE.attractorCap) {
		TraceLog(LOG_WARNING, "[PENG] Attractor capacity maxed out");
		return -1;
	}
	if (ENGINE.useMouseAttractor) {
		TraceLog(LOG_WARNING, "[PENG] Only one mouse attractor can exist at a time");
		return -1;
	}

	ENGINE.attractors[ENGINE.attractorCount++] = (Attractor) {
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

	ENGINE.mouseAttractor = ENGINE.attractors + ENGINE.attractorCount - 1;

	ENGINE.useMouseAttractor = true;
	return ATTRACTOR_ID++;
}
