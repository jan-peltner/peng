#include "engine.h"
#include "physics.h"
#include "raymath.h"
#include <math.h>

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

		// compute particle state
		applyAccel(&ENGINE.particles[i], tData->dt);
		if (!ENGINE.particlesFrozen) {
			applyVel(&ENGINE.particles[i], tData->dt);
		}
		computeColor(&ENGINE.particles[i]);
		
		int x = (int)ENGINE.particles[i].pos.x;
		int y = (int)ENGINE.particles[i].pos.y;
		ENGINE.pixelBuffer[ENGINE.winWidth * y + x] = ENGINE.particles[i].lerpedColor;
	}

	return NULL;		
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
void toggleParticlesFrozen() {
	ENGINE.particlesFrozen = !ENGINE.particlesFrozen;
}

