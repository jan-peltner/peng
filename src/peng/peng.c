#include <pthread.h>
#include <string.h>
#include <time.h>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h> 
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "core.h"
#include "peng.h"

float toAttractorLength(const Particle* self, Attractor* attractor) {
	Vector2 delta = Vector2Subtract(attractor->pos, self->pos);
	return sqrtf(delta.x * delta.x + delta.y * delta.y);
}

float toAttractorLengthNormalized(const Particle* self, Attractor* attractor, float winDiag) {
	Vector2 delta = Vector2Subtract(attractor->pos, self->pos);
	return sqrtf(delta.x * delta.x + delta.y * delta.y) / winDiag;
}

Vector2 toAttractorNormalized(const Particle* self, Attractor* attractor) {
	Vector2 delta = Vector2Subtract(attractor->pos, self->pos);
	float length = sqrtf(delta.x * delta.x + delta.y * delta.y);
	return (Vector2) {
		.x = delta.x / length,
		.y = delta.y / length
	};
}

void applyVel(Particle* self, float dt) {
	float newX = self->pos.x + self->vel.x * dt;
	float newY = self->pos.y + self->vel.y * dt;

	self->pos.x = fmaxf(0.0f, fminf(newX, ENGINE.winWidth - 1));
	self->pos.y = fmaxf(0.0f, fminf(newY, ENGINE.winHeight - 1));
}

void applyAccel(Particle* self, float dt) {
	Vector2 accelerationDt = Vector2Scale(self->accel, dt);
	Vector2 newVel = Vector2Add(self->vel, accelerationDt); 
	float newVelLen = Vector2Length(newVel);

	if (newVelLen > VELOCITY_VEC_MAX_LENGTH) {
		self->vel = Vector2Scale(newVel, VELOCITY_VEC_MAX_LENGTH / newVelLen);
		self->velLen = VELOCITY_VEC_MAX_LENGTH;
	} else {
		self->vel = newVel;		
		self->velLen = newVelLen;
	}
	self->accel = Vector2Zero();
}

void applyAttractorForce(Particle* self, Attractor* attractor, float winDiag) {
	if (!attractor->isActive) return;
	Vector2 direction = toAttractorNormalized(self, attractor);
	Vector2 normalVec = Vector2Rotate(direction, PI/2);
	float normalizedDist = toAttractorLengthNormalized(self, attractor, winDiag);
	float gravityForceMag = attractor->gravity / ((normalizedDist * normalizedDist) + EPSILON);
	float rotationForceMag = attractor->gravity * 0.33f / ((normalizedDist * normalizedDist) + EPSILON);
	Vector2 gravity = Vector2Scale(direction, gravityForceMag);
	Vector2 rotation = Vector2Scale(normalVec, rotationForceMag);
	self->accel = Vector2Add(Vector2Add(self->accel, gravity), rotation);
}

void applyFrictionForce(Particle *self) {
	Vector2 friction = Vector2Scale(self->vel, -FRICTION_SCALAR);
	self->accel = Vector2Add(self->accel, friction);
}

void applyRepellentForce(Particle *self, char* oMap) {
	bool forceApplied = false; 
	int cx = (int)self->pos.x;
	int cy = (int)self->pos.y;

	for (int col = cx - REPELLING_RADIUS + 1; col <= cx + REPELLING_RADIUS; ++col) {
		for (int row = cy - REPELLING_RADIUS + 1; row <= cy + REPELLING_RADIUS; ++row) {
			// INFO: forceApplied is a hacky way to gain performance
			// if (forceApplied) return;
			if(col > 0 && col < ENGINE.winWidth && row > 0 && row < ENGINE.winHeight) {
				if (col == cx && row == cy) continue;
				Vector2 delta = {
					.x = (float)(cx - col),
					.y = (float)(cy - row)
				};
				float distSq = delta.x * delta.x + delta.y * delta.y;
				if (distSq <= REPELLING_RADIUS * REPELLING_RADIUS) {
					int idx = row * ENGINE.winWidth + col;
					if (oMap[idx] == 1) {
						float invDistSq = 1.0f / distSq;
						Vector2 force = Vector2Scale(delta, invDistSq * 3.33);
						self->accel = Vector2Add(self->accel, force);
						forceApplied = true;
					}
				}
			}
		}
	}		
}

void computeColor(Particle* self) {
	self->lerpedColor = ColorLerp(self->slowColor, self->fastColor, self->velLen / VELOCITY_VEC_MAX_LENGTH);
}

void oMapClear(char* oMap) {
	memset(oMap, 0, ENGINE.winArea);
}

void oMapSet(Particle* p, char* oMap) {
	oMap[(int)p->pos.y * ENGINE.winWidth + (int)p->pos.x] = 1;
}

void startPeng(int winW, int winH, size_t particlesCount, size_t attractorCount) {
	// window
	ENGINE.winWidth = winW;
	ENGINE.winHeight = winH;
	ENGINE.winArea = winW * winH;
	ENGINE.winDiag = (int)(sqrtf(winW * winW + winH * winH) + 0.5f);
	ENGINE.frameCounter = 0;
	srand(time(NULL));

	// alloc particles
	Particle* particles = malloc(particlesCount * sizeof(Particle));

	if (particles == NULL) {
		fprintf(stderr, "malloc failed @ particles");
		exit(1);
	}

	ENGINE.particles = particles;
	ENGINE.particleCap = particlesCount;
	ENGINE.particleCount = 0;

	// default particle colors
	static Color slowColor = {
		.r = 255,
		.g = 255,
		.b = 255,
		.a = 255
	};
	ENGINE.particleFastColors = &slowColor;
	ENGINE.particleSlowColorsCount = 1;

	static Color fastColor = {
		.r = 253,
		.g = 231,
		.b = 76,
		.a = 255
	};
	ENGINE.particleSlowColors = &fastColor;
	ENGINE.particleFastColorsCount = 1;

	// alloc attractors
	Attractor* attractors = malloc(attractorCount * sizeof(Attractor));

	if (attractors == NULL) {
		fprintf(stderr, "malloc failed @ attractors");
		exit(1);
	}

	ENGINE.attractors = attractors;
	ENGINE.attractorCap = attractorCount;
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

	// shaders
	float* particleVertices = malloc(ENGINE.particleCap * 3 * sizeof(float));
	unsigned char* particleColors = malloc(ENGINE.particleCap * 4 * sizeof(unsigned char));

	if (particleVertices == NULL) {
		fprintf(stderr, "malloc failed @ particleVertices");
		exit(1);
	}

	if (particleColors == NULL) {
		fprintf(stderr, "malloc failed @ particleColors");
		exit(1);
	}
	
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
	ENGINE.isPhysicsPaused = false;
}

void stopPeng() {
	free(ENGINE.oMap);
	free(ENGINE.attractors);
	free(ENGINE.particles);
	free(ENGINE.pixelBuffer);
}

void spawnParticleAt(size_t x, size_t y) {
	if (ENGINE.particleCount >= ENGINE.particleCap) return;
	
	ENGINE.particles[ENGINE.particleCount] = (Particle) {
		.pos = (Vector2) {
			x,
			y
		},
		.vel = Vector2Zero(),
		.accel = Vector2Zero(),
		.slowColor = ENGINE.particleSlowColors[rand() % ENGINE.particleSlowColorsCount],
		.fastColor = ENGINE.particleFastColors[rand() % ENGINE.particleFastColorsCount]
	};

	++ENGINE.particleCount;
}

void spawnParticlesRandom() {
	size_t left = ENGINE.particleCap - ENGINE.particleCount;

	for (size_t i = 0; i < left; ++i) {
		size_t x = rand() % ENGINE.winWidth;
		size_t y = rand() % ENGINE.winHeight;
		spawnParticleAt(x, y);		
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

void toggleAttractors() {
	for (size_t i = 0; i < ENGINE.attractorCount; ++i) {
		ENGINE.attractors[i].isActive = !ENGINE.attractors[i].isActive;
	}
}

void toggleAttractorForce() {
	ENGINE.useAttractorForce = !ENGINE.useAttractorForce;
}
void toggleFrictionForce() {
	ENGINE.useFrictionForce = !ENGINE.useFrictionForce;
}
void toggleRepellentForce() {
	ENGINE.useRepellentForce = !ENGINE.useRepellentForce;
}

void* runMtPhysUpdate(void* arg) {
	ThreadData* tData = (ThreadData*)arg;

	for (size_t i = tData->start; i < tData->end; ++i) {
		oMapSet(&ENGINE.particles[i], ENGINE.oMap);	
	}

	for (size_t i = tData->start; i < tData->end; ++i) {
		if (ENGINE.useAttractorForce) {
			for (size_t j = 0; j < ENGINE.attractorCount; ++j) {
				applyAttractorForce(&ENGINE.particles[i], &ENGINE.attractors[j], ENGINE.winDiag);
			}
		}

		if (ENGINE.useFrictionForce) {
			applyFrictionForce(&ENGINE.particles[i]);
		}

		if (ENGINE.useRepellentForce) {
			applyRepellentForce(&ENGINE.particles[i], ENGINE.oMap);
		}

		applyAccel(&ENGINE.particles[i], tData->dt);
		applyVel(&ENGINE.particles[i], tData->dt);
		computeColor(&ENGINE.particles[i]);
		
		int x = (int)ENGINE.particles[i].pos.x;
		int y = (int)ENGINE.particles[i].pos.y;
		ENGINE.pixelBuffer[ENGINE.winWidth * y + x] = ENGINE.particles[i].lerpedColor;
	}

	return NULL;		
} 

void runUpdate(float dt) {
	size_t  particlesPerThread = ENGINE.particleCount / THREAD_COUNT;
	oMapClear(ENGINE.oMap);

	if (ENGINE.useMouseAttractor) {
		ENGINE.mouseAttractor->pos = GetMousePosition();
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

	++ENGINE.frameCounter;
}

void drawParticles() {
	DrawTexture(ENGINE.particleTexture, 0, 0, WHITE);
}

void setSlowParticleColors(Color* colors, size_t count) {
	ENGINE.particleSlowColors = colors;
	ENGINE.particleSlowColorsCount = count;
}

void setFastParticleColors(Color* colors, size_t count) {
	ENGINE.particleFastColors = colors;
	ENGINE.particleFastColorsCount = count;
}
