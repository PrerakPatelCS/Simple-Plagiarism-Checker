#include "wordFrequencyDistribution.h"


/**
 * @param : character
 * @return : new character Node
 */
struct charNode* newCharNode(char character){
    struct charNode* temp = malloc(sizeof(struct charNode));
    temp->character = character;
    temp->prev = 0;
    temp->next = 0;
    return temp;
}


/**
 * @param : character and character list head
 * @return : new character to list
 */
struct charNode* enqueue(char character, struct charNode* head){
    if (head == 0){
        head = newCharNode(character);
        return head;
    }
    struct charNode* ptr = head;
    struct charNode* prev = 0;

    while(ptr != 0){
        prev = ptr;
        if(ptr->next != 0){
            ptr = ptr->next;
        }
        else{
            struct charNode* tempNode = newCharNode(character);
            prev->next = tempNode;
            tempNode->prev = prev;
            tempNode->next = 0;
            break;
        }

    }
    return head;
}


/**
 * deallocates memory for entire linked list recursively
 * @param : character list head
 */
void freeCharNodes(struct charNode* head){
    if (head == 0) return;

    struct charNode* next = head->next;

    free(head);
    freeCharNodes(next);
}


/**
 * @param : character list head
 * @return : length of linked list (useful for length of stash)
 */
int getLength(struct charNode* head){
    if(head == 0){
        return 0;
    }
    int nodes = 0;
    struct charNode* ptr = head;
    while(ptr != 0){
        nodes++;
        ptr = ptr->next;
    }
    return nodes;
}


/**
 * New node for word list linked list
 * @param : 2 strings word and filename
 */
struct stringNode* newStringNode(char* word, char* filename){

    struct stringNode* temp = malloc(sizeof(struct stringNode));
    char* tempWord = malloc(sizeof(char) * (strlen(word) + 1));
    char* tempFileName = strdup(filename);

    strcpy(tempWord, word);
    tempWord[strlen(word)] = '\0';

    temp->word = tempWord;
    temp->filename = tempFileName;
    temp->prev = 0;
    temp->next = 0;

    temp->occurrences = 1;
    temp->frequency = 0;

    return temp;
}


/**
 * @param : word list head
 */
void printStringNodes(struct stringNode* head){
    if (head == 0 ){
        return;
    }
    struct stringNode* ptr = head;
    while(ptr != 0){
        printf("Word: %s\n", ptr->word);
        printf("Occurrences: %d\n", ptr->occurrences);
        printf("Frequency: %lf\n", ptr->frequency);
        printf("Filename: %s\n", ptr->filename);
        putchar('\n');
        ptr = ptr->next;
    }
}


/**
 * Insert node in alphabetical order
 * @param : string node and word list head
 * @return : word list
 */
struct stringNode*insert_StringNode(struct stringNode* node, struct stringNode* head){
    if(head == 0){

        return node;
    }

    struct stringNode* ptr = head;
    struct stringNode* prev = 0;

    while(ptr != 0){
        if(strcmp(ptr->word, node->word) == 0){

            //match found
            ptr->occurrences++;
            freeStringNodes(node);
            return head;
        }
        else if (strcmp(node->word, ptr->word) < 0){
            //word is lexicographically before ptr->word
           if(prev != 0){

               prev->next = node;
           }
           node->prev = prev;
           node->next = ptr;
           if(strcmp(node->word, head->word) < 0){
           //if(strcmp(tempNode->word, head->word) < 0){
               //new word is lexicographically before head's word
               //set tempNode as head, and return;
               return node;
           }
           return head;
        }
        else{
            //word is lexicographically after ptr->word
            prev = ptr;
            if (ptr->next != 0){
                //increment ptr
                ptr = ptr->next;
            }
            else{
                //set as last node in list.
                prev->next = node;
                node->prev = prev;
                node->next = 0;
                break;
            }
        }
    }
    return head;
}


/**
 * @param : word list head
 */
void freeStringNodes(struct stringNode* head){
    if (head == 0) return;

    struct stringNode* next = head->next;

    free(head->word);
    free(head->filename);
    free(head);
    freeStringNodes(next);
}


/**
 * @param : word list head
 * @return : word count
 */
int getWordCount(struct stringNode* head){
    if(head == 0){
        return 0;
    }
    int count = 0;
    struct stringNode* ptr = head;
    while(ptr != 0){
        count = count + ptr->occurrences;
        ptr = ptr->next;
    }
    return count;
}


/**
 * Sets the frequency of occurrence of each word within a file
 * @param : word list head
 */
void setFrequencies(struct stringNode* head){

    double totalWords = (double) getWordCount(head);
    double freq = 0;

    struct stringNode* ptr = head;

    while(ptr != 0){
        freq = (double) (ptr->occurrences / totalWords);
        ptr->frequency = freq;
        ptr = ptr->next;
    }
}


/**
 * Parses a given input file to create a linked list containing words and their
 * frequencies(Word Frequency Distribution) in alphabetically organized linked list
 * @param : word list and filename
 * @return : Linked List of words in file
 */
struct stringNode* getWFD(struct stringNode* wordList, char* filename){

    FILE *fp = fopen(filename, "r");
    if(!fp){
        perror("fopen error\n");
        abort();
    }

    struct charNode* read_word = 0;
    struct stringNode* sorted_list = 0;
    char c;

    while((c = getc(fp)) != EOF){
            if((isspace(c) == 0 && ispunct(c) == 0)
                || c == '-'){
                //nonwhitespace char, or hyphen

                if(isalpha(c) != 0){
                    //convert alphabet character to lowercase
                    c = tolower(c);
                }
                read_word = enqueue(c, read_word);
            }
            else{
                //whitespace char or punctuation
                if(isspace(c) == 0){
                    //skip punctuation/nonhyphen chars
                    continue;
                }
                if(getLength(read_word) > 0){
                    //convert char LL to word, add to word LL
                    int length = getLength(read_word);

                    char* nodeWord = malloc(sizeof(char) * (length + 1));
                    struct charNode* ptr = read_word;
                    for(int i = 0; i < length; i++){
                        nodeWord[i] = ptr->character;
                        if(ptr->next != 0){
                            ptr = ptr->next;
                        }
                    }
                    nodeWord[length] = '\0';
                    //add new word to wordList LL
                    struct stringNode* tempNode = newStringNode(nodeWord, filename);
                    sorted_list = insert_StringNode(tempNode, sorted_list);

                    free(nodeWord);
                    nodeWord = 0;

                    freeCharNodes(read_word);
                    read_word = 0;
                }
            }

    }
    if(getLength(read_word) > 0){
        //convert char LL to word, add to word LL
        int length = getLength(read_word);

        char* nodeWord = malloc(sizeof(char) * (length + 1));
        struct charNode* ptr = read_word;
        for(int i = 0; i < length; i++){
            nodeWord[i] = ptr->character;
            if(ptr->next != 0){
                ptr = ptr->next;
            }
        }
        nodeWord[length] = '\0';
        //add new word to wordList LL
        struct stringNode* tempNode = newStringNode(nodeWord, filename);
        sorted_list = insert_StringNode(tempNode, sorted_list);

        free(nodeWord);
        nodeWord = 0;

        freeCharNodes(read_word);
        read_word = 0;
    }
    setFrequencies(sorted_list);
    fclose(fp);
    return sorted_list;
}
