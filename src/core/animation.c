#include "engine.h"
#include <math.h>
#include <raymath.h>

void updateAttractorAnimation(Attractor* self, float dt) {
	if (!self->isAnimated || self->pathLen < 2) return;

	self->elapsedTime += dt;

	float t = self->isLooping // t = 13 | t = 19.0
	? fmodf(self->elapsedTime, self->totalTime)
	: fminf(self->elapsedTime, self->totalTime);

	// totalTime = 20.0f
	// path = [p0, p1, p2, p3]
	size_t segCount = self->pathLen - 1; // we have 3 segments p0 -> p1; p1 -> p2; p2 -> p3
	float segDuration = self->totalTime / segCount;  // 20.0f / 3 => 6.66 sec per duration
	int segIndex = (int)(t / segDuration); // 13 / 6.66 => int(1.95) => 1

	if (segIndex >= (int)segCount) segIndex = (int)self->pathLen - 2; // false
	float localT = (t - segIndex * segDuration) / segDuration; // 13 - 1 * 6.66 => 6.34 / 6.66 => 0.95
	Vector2 p0 = self->path[segIndex]; // p1
	Vector2 p1 = self->path[segIndex+1]; // p2

	self->pos = Vector2Lerp(p0, p1, localT); // final state: p1 -> p2 @ 95% completion
}


