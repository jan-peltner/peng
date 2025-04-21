#include "engine.h"
#include "physics.h"
#include "animations.h"
#include "scheduler.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

ENGINE_t ENGINE;

void oMapClear(char* oMap) {
	memset(oMap, 0, ENGINE.winArea);
}

void oMapSet(Particle* p, char* oMap) {
	oMap[(int)p->pos.y * ENGINE.winWidth + (int)p->pos.x] = 1;
}

void startPeng(int winW, int winH, size_t particleCap, size_t attractorCap) {
	// window
	ENGINE.winWidth = winW;
	ENGINE.winHeight = winH;
	ENGINE.winArea = winW * winH;
	ENGINE.winDiag = (int)(sqrtf(winW * winW + winH * winH) + 0.5f);
	ENGINE.time = 0;

	srand(time(NULL));

	// init scheduler
		// alloc events
		// INFO: event cap hard-coded for now
	Event* events = malloc(64 * sizeof(Event));
	if (events == NULL) {
		fprintf(stderr, "malloc failed @ events");
		exit(1);
	}

	ENGINE.scheduler = (Scheduler) {
		.events = events,
		.eventCount = 0,
		.eventCap = 64,
		.eventCursor = 0
	};

	// alloc particles
	Particle* particles = malloc(particleCap * sizeof(Particle));

	if (particles == NULL) {
		fprintf(stderr, "malloc failed @ particles");
		exit(1);
	}

	ENGINE.particles = particles;
	ENGINE.particleCap = particleCap;
	ENGINE.particleCount = 0;

	// alloc attractors
	Attractor* attractors = malloc(attractorCap * sizeof(Attractor));

	if (attractors == NULL) {
		fprintf(stderr, "malloc failed @ attractors");
		exit(1);
	}

	ENGINE.attractors = attractors;
	ENGINE.attractorCap = attractorCap;
	ENGINE.attractorCount = 0;

	// alloc oMap
	char* oMap = malloc(ENGINE.winArea);

	if (oMap == NULL) {
		fprintf(stderr, "malloc failed @ oMap");
		exit(1);
	}

	ENGINE.oMap = oMap;

	// initialize oMap with 0's
	oMapClear(ENGINE.oMap);

	// GPU rendering
	Color* pixelBuf = malloc(ENGINE.winArea * sizeof(Color));

	if (pixelBuf == NULL) {
		fprintf(stderr, "malloc failed @ pixelBuf");
		exit(1);
	}

	ENGINE.pixelBuffer = pixelBuf;

	Image temp = GenImageColor(ENGINE.winWidth, ENGINE.winHeight, BLACK);
	ENGINE.particleTexture = LoadTextureFromImage(temp);
	UnloadImage(temp);

	// config
	ENGINE.useFrictionForce = true;
	ENGINE.useAttractorForce = true;
	ENGINE.useRepellentForce = true;
	ENGINE.particlesFrozen = false;
	ENGINE.renderUi = true;
}

void stopPeng() {
	free(ENGINE.oMap);
	free(ENGINE.attractors);
	free(ENGINE.particles);
	free(ENGINE.pixelBuffer);
}

void runUpdate(float dt) {
	ENGINE.time += dt;
	pollScheduler();

	size_t  particlesPerThread = ENGINE.particleCount / THREAD_COUNT;
	oMapClear(ENGINE.oMap);

	if (ENGINE.useMouseAttractor) {
		ENGINE.mouseAttractor->pos = GetMousePosition();
	}

	for (size_t a = 0; a < ENGINE.attractorCount; ++a) {
		updateAttractorAnimation(&ENGINE.attractors[a], dt);
	}

	// clear pixel buffer -> set background to black
	memset(ENGINE.pixelBuffer, 0, ENGINE.winArea * sizeof(Color));

	for (size_t t = 0; t < THREAD_COUNT; ++t) {
		ENGINE.threadData[t].start = t * particlesPerThread;
		ENGINE.threadData[t].end = (t == THREAD_COUNT - 1) ? ENGINE.particleCount : (t + 1) * particlesPerThread;
		ENGINE.threadData[t].dt = dt; 
		pthread_create(&ENGINE.threads[t], NULL, runMtPhysUpdate, &ENGINE.threadData[t]);
	}	

	for (size_t t = 0; t < THREAD_COUNT; ++t) {
		pthread_join(ENGINE.threads[t], NULL);
	}

	UpdateTexture(ENGINE.particleTexture, ENGINE.pixelBuffer);
}

void toggleAttractorForce() {
	ENGINE.useAttractorForce = !ENGINE.useAttractorForce;
	printf("toggled");
}

void toggleFrictionForce() {
	ENGINE.useFrictionForce = !ENGINE.useFrictionForce;
}

void toggleRepellentForce() {
	ENGINE.useRepellentForce = !ENGINE.useRepellentForce;
}

void toggleParticlesFrozen() {
	ENGINE.particlesFrozen = !ENGINE.particlesFrozen;
}

void toggleRenderUi() {
	ENGINE.renderUi = !ENGINE.renderUi;
}

