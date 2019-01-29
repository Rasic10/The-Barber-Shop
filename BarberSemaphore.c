#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NUMBER_OF_CLIENT 10
#define NUMBER_OF_SEATS 3

sem_t barberWakeUp;
sem_t barberSleep;

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
	
	sem_init(&barberWakeUp, 0, 0);
	sem_init(&barberSleep, 0, 0);
	
	//kreiranje niti
	pthread_create(&bar, NULL, barber, NULL);
	pthread_create(&con, NULL, clientMaker, NULL);
	
	pthread_join(bar, NULL);
	pthread_join(con, NULL);
	
	sem_destroy(&barberWakeUp);
	sem_destroy(&barberSleep);
	
	return 0;
}

void *barber() {
	int i = 0, br = 0;
	while(i == 0) {
		if(number_client == 0) {
			printf("The shop is empty, the barber sleeps.\n");
			sleeping = 1;
			sem_post(&barberSleep);
			sem_wait(&barberWakeUp);
			sleeping = 0;
			printf("The barber wakes up.\n");
		}
		else {
			br = random()%10;
			printf("The barber begins working..(%dmin)\n", br);
			sleep(br);
			number_client--;
			printf("Working is done, the client leaves the shop.\n");
			signal++;
			if(theEnd == signal) {
				i = 1;
				printf("\n\t\t\t-The Barber Shop is closed-\n");
			}
		}
	}
}

void *clientMaker() {
	int i;
	int br;
	pthread_t client[NUMBER_OF_CLIENT];	
	sem_wait(&barberSleep);
	for(i = 0; i < NUMBER_OF_CLIENT; i++) {
		if(i == 1) sleep(5);
		pthread_create(&client[i], NULL, waitingRoom, NULL);
		sleep(rand()%5);
	}
}

void *waitingRoom() {
	number_client++;
	printf("The client walks in and checks all the chairs.\n");
	if(number_client <= NUMBER_OF_SEATS + 1) {
		if(sleeping==1) {
			printf("The barber sleeps, the client wakes him up.\n");
			printf("The client sits down in a room meant for working.\n");
			sem_post(&barberWakeUp);
		} else {
			printf("Barber is working.. ");
			printf("The client sits down in a room meant for waiting.\n");
		}
		return;
	}
	else {
		printf("All the chairs are taken, the client leaves  the shop.\n");
		number_client--;
		theEnd--;
		return;
	}
}
