#include "raylib.h"
#include "../src/peng.h"

#define WINDOW_WIDTH 1440 
#define WINDOW_HEIGHT 900 

int main(void) {
	int fps;
	Color fastColors[] = {
	    {245, 224, 220, 255}, // Rosewater
	    {242, 205, 205, 255}, // Flamingo
	    {245, 194, 231, 255}, // Pink
	    {203, 166, 247, 255}, // Mauve
	    {243, 139, 168, 255}, // Red
	    {250, 179, 135, 255}, // Peach
	    {249, 226, 175, 255}, // Yellow
	    {166, 227, 161, 255}, // Green
	    {148, 226, 213, 255}, // Teal
	    {137, 180, 250, 255}  // Blue
	};
	Color slowColors[] = {
		BLACK	
	};

	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Peng");	

	startPeng(
		WINDOW_WIDTH, 
		WINDOW_HEIGHT, 
		40000, 
		10 
	);

	setSlowParticleColors(slowColors, 1);
	setFastParticleColors(fastColors, 10);
	spawnParticlesRandom();

	toggleParticlesFrozen();
	createMouseAttractor(2.0f, 0.33f);

	// DisableCursor();

	while (!WindowShouldClose()) {
		float dt = GetFrameTime();
		fps = GetFPS();

		if (IsKeyPressed(KEY_SPACE)) {
			toggleParticlesFrozen();
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
