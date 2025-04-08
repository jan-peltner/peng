#include "raylib.h"
#include "engine.h"
#include "window.h"
#include <math.h>
#include <raymath.h>

int main(void) {
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Peng");	
	float windowDiagonal = sqrtf(WINDOW_WIDTH * WINDOW_WIDTH + WINDOW_HEIGHT * WINDOW_HEIGHT);

	Attractor mouse = {
		.pos = GetMousePosition(),
		.gravity = 2.0f
	};

	Particle p = spawn(10, 10, RAYWHITE);
	while (!WindowShouldClose()) {
		float dt = GetFrameTime();
		mouse.pos = GetMousePosition();

		applyForces(&p, &mouse, windowDiagonal, dt);
		applyVel(&p, dt);

		BeginDrawing();

		ClearBackground(BLACK);
		draw(&p);

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
