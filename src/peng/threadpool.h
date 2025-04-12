#ifndef PENG_THREADPOOL_H

#define PENG_THREADPOOL_H

#include <pthread.h>
#include "peng.h"

typedef void (*TaskFn)(void* arg);

typedef struct {
	pthread_t thread;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	bool shouldRun;
	bool shouldQuit;
	bool isDone;
	void* taskData;
	TaskFn taskFn;
} WorkerThread;

void* runMainWorkerThread(void* arg);
void startWorker(WorkerThread* worker, void* taskData, TaskFn taskFn);
void dispatchWorker(WorkerThread* worker);
void awaitWorker(WorkerThread* worker);
void stopWorker(WorkerThread* worker);

#endif // PENG_THREADPOOL_H
