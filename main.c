#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <string.h>

#define THREAD_NUM 4

sem_t sem_empty;
sem_t sem_full;

pthread_mutex_t mutex_buffer;

int buffer[10];
int count = 0;

void* producer(void* args) {
  while(1) {
    int x = rand() % 100;
    sleep(1);

    sem_wait(&sem_empty);
    pthread_mutex_lock(&mutex_buffer);

      buffer[count] = x;
      count++;

    pthread_mutex_unlock(&mutex_buffer);
    sem_post(&sem_full);
  }
}

void* consumer(void* args) {
  while(1) {
    int y;

    sem_wait(&sem_full);
    pthread_mutex_lock(&mutex_buffer);

    y = buffer[count - 1];
    count--;

    pthread_mutex_unlock(&mutex_buffer);
    sem_post(&sem_empty);

    printf("Got %d\n", y);
  }
}

int main(int argc, char *argv[])
{
  pthread_t th[THREAD_NUM];

  pthread_mutex_init(&mutex_buffer, NULL);

  sem_init(&sem_empty, 0, 10);
  sem_init(&sem_full, 0, 0);

  int i;

  for (i = 0; i < THREAD_NUM; i++) {
    if(i > 0) {
        if (pthread_create(&th[i], NULL, &producer, NULL) != 0) {
          perror("failed to create thread");
        }
    } else {
        if (pthread_create(&th[i], NULL, &consumer, NULL) != 0) {
          perror("failed to create thread");
        }
    }
  }

  // Join threads
  for (i = 0; i < THREAD_NUM; i++) {
    if (pthread_join(th[i], NULL) != 0) {
      perror("failed to join thread");
    }
  }

    sem_destroy(&sem_empty);
    sem_destroy(&sem_full);
    pthread_mutex_destroy(&mutex_buffer);
    return 0;
}
