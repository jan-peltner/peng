#include <string.h>
#include <time.h>
#include <raylib.h>
#include <raymath.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "core.h"
#include "peng.h"

float toAttractorLength(Particle* self, Attractor* attractor) {
	Vector2 delta = Vector2Subtract(attractor->pos, self->pos);
	return sqrtf(delta.x * delta.x + delta.y * delta.y);
}

float toAttractorLengthNormalized(Particle* self, Attractor* attractor, float winDiag) {
	Vector2 delta = Vector2Subtract(attractor->pos, self->pos);
	return sqrtf(delta.x * delta.x + delta.y * delta.y) / winDiag;
}

Vector2 toAttractorNormalized(Particle* self, Attractor* attractor) {
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
	} else {
		self->vel = newVel;		
	}
	self->accel = Vector2Zero();
}

void applyAttractorForce(Particle* self, Attractor* attractor, float winDiag) {
	if (!attractor->isActive) return;
	Vector2 direction = toAttractorNormalized(self, attractor);
	float normalizedDist = toAttractorLengthNormalized(self, attractor, winDiag);
	float gravityForceMag = attractor->gravity / ((normalizedDist * normalizedDist) + EPSILON);
	Vector2 gravity = Vector2Scale(direction, gravityForceMag);
	self->accel = Vector2Add(self->accel, gravity);
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
			if (forceApplied) return;
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
						// we really only scale by invDist since we divide by dist to get the normalized delta
						Vector2 force = Vector2Scale(delta, invDistSq);
						self->accel = Vector2Add(self->accel, force);
						forceApplied = true;
					}
				}
			}
		}
	}		
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
}

void spawnParticleAt(size_t x, size_t y, Color color) {
	if (ENGINE.particleCount >= ENGINE.particleCap) return;
	
	ENGINE.particles[ENGINE.particleCount] = (Particle) {
		.pos = (Vector2) {
			x,
			y
		},
		.vel = Vector2Zero(),
		.accel = Vector2Zero(),
		.color = color
	};

	++ENGINE.particleCount;
}

void spawnParticlesRandom() {
	size_t left = ENGINE.particleCap - ENGINE.particleCount;

	for (size_t i = 0; i < left; ++i) {
		size_t x = rand() % ENGINE.winWidth;
		size_t y = rand() % ENGINE.winHeight;
		spawnParticleAt(x, y, RAYWHITE);		
		ENGINE.oMap[y * ENGINE.winWidth + x] = 1;
	}
}

void createMouseAttractor(float gravity) {
	if (ENGINE.attractorCount >= ENGINE.attractorCap || ENGINE.useMouseAttractor) return;

	ENGINE.attractors[ENGINE.attractorCount] = (Attractor) {
		.pos = GetMousePosition(),
		.gravity = gravity,
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

void runPhysicsUpdate(float dt) {
		oMapClear(ENGINE.oMap);

		if (ENGINE.useMouseAttractor) {
			ENGINE.mouseAttractor->pos = GetMousePosition();
		}

		for (size_t i = 0; i < ENGINE.particleCount; ++i) {
			oMapSet(&ENGINE.particles[i], ENGINE.oMap);	
		}

		for (size_t i = 0; i < ENGINE.particleCount; ++i) {
			if (ENGINE.useAttractorForce) {
				for (size_t j = 0; j < ENGINE.attractorCount; ++j) {
					applyAttractorForce(&ENGINE.particles[i], &ENGINE.attractors[j], ENGINE.winDiag);
				}
			}
			if (ENGINE.useRepellentForce) {
				applyRepellentForce(&ENGINE.particles[i], ENGINE.oMap);
			}
			if (ENGINE.useFrictionForce) {
				applyFrictionForce(&ENGINE.particles[i]);
			}

			applyAccel(&ENGINE.particles[i], dt);
			applyVel(&ENGINE.particles[i], dt);
		}
} 

void drawParticles() {
	for (size_t i = 0; i < ENGINE.particleCount; ++i) {
		Particle p = ENGINE.particles[i];
		DrawPixelV(p.pos, p.color);
	}
}
