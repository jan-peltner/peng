#include "engine.h"
#include "../peng.h"
#include <math.h>
#include <raymath.h>
#include <stdio.h>

void updateAttractorAnimation(Attractor* self, float dt) {
        if (!self->isAnimated || self->pathLen < 2) return;

        self->elapsedTime += dt;

        float t;
        if (!self->isLooping) {
                // simple one‐time clamp
                t = fminf(self->elapsedTime, self->totalTime);
        } else {
                if (self->loopMode == LOOP_WRAP) {
                        // wrap back to start
                        t = fmodf(self->elapsedTime, self->totalTime);
                }
                else /* LOOP_PINGPONG */ {
                        // forward + backward in one cycle
                        float cycleTime = self->totalTime * 2.0f;
                        float timeInCycle = fmodf(self->elapsedTime, cycleTime);
                        if (timeInCycle <= self->totalTime) {
                            t = timeInCycle;
                        } else {
                            t = cycleTime - timeInCycle;
                        }
                }
        }

        // interpolate along the correct segment
        size_t segCount    = self->pathLen - 1;
        float  segDuration = self->totalTime / segCount;
        int    segIndex    = (int)(t / segDuration);
        if (segIndex >= (int)segCount) segIndex = (int)segCount - 1;

        float localT = (t - segIndex * segDuration) / segDuration;
        Vector2 p0  = self->path[segIndex];
        Vector2 p1  = self->path[segIndex + 1];

        self->pos = Vector2Lerp(p0, p1, localT);
}
