#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include "window.h"
#include "engine.h"

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

	self->pos.x = fmaxf(0.0f, fminf(newX, WINDOW_WIDTH - 1));
	self->pos.y = fmaxf(0.0f, fminf(newY, WINDOW_HEIGHT - 1));
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

Particle spawn(int x, int y, Color color) {
	return (Particle) {
		.pos = (Vector2) {
			x,
			y
		},
		.vel = Vector2Zero(),
		.accel = Vector2Zero(),
		color
	};
}

void draw(Particle* self) {
	DrawPixelV(self->pos, self->color);
}

void clearOccupancyMap(char* map) {
	memset(map, 0, WINDOW_AREA);
}

char* initOccupancyMap(int size) {
	char* map = malloc(size);

	if (map == NULL) {
		fprintf(stderr, "malloc failed");
		exit(1);
	}

	clearOccupancyMap(map);
	return map;
}

void setOccupancyMap(Particle* p, char* map) {
	map[(int)p->pos.y * WINDOW_WIDTH + (int)p->pos.x] = 1;
}

Particle* initField(int amount, char* occupancyMap) {
	srand(time(NULL));
	Particle* particles = malloc(amount * sizeof(Particle));

	if (particles == NULL) {
		fprintf(stderr, "malloc failed");
		exit(1);
	}

	for (int i = 0; i < amount; ++i) {
		int x = rand() % WINDOW_WIDTH;
		int y = rand() % WINDOW_HEIGHT;
		particles[i] = spawn(x, y, RAYWHITE);		
		occupancyMap[y * WINDOW_WIDTH + x] = 1;
	}
	return particles;
}

