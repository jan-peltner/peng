#include "entities.h"
#include "engine.h"
#include "raymath.h"
#include "../peng.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

static int32_t ATTRACTOR_ID = 1;
static int32_t LIGHT_ID = 1;

// particles
void spawnParticleAt(size_t x, size_t y, Color lowVelColor, Color highVelColor) {
	if (ENGINE.particleCount >= ENGINE.particleCap) return;
	
	ENGINE.particles[ENGINE.particleCount] = (Particle) {
		.pos = (Vector2) {
			x,
			y
		},
		.vel = Vector2Zero(),
		.accel = Vector2Zero(),
		.lowColor = lowVelColor,
		.highColor = highVelColor,
		.kfIndex = (size_t)-1,
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

bool pushParticleKeyframe(float kfDuration, float kfForce) {
	if (ENGINE.kfCount >= KEYFRAME_MAX_COUNT) return false;
	
	Keyframe kf = {.force = kfForce, .duration = kfDuration};
	ENGINE.kfs[ENGINE.kfCount] = kf;

	for (size_t i = 0; i < ENGINE.particleCount; ++i) {
		ENGINE.particles[i].kfPos[ENGINE.kfCount] = ENGINE.particles[i].pos;
	}

	++ENGINE.kfCount;
	return true;
}

void dispatchParticleKeyframe(void* _) {
	++ENGINE.kfIndex;
	if (ENGINE.kfIndex >= ENGINE.kfCount) return; 

	for (size_t i = 0; i < ENGINE.particleCount; ++i) {
		ENGINE.particles[i].kfIndex++;
		if (ENGINE.particles[i].kfIndex >= ENGINE.kfCount) return; 
	}
	printf("dispatched particle kf\n");

	ENGINE.kfActive = true;
}

// attractors
AttractorId spawnAnimatedAttractor(const Vector2* animationPath, size_t pathLen, float totalAnimationTime, bool isLooping, LoopMode loopMode, float gravity, float rotationCoeff) {
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
		.loopMode = loopMode,
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
		.loopMode = LOOP_WRAP,
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
		.totalTime = 0.0f,
		.elapsedTime = 0.0f,
		.isLooping = false,
		.loopMode = LOOP_WRAP,
		.isAnimated = false
	};

	ENGINE.mouseAttractor = ENGINE.attractors + ENGINE.attractorCount - 1;

	ENGINE.useMouseAttractor = true;

	return ATTRACTOR_ID++;
}

// lights
LightId spawnStaticLight(Vector2 origin, float intensity) {
	if (ENGINE.lightCount >= ENGINE.lightCap) {
		TraceLog(LOG_WARNING, "[PENG] light capacity maxed out");
		return -1;
	}

	ENGINE.lights[ENGINE.lightCount++] = (Light) {
		.id = LIGHT_ID,
		.pos = origin,
		.intensity = intensity,
		.isActive = true,
		.path = NULL,
		.pathLen = 0,
		.totalTime = 0.0f,
		.elapsedTime = 0.0f,
		.isLooping = false,
		.loopMode = LOOP_WRAP,
		.isAnimated = false 
	};

	return LIGHT_ID++;
}

LightId createMouseLight(float intensity) {
	if (ENGINE.lightCount >= ENGINE.lightCap) {
		TraceLog(LOG_WARNING, "[PENG] Light capacity maxed out");
		return -1;
	}
	if (ENGINE.useMouseLight) {
		TraceLog(LOG_WARNING, "[PENG] Only one mouse light can exist at a time");
		return -1;
	}

	ENGINE.lights[ENGINE.lightCount++] = (Light) {
		.id = LIGHT_ID,
		.pos = GetMousePosition(),
		.intensity = intensity,
		.isActive = true,
		.path = NULL,
		.pathLen = 0,
		.totalTime = 0.0f,
		.elapsedTime = 0.0f,
		.isLooping = false,
		.loopMode = LOOP_WRAP,
		.isAnimated = false
	};

	ENGINE.mouseLight = ENGINE.lights + ENGINE.lightCount - 1;

	ENGINE.useMouseLight = true;

	return LIGHT_ID++;
}
