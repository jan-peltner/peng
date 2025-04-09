#ifndef PARTICLE_ENGINE_H

#define PARTICLE_ENGINE_H

#include "raylib.h"
#define FRICTION_SCALAR 0.98f
#define REPELLING_SCALAR 2.0f
#define VELOCITY_VEC_MAX_LENGTH 120.0f

typedef struct {
	Vector2 pos; 
	Vector2 vel; 
	Vector2 accel;
	Color color;
} Particle;

typedef struct {
	Vector2 pos;
	float gravity;
} Attractor;

Particle spawn(int x, int y, Color color);
void draw(Particle* self);
Particle* initField(int amount, char* occupancyMap);

float toAttractorLength(Particle* self, Attractor* attractor);
float toAttractorLengthNormalized(Particle* self, Attractor* attractor, float winDiag);
Vector2 toAttractorNormalized(Particle* self, Attractor* attractor);

void applyVel(Particle* self, float dt);
void applyAccel(Particle* self, float dt);
void applyAttractorForce(Particle* self, Attractor* attractor, float winDiag);
void applyFrictionForce(Particle *self);

void clearOccupancyMap(char* occupancyMap);
void setOccupancyMap(Particle* p, char* map);
char* initOccupancyMap(int size);

#endif // PARTICLE_ENGINE_H
