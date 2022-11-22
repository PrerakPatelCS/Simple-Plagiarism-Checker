#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <pthread.h>

struct queue {
        struct node *front, *rear;
        pthread_mutex_t lock;
        pthread_cond_t read_ready;
        int count;
        int active;
};

struct node {
        char *fileName;
        struct node* next;
};

struct Dargs {
        char* suffix;
};


struct node* newNode(char *fileName);
struct queue* createQueue();
void enQueue(struct queue* queue, char* fileName);
struct node* deQueue(struct queue* queue);
void freeQueue(struct node* queue);
void printNodes(struct node* head);
void *dirThreadFunct(void *A);

