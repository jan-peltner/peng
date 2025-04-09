#ifndef PARTICLE_ENGINE_H

#define PARTICLE_ENGINE_H

#include "raylib.h"
#define FRICTION_SCALAR 0.98f
#define REPELLING_SCALAR 
#define VELOCITY_VEC_MAX_LENGTH 120.0f

typedef struct {
	Vector2 pos, vel, accel;
	Color color;
} Particle;

typedef struct {
	Vector2 pos;
	float gravity;
} Attractor;

Particle spawn(int x, int y, Color color);
void draw(Particle* self);
Particle* initField(int amount);
float toAttractorLength(Particle* self, Attractor* attractor);
float toAttractorLengthNormalized(Particle* self, Attractor* attractor, float winDiag);
Vector2 toAttractorNormalized(Particle* self, Attractor* attractor);
void applyVel(Particle* self, float dt);
void applyAccel(Particle* self, float dt);
void applyAttractorForce(Particle* self, Attractor* attractor, float winDiag);
void applyFrictionForce(Particle *self);

#endif // PARTICLE_ENGINE_H
