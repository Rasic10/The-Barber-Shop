#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUMBER_OF_CLIENT 10
#define NUMBER_OF_SEATS 3

pthread_mutex_t the_mutex;
pthread_mutex_t mutex_queue;
pthread_mutex_t mutex_wait;
pthread_mutex_t mutex_sleep;
pthread_cond_t condB, condC, condSleep, condFirst;

int t; //time
int number_client = 0;
int signal = 0;
int sleeping = 0;
int theEnd = NUMBER_OF_CLIENT;

void *barber();
void *clientMaker();
void *waitingRoom();

int main(void) {
	pthread_t bar, con;
	
	t = time(NULL);
	srand(t);
	
	printf("\t\t\t-The Barber Shop is open-\n\n");
	
	//inicijalizacija
	pthread_mutex_init(&the_mutex, NULL);
	pthread_cond_init(&condB, NULL);
	pthread_cond_init(&condC, NULL);
	
	//kreiranje niti
	pthread_create(&bar, NULL, barber, NULL);
	pthread_create(&con, NULL, clientMaker, NULL);
	
	pthread_join(bar, NULL);
	pthread_join(con, NULL);
	
	//unistavanje
	pthread_mutex_destroy(&the_mutex);
	pthread_cond_destroy(&condB);
	pthread_cond_destroy(&condC);
	
	return 0;
}

void *barber() {
	int i = 0, br = 0;
	while(i == 0) {
		if(number_client == 0) {
			printf("The shop is empty, the barber sleeps.\n");
			pthread_mutex_lock(&mutex_sleep);
			sleeping = 1;
			if(br == 0) {
				pthread_cond_signal(&condFirst);
				br = 1;
			}
			pthread_cond_wait(&condSleep, &mutex_sleep);
			sleeping = 0;
			pthread_mutex_unlock(&mutex_sleep);
			printf("The barber wakes up.\n");
		}
		else {
			br = random()%10;
			printf("The barber begins working..(%dmin)\n", br);
			sleep(br);
			number_client--;
			printf("Working is done, the client leaves the shop.\n");
			signal++;
			pthread_cond_signal(&condB);
			if(theEnd == signal) {
				i = 1;
				printf("\n\t\t\t-The Barber Shop is closed-\n");
			}
		}
	}
}

void *clientMaker() {
	int i, br;
	pthread_t client[NUMBER_OF_CLIENT];
	pthread_cond_wait(&condFirst, &the_mutex);
	for(i = 0; i < NUMBER_OF_CLIENT; i++) {
		pthread_create(&client[i], NULL, waitingRoom, NULL);
		sleep(rand()%5);
	}
}

void *waitingRoom() {
	pthread_mutex_lock(&mutex_queue);
	number_client++;
	printf("The client walks in and checks all the chairs.\n");
	if(number_client <= NUMBER_OF_SEATS + 1) {
		if(sleeping==1) {
			printf("The barber sleeps, the client wakes him up.\n");
			pthread_cond_signal(&condSleep);
			printf("The client sits down in a room meant for working.\n");
		} else {
			printf("Barber is working.. ");
			printf("The client sits down in a room meant for waiting.\n");
		}
		pthread_mutex_unlock(&mutex_queue);
		pthread_mutex_lock(&mutex_wait);
		pthread_cond_wait(&condB, &mutex_wait);
		pthread_mutex_unlock(&mutex_wait);
		return;
	}
	else {
		printf("All the chairs are taken, the client leaves  the shop.\n");
		number_client--;
		theEnd--;
		pthread_mutex_unlock(&mutex_queue);
		return;
	}
}
