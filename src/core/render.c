#include "raylib.h"
#include "engine.h"

void drawParticles() {
	DrawTexture(ENGINE.particleTexture, 0, 0, WHITE);
}

void drawForcesUi(int x, int y, int gap, int fontSize, Color fontColor) {
	if (ENGINE.renderUi) {
		DrawText(TextFormat("Attractor Force: %s", ENGINE.useAttractorForce ? "True" : "False"), x, y, fontSize, fontColor);
		DrawText(TextFormat("Repellent Force: %s", ENGINE.useRepellentForce ? "True" : "False"), x, y + gap, fontSize, fontColor);
		DrawText(TextFormat("Friction Force: %s", ENGINE.useFrictionForce ? "True" : "False"), x, y + gap * 2, fontSize, fontColor);
	}
}

void drawAttractors() {
	for (size_t a = 0; a < ENGINE.attractorCount; ++a) {
		DrawCircleV(ENGINE.attractors[a].pos, 5, RED);
	};
}
