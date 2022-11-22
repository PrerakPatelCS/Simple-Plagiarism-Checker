#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


struct charNode{
    char character;
    struct charNode* next;
    struct charNode* prev;
};

struct stringNode{
    char* word;
    struct stringNode* next;
    struct stringNode* prev;

    int occurrences;
    double frequency;
    char* filename;
};



struct charNode* newCharNode(char character);
struct charNode* enqueue(char character, struct charNode* head);
void freeCharNodes(struct charNode* head);
int getLength(struct charNode* head);
struct stringNode* newStringNode(char* word, char* filename);
void printStringNodes(struct stringNode* head);
struct stringNode*insert_StringNode(struct stringNode* node, struct stringNode* head);
void freeStringNodes(struct stringNode* head);
int getWordCount(struct stringNode* head);
void setFrequencies(struct stringNode* head);
struct stringNode* getWFD(struct stringNode* wordList, char* filename);

