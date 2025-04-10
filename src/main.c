#include "raylib.h"
#include "peng.h"

#define WINDOW_WIDTH 1440 
#define WINDOW_HEIGHT 900 

int main(void) {
	int fps;

	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Peng");	

	startPeng(
		WINDOW_WIDTH, 
		WINDOW_HEIGHT, 
		20000, 
		10 
	);

	toggleAttractorForce();
	
	// spawnParticleAt(100, 100, RAYWHITE);
	// spawnParticleAt(100, 101, RAYWHITE);
	spawnParticlesRandom();
	createMouseAttractor(2.0f);

	while (!WindowShouldClose()) {
		float dt = GetFrameTime();
		fps = GetFPS();

		if (IsKeyPressed(KEY_SPACE)) {
			toggleAttractorForce();
		}

		runUpdate(dt);

		BeginDrawing();

		ClearBackground(BLACK);
		drawParticles();
		DrawText(TextFormat("FPS: %d", fps), 10, 10, 18, RAYWHITE);

		EndDrawing();
	}

	stopPeng();
	CloseWindow();
	return 0;
}
