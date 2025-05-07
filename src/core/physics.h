#ifndef PHYSICS_H

#define PHYSICS_H

#define FRICTION_SCALAR 0.98f
#define REPELLING_RADIUS 10
#define MAX_SPEED 120.0f
#define AMBIENT_LIGHT 0.1f

void* runMtPhysUpdate(void* arg);
float cubicEaseOut(float t);
float sineEaseOut(float t);

#endif // PHYSICS_H
