#ifndef UTILS_H

#define UTILS_H

#include <raylib.h>

float cubicEaseOut(float t);
float sineEaseOut(float t);
float normalizedDistanceTo(Vector2 from, Vector2 to, float winDiag);
Vector2 unitVectorTo(Vector2 from, Vector2 to);

#endif // UTILS_H
