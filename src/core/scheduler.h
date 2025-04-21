#ifndef SCHEDULER_H

#define SCHEDULER_H

#include <raylib.h>
#include "../peng.h"

typedef struct {
	Event* events;
	size_t eventCount;
	size_t eventCap;
	size_t eventCursor;
} Scheduler;

void pollScheduler();

#endif // SCHEDULER_H
