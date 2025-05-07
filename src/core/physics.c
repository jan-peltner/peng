#include "engine.h"
#include "physics.h"
#include "raymath.h"
#include "utils.h"
#include <math.h>
#include <stdio.h>

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

	if (newVelLen > MAX_SPEED) {
		self->vel = Vector2Scale(newVel, MAX_SPEED / newVelLen);
	} else {
		self->vel = newVel;		
	}
	self->accel = Vector2Zero();
}

void applyAttractorForce(Particle* self, Attractor* attractor, float winDiag) {
	if (!attractor->isActive) return;

	Vector2 direction = unitVectorTo(self->pos, attractor->pos);
	Vector2 normalVec = Vector2Rotate(direction, PI/2);

	float normalizedDist = normalizedDistanceTo(self->pos, attractor->pos, winDiag);
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
	int cx = (int)self->pos.x;
	int cy = (int)self->pos.y;

	for (int col = cx - REPELLING_RADIUS + 1; col <= cx + REPELLING_RADIUS; ++col) {
		for (int row = cy - REPELLING_RADIUS + 1; row <= cy + REPELLING_RADIUS; ++row) {
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
					}
				}
			}
		}
	}		
}

void applyKeyframeForce(Particle* self, float scaledKfForce) {
	Vector2 force = Vector2Scale(unitVectorTo(self->pos, self->kfPos[self->kfIndex]), scaledKfForce);
	self->accel = Vector2Add(self->accel, force);
}

void applyLighting(Particle* self) {
	float physicalBrightness= 0.0f + AMBIENT_LIGHT;
	float velocityBrightness = fminf(Vector2Length(self->vel) / MAX_SPEED, 1.0f);
	for (size_t i = 0; i < ENGINE.lightCount; ++i) {
		Light light = ENGINE.lights[i];
		float normalizedDist = normalizedDistanceTo(self->pos, light.pos, ENGINE.winDiag);	
		float brightness = light.intensity / expf(normalizedDist * 5.0f);
		physicalBrightness += brightness;	
	}
	if (ENGINE.lightCount > 0) {
		physicalBrightness /= ENGINE.lightCount;
	}
	self->brightness = velocityBrightness * (1 - ENGINE.lightingBlend) + physicalBrightness * ENGINE.lightingBlend;
}

void lerpColor(Particle* self) {
	self->lerpedColor = ColorLerp(self->lowColor, self->highColor, self->brightness);
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

		if (ENGINE.kfActive) {
			applyKeyframeForce(&ENGINE.particles[i], tData->scaledKfForce);
		}

		// compute particle state
		applyAccel(&ENGINE.particles[i], tData->dt);
		if (!ENGINE.particlesFrozen) {
			applyVel(&ENGINE.particles[i], tData->dt);
		}
		applyLighting(&ENGINE.particles[i]);
		lerpColor(&ENGINE.particles[i]);
		
		int x = (int)ENGINE.particles[i].pos.x;
		int y = (int)ENGINE.particles[i].pos.y;
		ENGINE.pixelBuffer[ENGINE.winWidth * y + x] = ENGINE.particles[i].lerpedColor;
	}

	return NULL;		
} 
