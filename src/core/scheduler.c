#include "engine.h"
#include "scheduler.h"
#include <float.h>
#include <stdio.h>

void scheduleEvents(const Event* events, size_t amount) {
        bool used[amount];
        for (size_t i = 0; i < amount; ++i) used[i] = false;

        for (size_t i = 0; i < amount; ++i) {
                float minTime = FLT_MAX;
                int minIndex = -1;

                for (size_t j = 0; j < amount; ++j) {
                        if (!used[j] && events[j].dispatchAt < minTime) {
                        minTime = events[j].dispatchAt;
                        minIndex = j;
                        }
                }

                if (minIndex != -1 && ENGINE.scheduler.eventCount < ENGINE.scheduler.eventCap) {
                        ENGINE.scheduler.events[i] = events[minIndex];
                        ++ENGINE.scheduler.eventCount;
                        used[minIndex] = true;
                }
        }
}

void pollScheduler() {
        size_t cursor = ENGINE.scheduler.eventCursor;
        if (cursor >= ENGINE.scheduler.eventCount) return;
        if (ENGINE.time >= ENGINE.scheduler.events[cursor].dispatchAt) {
                Event event = ENGINE.scheduler.events[cursor];
                event.fn(event.eventData);
                ++ENGINE.scheduler.eventCursor;
        }
}
