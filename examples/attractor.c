#include "raylib.h"
#include <raymath.h>
#include <stdio.h>
#include "../src/peng.h"

#define WINDOW_WIDTH 1440 
#define WINDOW_HEIGHT 900 

int main(void) {
	int fps;

	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Peng");	

	startPeng(
		WINDOW_WIDTH, 
		WINDOW_HEIGHT, 
		1000000, 
		10 
	);

	Vector2 path[] = {
		(Vector2) {.x = 10.0f, .y = 10.0f},
		(Vector2) {.x = 100.0f, .y = 100.0f},
		(Vector2) {.x = 200.0f, .y = 200.0f},
	};

	// Image img  = LoadImage("../assets/neon-flower.jpg");
	// spawnParticlesFromImage(&img, (Vector2){.x = WINDOW_WIDTH / 2 - img.width / 2, .y = WINDOW_HEIGHT / 2 - img.height / 2}, 2);	

	AttractorId id = spawnAnimatedAttractor(path, PENG_ARRAY_LEN(path), 5.0f, false, 2.0f, 0.33f);
	printf("id: %d", id);
	toggleRepellentForce();
	toggleParticlesFrozen();
	// createMouseAttractor(2.0f, 0.33f);

	// DisableCursor();

	while (!WindowShouldClose()) {
		float dt = GetFrameTime();
		fps = GetFPS();

		handleKeyInputs();
		runUpdate(dt);

		BeginDrawing();
		
		ClearBackground(BLACK);
		drawParticles();
		drawAttractors();

		DrawText(TextFormat("FPS: %d", fps), 10, 10, 18, RAYWHITE);
		drawForcesUi(10, 30, 20, 18, RAYWHITE);

		EndDrawing();
	}

	stopPeng();

	// UnloadImage(img);
	CloseWindow();
	return 0;
}
