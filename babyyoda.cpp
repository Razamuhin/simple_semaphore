/*************************************************************************************
 * babyyoda - store front implementation for baby Yoda doll business
 *
 *************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include "Semaphore.h"
#include <vector>

// Shared semaphores
Semaphore *empty = NULL;
Semaphore *full = NULL;

// Mutex to protect the buffer
pthread_mutex_t buf_mutex;

// The buffer - chose a vector for simplicity of expansion
std::vector<int> buffer;

// Tracks number of dolls purchased
int consumed = 0;


/*************************************************************************************
 * producer_routine - this function is called when the producer thread is created.
 *
 *			Params: data - pointer to an integer indicating number of dolls to produce
 *
 *			Returns: always NULL
 *
 *************************************************************************************/

void *producer_routine(void *data) {

	time_t rand_seed;
	srand((unsigned int) time(&rand_seed));

	// The current serial number (incremented)
	unsigned int serialnum = 1;

	// We know the data pointer is an integer that indicates the number to produce
	int left_to_produce = *((int *) data);


	// Loop through the amount we're going to produce and place into the buffer
	while (left_to_produce > 0) {
		printf("Producer wants to put Yoda #%d into buffer...\n", serialnum);

		// Semaphore check to make sure there is an available slot
		full->wait();

		// Place item on the next shelf slot by first setting the mutex to protect our buffer vars
		pthread_mutex_lock(&buf_mutex);

		buffer.push_back(serialnum);
		serialnum++;
		left_to_produce--;

		printf("   Yoda put on shelf.\n");
		pthread_mutex_unlock(&buf_mutex);

		// Semaphore signal that there are items available
		empty->signal();

		// random sleep but he makes them fast so 1/20 of a second
		usleep((useconds_t) (rand() % 200000));

	}
	return NULL;
}


/*************************************************************************************
 * consumer_routine - this function is called when the consumer thread is created.
 *
 *       Params: none
 *
 *       Returns: always NULL
 *
 *************************************************************************************/

void *consumer_routine(void*) {
	// Consumer will continue to buy baby Yoda dolls until they are out of stock
	// ...probably reselling on eBay...
	while (true) {
		printf("Consumer wants to buy a Yoda...\n");

		// Semaphore to see if there are any items to take
		empty->wait();

		// Take an item off the shelf
		pthread_mutex_lock(&buf_mutex);

		printf("   Consumer bought Yoda #%d.\n", buffer.back());
		buffer.pop_back();
		consumed++;

		pthread_mutex_unlock(&buf_mutex);

		// Consumers wait up to one second
		usleep((useconds_t) (rand() % 1000000));

		full->signal();
	}
	printf("Consumer goes home.\n");

	return NULL;
}


/*************************************************************************************
 * main - Standard C main function for the storefront.
 *
 *		Expected params: babyyoda <buffer_size> <num_consumers> <max_items>
 *				buffer_size - howw many baby Yoda dolls will fit on the shelves
 *				num_consumers - number of customers who want a baby Yoda at any given time
 *				max_items - how many items will be produced before the shop closes
 *
 *************************************************************************************/

int main(int argv, const char *argc[]) {

	// Get our argument parameters
	if (argv < 4) {
		printf("Invalid parameters. Format: %s <buffer_size> <num_consumers> <max_items>\n", argc[0]);
		exit(0);
	}

	// User input on the size of the buffer
	int buffer_size = (unsigned int) strtol(argc[1], NULL, 10);

	int num_consumers = (unsigned int) strtol(argc[2],NULL,10);

	int num_produce = (unsigned int) strtol(argc[3], NULL, 10);


	printf("Producing %d today.\n", num_produce);

	// Initialize our semaphores
	empty = new Semaphore(0);
	full = new Semaphore(buffer_size-1);

	pthread_mutex_init(&buf_mutex, NULL); // Initialize our buffer mutex

	pthread_t producer;
	pthread_t consumer[num_consumers];

	// Launch our producer thread
	pthread_create(&producer, NULL, producer_routine, (void *) &num_produce);

	// Launch our consumer thread
	for(int i = 0; i < num_consumers; i++){
		pthread_create(&consumer[i], NULL, consumer_routine, NULL);
	}

	// Wait for our producer thread to finish up
	pthread_join(producer, NULL);

	printf("The manufacturer has completed his work for the day.\n");

	printf("Waiting for consumer to buy up the rest.\n");

	// Give the consumers a second to finish snatching up items
	while (consumed < num_produce)
		sleep(1);

	// Kick remaining customers out of the store
	for (int i=0; i<num_consumers; i++) {
		pthread_cancel(consumer[i]);
	}

	printf("Out of stock! Closing up shop...\n");

	// Clean up
	delete empty;
	delete full;

	printf("Producer/Consumer simulation complete!\n");
}
