#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <pthread.h>

class Semaphore
{
public:

	Semaphore(int count);
	~Semaphore();

	void wait();
	void signal();
	int val;

private:
	pthread_cond_t status;
    pthread_mutex_t mutex;
};

#endif
