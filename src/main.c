#include "raylib.h"
#include "engine.h"
#include "window.h"
#include <math.h>
#include <raymath.h>

#define PARTICLES_AMOUNT 10000 

int main(void) {
	float windowDiagonal = sqrtf(WINDOW_WIDTH * WINDOW_WIDTH + WINDOW_HEIGHT * WINDOW_HEIGHT);
	int fps;

	char* occupancyMap = initOccupancyMap(WINDOW_AREA);
	Particle* ps = initField(PARTICLES_AMOUNT, occupancyMap);
	
	Attractor mouse = {
		.pos = GetMousePosition(),
		.gravity = 2.0f
	};

	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Peng");	


	while (!WindowShouldClose()) {
		float dt = GetFrameTime();
		fps = GetFPS();
		mouse.pos = GetMousePosition();
			
		// physics updates
		for (int i = 0; i < PARTICLES_AMOUNT; ++i) {
			applyAttractorForce(&ps[i], &mouse, windowDiagonal);
			applyFrictionForce(&ps[i]);
			applyRepellentForce(&ps[i], occupancyMap);

			applyAccel(&ps[i], dt);
			applyVel(&ps[i], dt);

			setOccupancyMap(&ps[i], occupancyMap);	
		}

		BeginDrawing();

		ClearBackground(BLACK);

		// particle rendering
		for (int i = 0; i < PARTICLES_AMOUNT; ++i) {
			draw(&ps[i]);
		}

		DrawText(TextFormat("FPS: %d", fps), 10, 10, 18, RAYWHITE);

		EndDrawing();
		clearOccupancyMap(occupancyMap);
	}


	CloseWindow();
	return 0;
}
