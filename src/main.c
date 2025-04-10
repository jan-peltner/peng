#include "raylib.h"
#include "peng.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

int main(void) {
	int fps;

	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Peng");	

	startPeng(WINDOW_WIDTH, WINDOW_HEIGHT, 1000, 10);
	spawnParticlesRandom();
	createMouseAttractor(2.0f);

	while (!WindowShouldClose()) {
		float dt = GetFrameTime();
		fps = GetFPS();

		if (IsKeyPressed(KEY_SPACE)) {
			toggleAttractors();
		}

		runPhysicsUpdate(dt);		

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
