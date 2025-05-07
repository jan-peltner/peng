#include "../peng.h"
#include "utils.h"
#include <math.h>
#include <raymath.h>

float cubicEaseOut(float t) {
	return 1 - powf(1 - t, 3);
}

float sineEaseOut(float t) {
	return sinf((t * PI) / 2);
}

float normalizedDistanceTo(Vector2 from, Vector2 to, float winDiag) {
	Vector2 delta = Vector2Subtract(to, from);
	return sqrtf(delta.x * delta.x + delta.y * delta.y) / winDiag;
}

Vector2 unitVectorTo(Vector2 from, Vector2 to) {
	Vector2 delta = Vector2Subtract(to, from);
	float length = sqrtf(delta.x * delta.x + delta.y * delta.y);
	return (Vector2) {
		.x = delta.x / length,
		.y = delta.y / length
	};
}

