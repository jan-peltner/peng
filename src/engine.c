#include <math.h>
#include <stdlib.h>
#include <time.h>
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

void applyForces(Particle* self, Attractor* attractor, float winDiag, float dt) {
	Vector2 direction = toAttractorNormalized(self, attractor);
	float normalizedDist = toAttractorLengthNormalized(self, attractor, winDiag);
	float gravityForceMag = attractor->gravity / ((normalizedDist * normalizedDist) + EPSILON);
	Vector2 gravity = Vector2Scale(direction, gravityForceMag);

	Vector2 friction = Vector2Scale(self->vel, -FRICTION_SCALAR);
	Vector2 acceleration = Vector2Add(gravity, friction);
	Vector2 accelerationDt = Vector2Scale(acceleration, dt);
	Vector2 newVel = Vector2Add(self->vel, accelerationDt); 
	float newVelLen = Vector2Length(newVel);

	if (newVelLen > VELOCITY_VEC_MAX_LENGTH) {
		self->vel = Vector2Scale(newVel, VELOCITY_VEC_MAX_LENGTH / newVelLen);
	} else {
		self->vel = newVel;		
	}
}

Particle spawn(int x, int y, Color color) {
	return (Particle) {
		.pos = (Vector2) {
			.x = x,
			.y = y
		},
		.vel = (Vector2) {
			.x = 0,
			.y = 0
		},
		.color = color
	};
}

void draw(Particle* self) {
	DrawPixelV(self->pos, self->color);
}

Particle* initField(int amount) {
	srand(time(NULL));
	Particle* particles = malloc(amount * sizeof(Particle));
	for (int i = 0; i < amount; ++i) {
		particles[i] = spawn(rand() % WINDOW_WIDTH, rand() % WINDOW_HEIGHT, WHITE);		
	}
	return particles;
}
