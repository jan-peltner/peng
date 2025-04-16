
#include "raylib.h"
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

	Image img  = LoadImage("../assets/flower.jpg");
	spawnParticlesFromImage(&img, (Vector2){.x = WINDOW_WIDTH / 2 - img.width / 2, .y = WINDOW_HEIGHT / 2 - img.height / 2});	

	toggleRepellentForce();
	toggleParticlesFrozen();
	createMouseAttractor(2.0f, 0.33f);

	// DisableCursor();

	while (!WindowShouldClose()) {
		float dt = GetFrameTime();
		fps = GetFPS();

		handleKeyInputs();
		runUpdate(dt);

		BeginDrawing();
		
		ClearBackground(BLACK);
		drawParticles();

		DrawText(TextFormat("FPS: %d", fps), 10, 10, 18, RAYWHITE);
		drawForcesUi(10, 30, 20, 18, RAYWHITE);

		EndDrawing();
	}

	stopPeng();

	UnloadImage(img);
	CloseWindow();
	return 0;
}
