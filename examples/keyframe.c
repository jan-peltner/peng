#include "raylib.h"
#include <raymath.h>
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
		10,
		10
	);

	Image img  = LoadImage("../assets/neon-flower.jpg");
	spawnParticlesFromImage(&img, (Vector2){.x = WINDOW_WIDTH / 2 - img.width / 2, .y = WINDOW_HEIGHT / 2 - img.height / 2}, 2, BLACK);	

	pushParticleKeyframe(10.0f, 100.0f);

	Event events[] = {
		(Event) {.dispatchAt = 3.0f, .fn = dispatchParticleKeyframe, .eventData = NULL}
	};
	scheduleEvents(events, PENG_ARRAY_LEN(events));
	spawnStaticLight((Vector2){.x = WINDOW_WIDTH / 2 - img.width / 2, .y = WINDOW_HEIGHT / 2 - img.height / 2}, 5.0f);
	createMouseAttractor(1.0f, 0.33f);
	toggleAttractorForce();
	toggleRepellentForce();

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
