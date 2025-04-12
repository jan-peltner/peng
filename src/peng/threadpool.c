#include <pthread.h>
#include "threadpool.h"

void* runMainWorkerThread(void* arg) {
	WorkerThread* worker = (WorkerThread*)arg;

	while(true) {
		pthread_mutex_lock(&worker->mutex);

		while (!worker->shouldRun && !worker->shouldQuit) {
			pthread_cond_wait(&worker->cond, &worker->mutex);
		}

		if(worker->shouldQuit) {
			pthread_mutex_unlock(&worker->mutex);
			break;
		}

		worker->shouldRun = false;
		pthread_mutex_unlock(&worker->mutex);

		worker->taskFn(&worker->taskData);	
		
		pthread_mutex_lock(&worker->mutex);
		worker->isDone = true;
		pthread_mutex_unlock(&worker->mutex);
	}

	return NULL;
};

void startWorker(WorkerThread* worker, void* taskData, TaskFn taskFn) {
	pthread_mutex_init(&worker->mutex, NULL);
	pthread_cond_init(&worker->cond, NULL);

	worker->shouldRun = false;
	worker->shouldQuit = false;
	worker->isDone = false;
	worker->taskData = taskData;
	worker->taskFn = taskFn;

	pthread_create(&worker->thread, NULL, runMainWorkerThread, (void*)worker);
}

void dispatchWorker(WorkerThread* worker) {
	pthread_mutex_lock(&worker->mutex);
	worker->isDone = false;
	worker->shouldRun = true;
	pthread_cond_signal(&worker->cond);
	pthread_mutex_unlock(&worker->mutex);
}

void awaitWorker(WorkerThread* worker) {
	while(true) {
		pthread_mutex_lock(&worker->mutex);
		bool done = worker->isDone;
		pthread_mutex_unlock(&worker->mutex);
		if (done) break;
	}
}

void stopWorker(WorkerThread* worker) {
	pthread_mutex_lock(&worker->mutex);
	worker->shouldQuit = true;
	pthread_mutex_unlock(&worker->mutex);
	pthread_cond_signal(&worker->cond);

	pthread_join(worker->thread, NULL);
	pthread_mutex_destroy(&worker->mutex);
	pthread_cond_destroy(&worker->cond);
}
