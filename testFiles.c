#include "files.h"


struct queue* dirQueue;
struct queue* fileQueue;
int numFiles = 0;


/**
 * Checks each character in char array of input to see if each is a digit 0-9.
 * @param : address of argv[1] from main (number input for width)
 * @return : Returns 0 (false) if non 0-9 char detected, 1 (true) otherwise.
 */
int checkIfValidNumber(char* input){
    for(int i = 0; input[i] != '\0'; i++){
        //isdigit checks if char at input[i] is 0-9
        if(isdigit(input[i]) == 0){
            return 0;
        }
    }
    if(atoi(input) < 1){
    	return 0;
    }
    return 1;
}


int main(int argc, char *argv[]){
    dirQueue = createQueue();
    fileQueue = createQueue();
    pthread_t *dir_tids;
    int dirThreads = 1;
    char *suffix = ".txt";
    char *fileName;
    for(int i = 1; i < argc; i++){
        if(argv[i][0] == '-'){
            if(strlen(argv[i]) >= 2){
                if(argv[i][1] == 'd'){
                    argv[i] += 2;
                    if(checkIfValidNumber(argv[i]) == 1){
                        dirThreads = atoi(argv[i]);
                    }
                }
            }
        }
        else{
            struct stat data;
            fileName = argv[i];
            fileName = realpath(fileName, NULL);
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
                if(boolean){
                    enQueue(fileQueue, fileName);
                }
            }
            else if (S_ISDIR(data.st_mode)){
                enQueue(dirQueue, fileName);
            }
            free(fileName);
        }
    }
    dir_tids = malloc(dirThreads * sizeof(pthread_t));
    struct Dargs *Dargs;
    int err = 0;
    Dargs = malloc(dirThreads * sizeof(struct Dargs));
    dirQueue->active = dirThreads;
    for (int i = 0; i < dirThreads; i++) {
        Dargs->suffix = suffix;
        err = pthread_create(&dir_tids[i], NULL, dirThreadFunct, Dargs);
        if(err != 0){
            perror("pthread_create");
        }
    }

    for(int i = 0; i < dirThreads; i++){
        pthread_join(dir_tids[i], NULL);
    }
    struct node* fileQ_head = fileQueue->front;
    free(Dargs);
    free(dir_tids);
    printNodes(fileQ_head);
    printf("NUMBER OF DIRECTORY THREADS = %d\nNUMBER OF FILES = %d\n",dirThreads, numFiles);
    return EXIT_SUCCESS;
}
