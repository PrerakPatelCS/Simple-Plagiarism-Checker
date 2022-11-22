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
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <pthread.h>


#ifndef DEBUG
#define DEBUG 0
#endif


struct Aargs{
    struct WFD_Node** WFD_Array;
    struct pair *compareArr;
    double *results;
    int start;
    int comparisons;
};

struct pair{
	int first;
	int second;
	int done;
};

struct WFD_Node{
    struct stringNode* wordList;
    struct WFD_Node* next;
    struct WFD_Node* prev;

    int wordCount;
    char* filename;

};

struct WFD_Node* new_WFD_Node(struct stringNode* word_list);
struct WFD_Node* insert_WFD_Node(struct stringNode* word_list, struct WFD_Node* head);
unsigned get_WFD_Length(struct WFD_Node* head);
void print_WFD_Nodes(struct WFD_Node*);
void free_WFD_Nodes(struct WFD_Node*);
struct stringNode* getMeanFreq(struct WFD_Node* head1, struct WFD_Node* head2);
double getKLD(struct WFD_Node* file, struct stringNode* meanFreq);
double getJSD(struct WFD_Node* firstNode, struct WFD_Node* secondNode);
void *fileThreadFunct(void *A);
void *analyzeThreadFunc(void *A);
