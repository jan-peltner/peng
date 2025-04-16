#include <raylib.h>
#include "engine.h"

void handleKeyInputs() {
	if (IsKeyPressed(KEY_SPACE)) {
		toggleParticlesFrozen();
	}
	if (IsKeyPressed(KEY_A)) {
		toggleAttractorForce();
	}
	if (IsKeyPressed(KEY_F)) {
		toggleFrictionForce();
	}
	if (IsKeyPressed(KEY_R)) {
		toggleRepellentForce();
	}
}
