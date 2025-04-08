#include "raylib.h"
#include "engine.h"
#include "window.h"
#include <math.h>
#include <raymath.h>

#define PARTICLES_AMOUNT 20000

int main(void) {
	float windowDiagonal = sqrtf(WINDOW_WIDTH * WINDOW_WIDTH + WINDOW_HEIGHT * WINDOW_HEIGHT);
	Particle* ps = initField(PARTICLES_AMOUNT);
	Particle p = spawn(10, 10, RAYWHITE);

	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Peng");	

	Attractor mouse = {
		.pos = GetMousePosition(),
		.gravity = 2.0f
	};

	while (!WindowShouldClose()) {
		float dt = GetFrameTime();
		mouse.pos = GetMousePosition();

		for (int i = 0; i < PARTICLES_AMOUNT; ++i) {
			applyForces(&ps[i], &mouse, windowDiagonal, dt);
			applyVel(&ps[i], dt);
		}

		BeginDrawing();

		ClearBackground(BLACK);
		for (int i = 0; i < PARTICLES_AMOUNT; ++i) {
			draw(&ps[i]);
		}

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
