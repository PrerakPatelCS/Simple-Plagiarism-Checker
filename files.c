#include "files.h"

extern struct queue* dirQueue;
extern struct queue* fileQueue;
extern int numFiles;


/**
 * @param : filename
 * @return : new node
 */
struct node* newNode(char *fileName){
    struct node* temp = malloc(sizeof(struct node));
    temp->fileName = strdup(fileName);
    temp->next = NULL;
    return temp;
}


/**
 * @return : new queue
 */
struct queue* createQueue(){
        struct queue* queue;
        queue = malloc(sizeof(struct queue));
        queue->count = 0;
        queue->active = 0;
        pthread_mutex_init(&queue->lock, NULL);
        queue->front = queue->rear = NULL;
        return queue;
}


/**
 * @param : queue, filename
 */
void enQueue(struct queue* queue, char* fileName){

        // lock
        pthread_mutex_lock(&queue->lock);

        // Create a new LL node
        struct node* temp = newNode(fileName);

        // If queue is empty, then new node is front and rear both
        if (queue->rear == NULL) {
                queue->front = queue->rear = temp;
                queue->count++;
                pthread_mutex_unlock(&queue->lock);
                //pthread_cond_signal(&q->read_ready);
                return;
        }

        // Add the new node at the end of queue and change rear
        queue->rear->next = temp;
        queue->rear = temp;
        queue->count++;
        pthread_mutex_unlock(&queue->lock);
}


/**
 * @param : queue
 * @return : node at the front
 */
struct node* deQueue(struct queue* queue){
        //lock
        pthread_mutex_lock(&queue->lock);
        // If queue is empty, return NULL. and decrement active threads
        if (queue->front == NULL){
                queue->active--;
                if(queue->active == 0){
                        pthread_mutex_unlock(&queue->lock);
                        return NULL;
                }
                if(queue->front == NULL && queue->active > 0){
                        pthread_mutex_unlock(&queue->lock);
                        return NULL;
                }
                queue->active++;
        }

        // Store previous front and move front one node ahead
        struct node* temp = queue->front;

        queue->front = queue->front->next;

        // If front becomes NULL, then change rear also as NULL
        if (queue->front == NULL)
                queue->rear = NULL;
        queue->count--;
        pthread_mutex_unlock(&queue->lock);

        return(temp);
}


/**
 * @param : node at front of queue
 */
void freeQueue(struct node* node){
    if(node == NULL){
        return;
    }

    struct node* next = node->next;
    free(node->fileName);
    free(node);
    freeQueue(next);
}


/**
 * Prints nodes in linked list depending on outputFileDesc
 */
void printNodes(struct node* head){
    printf("PrintNodes\n");
    if (head == 0 ){
        return;
    }
    struct node* ptr = head;
    while(ptr != 0){
        printf("%s\n", ptr->fileName);
        ptr = ptr->next;
    }
}


/**
 * Goes through Directories to find files and puts files with the suffix into Files Queue
 * @param : Dargs 
 * Lists all files and sub-directories recursively
 * considering path as base path.
 */
void *dirThreadFunct(void *A){
        struct Dargs *Dargs = A;
        char* suffix = Dargs->suffix;

        struct node* directory;
        while((directory = deQueue(dirQueue)) != NULL){
                if(dirQueue->active == 0){
                        break;
                }
                struct dirent *dp;
                DIR *dir = opendir(directory->fileName);
                char* fileName;
                // Unable to open directory stream
                if (!dir){
                        perror("null directory");
                        abort();
                }
                while ((dp = readdir(dir)) != NULL) {
                        if(dp->d_type == DT_DIR){
                                if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){

                                        fileName = malloc((strlen(directory->fileName) + 1 + strlen(dp->d_name) + 1)  * sizeof(char));
                                        strcpy(fileName, directory->fileName);
                                        strcat(fileName, "/");
                                        strcat(fileName, dp->d_name);
                                        enQueue(dirQueue, fileName);
                                        free(fileName);
                                }
                        }
                        else{
                            //("FILE FOUND\n");
                                struct stat data;
                                fileName = malloc((strlen(directory->fileName) + 1 + strlen(dp->d_name) + 1)  * sizeof(char));
                                strcpy(fileName, directory->fileName);
                                strcat(fileName, "/");
                                strcat(fileName, dp->d_name);
                                if(stat(fileName, &data)){
                                        perror(fileName);
                                        abort();
                                }
                                else if (S_ISREG(data.st_mode)){

                                        int len1 = strlen(fileName);
                                        int len2 = strlen(suffix);
                                        int boolean = 0;
                                        for(int i = 0; i < len2; i++){
                                                if(suffix[len2 - i - 1] != fileName[len1 - i - 1]){
                                                        boolean = 0;
                                                        break;
                                                }
                                                else{
                                                        boolean = 1;
                                                }
                                        }
                                        if(boolean || (strcmp(suffix, "") == 0)){
						numFiles++;
                                                enQueue(fileQueue, fileName);
                                        }
                                }
                                free(fileName);
                        }
                }
                free(directory->fileName);
                free(directory);
                closedir(dir);
        }
        pthread_exit(0);



}
