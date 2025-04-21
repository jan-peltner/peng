#include "raylib.h"
#include <raymath.h>
#include "../src/peng.h"

#define WINDOW_WIDTH 1440 
#define WINDOW_HEIGHT 900 
#define PENG_LOG

int main(void) {
	int fps;

	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Peng");	

	startPeng(
		WINDOW_WIDTH, 
		WINDOW_HEIGHT, 
		1000000, 
		10,
		10
	);

	float animationSpeed = 1.0f;

	Vector2 topPath[] = {
		(Vector2) {.x = 0.0f, .y = 200.0f},
		(Vector2) {.x = 1439.0f, .y = 200.0f},
	};
	Vector2 bottomPath[] = {
		(Vector2) {.x = 1439.0f, .y = 700.0f},
		(Vector2) {.x = 0.0f, .y = 700.0f},
	};

	Event events[] = {
		(Event) {.dispatchAt = 3.0f, .fn = toggleAttractorForce, .eventData = NULL},
		(Event) {.dispatchAt = 10.0f, .fn = toggleAttractorForce, .eventData = NULL}
	};

	scheduleEvents(events, PENG_ARRAY_LEN(events));

	Image img  = LoadImage("../assets/neon-flower.jpg");
	spawnParticlesFromImage(
		&img, 
		(Vector2){.x = WINDOW_WIDTH / 2 - img.width / 2, .y = WINDOW_HEIGHT / 2 - img.height / 2},
		4,
		BLACK	
	);	

	AttractorId top = spawnAnimatedAttractor(topPath, PENG_ARRAY_LEN(topPath), animationSpeed, true, LOOP_PINGPONG, 4.0f, 0.33f);
	AttractorId bot = spawnAnimatedAttractor(bottomPath, PENG_ARRAY_LEN(bottomPath), animationSpeed, true, LOOP_PINGPONG, 4.0f, 0.33f);
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

	UnloadImage(img);
	CloseWindow();
	return 0;
}
