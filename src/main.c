#include "raylib.h"
#include "peng/peng.h"

#define WINDOW_WIDTH 1440 
#define WINDOW_HEIGHT 900 

int main(void) {
	int fps;

	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Peng");	

	startPeng(
		WINDOW_WIDTH, 
		WINDOW_HEIGHT, 
		30000, 
		10 
	);

	toggleAttractorForce();
	// setSlowParticleColor(BLACK);
	
	// spawnParticleAt(100, 100, RAYWHITE);
	// spawnParticleAt(100, 101, RAYWHITE);
	spawnParticlesRandom();
	createMouseAttractor(2.0f, 0.33f);

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
