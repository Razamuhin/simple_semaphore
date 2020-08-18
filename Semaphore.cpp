#include <pthread.h>
#include <iostream>
#include "Semaphore.h"

/*************************************************************************************
 * Semaphore (constructor) - this should take count and place it into a local variable.
 *						Here you can do any other initialization you may need.
 *
 *    Params:  count - initialization count for the semaphore
 *
 *************************************************************************************/

Semaphore::Semaphore(int count) {
    val = count;
    pthread_cond_t status;
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex,NULL);
}


/*************************************************************************************
 * ~Semaphore (destructor) - called when the class is destroyed. Clean up any dynamic
 *						memory.
 *
 *************************************************************************************/

Semaphore::~Semaphore() {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&status);
}


/*************************************************************************************
 * wait - implement a standard wait Semaphore method here
 *
 *************************************************************************************/

void Semaphore::wait() {
    val--;
    if(val < 0){
        pthread_cond_wait(&status,&mutex);
    }
}


/*************************************************************************************
 * signal - implement a standard signal Semaphore method here
 *
 *************************************************************************************/

void Semaphore::signal() {
    val++;
    if(val>=0){
        pthread_cond_signal(&status);
    }
}

// For testing only
using namespace std;
int main(){
    Semaphore mando(10);
    cout << mando.val;
    //pthread_mutex_lock(&mutex);
    //pthread_mutex_unlock(&mutex);
    return 0;
}
