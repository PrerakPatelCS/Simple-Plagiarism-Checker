#include "files.h"
#include "wordFrequencyDistribution.h"
#include "analysis.h"


extern struct queue* dirQueue;
extern struct queue* fileQueue;
extern int numFiles;
extern struct WFD_Node* WFD_List;
extern struct comp_result* results;
extern int fileDone;


/**
 * @param : wordList
 * @return : new WFD Node
 */
struct WFD_Node* new_WFD_Node(struct stringNode* word_list){
    struct WFD_Node* temp = malloc(sizeof(struct WFD_Node));
    temp->wordList = word_list;
    temp->wordCount = getWordCount(word_list);
    temp->filename = word_list->filename;

    temp->prev = 0;
    temp->next = 0;
    return temp;
}


/**
 * inserts new WFDNode at the front
 * @param : wordList, WFD_List
 * @return : WFD_List
 */
struct WFD_Node* insert_WFD_Node(struct stringNode* word_list, struct WFD_Node* head){

	if(head == 0){
		struct WFD_Node* temp = new_WFD_Node(word_list);
		return temp;
	}

	struct WFD_Node* ptr = head;
	struct WFD_Node* prev = 0;
	while(ptr != 0){
		struct WFD_Node* tempNode = new_WFD_Node(word_list);
		if(prev != 0){
			prev->next = tempNode;
		}
		tempNode->prev = prev;
		tempNode->next = ptr;
		return tempNode;
    	}
	return head;
}


/**
 * @param : WFD_List
 * @return : length of WFD_List
 */
unsigned get_WFD_Length(struct WFD_Node* head){
    if(head == 0){
        return 0;
    }
    unsigned nodes = 0;
    struct WFD_Node* ptr = head;
    while(ptr != 0){
        nodes++;
        ptr = ptr->next;
    }
    return nodes;
}


/**
 * @param : WFD_List
 */
void free_WFD_Nodes(struct WFD_Node* head){
    if (head == 0){
        return;
    }

    struct WFD_Node* next = head->next;

    freeStringNodes(head->wordList);
    free(head);
    free_WFD_Nodes(next);
}


/**
 * @param : WFD_List
 */
void print_WFD_Nodes(struct WFD_Node* head){
    if (head == 0 ){
        return;
    }
    int count = 1;
    struct WFD_Node* ptr = head;
    while(ptr != 0){
        printf("WFD_Node #%d\n", count);
        printf("WordCount: %d\n", ptr->wordCount);
        printf("FileName: %s\n", ptr->filename);
        //printStringNodes(ptr->wordList);
        ptr = ptr->next;
        count++;
    }
}


/**
 * @param : 2 WFD Nodes
 * @return : a Wordlist Linked List with the mean frequencies of each word in the combined lists
 */
struct stringNode* getMeanFreq(struct WFD_Node* head1, struct WFD_Node* head2){

    struct stringNode* ptr1 = head1->wordList;
    struct stringNode* ptr2 = head2->wordList;

    struct stringNode* mergedHead = 0;
    struct stringNode* mergedTail = mergedHead;

    while(ptr1 != 0 && ptr2 != 0){
        struct stringNode* insert = 0;
        double meanFreq;

        if(strcmp(ptr1->word, ptr2->word) == 0){
            //same string, increment both pointers
            insert = newStringNode(ptr1->word, "meanFreq");

            meanFreq = (ptr1->frequency + ptr2->frequency) / 2;

            if(DEBUG){
                printf("Insert1: %s\n", ptr1->word);
                printf("Frequencies: %lf %lf\n",
                        ptr1->frequency, ptr2->frequency);
                printf("meanFreq: %lf\n\n", meanFreq);
            }
            insert->frequency = meanFreq;
            insert->occurrences = 0;
            //set occurrences to 0 to avoid confusion between actual WFD and
            //meanFreq

            ptr1 = ptr1->next;
            ptr2 = ptr2->next;
        }
        else if(strcmp(ptr1->word, ptr2->word) > 0){
            //ptr2->word is before ptr1->word
            //add ptr2->word to LL, increment ptr2


            insert = newStringNode(ptr2->word, "meanFreq");
            meanFreq = ptr2->frequency / 2;

            if(DEBUG){
                printf("Insert2: %s\n", ptr2->word);
                printf("Frequencies: 0 %lf\n", ptr2->frequency);
                printf("meanFreq: %lf\n\n", meanFreq);
            }
            insert->frequency = meanFreq;
            insert->occurrences = 0;
            ptr2 = ptr2->next;
        }
        else if(strcmp(ptr1->word, ptr2->word) < 0){
            insert = newStringNode(ptr1->word, "meanFreq");
            meanFreq = ptr1->frequency / 2;
            if(DEBUG){
                printf("Insert3: %s\n", ptr1->word);
                printf("Frequencies: %lf 0\n", ptr1->frequency);
                printf("meanFreq: %lf\n\n", meanFreq);
            }
            insert->frequency = meanFreq;
            insert->occurrences = 0;
            ptr1 = ptr1->next;
        }
        if(mergedHead != 0){
            mergedTail->next = insert;
            insert->prev = mergedTail;
            mergedTail = insert;
        }
        else{
            mergedHead = insert;
            mergedTail = mergedHead;
        }
    }

    if(ptr1 == 0){
        while(ptr2 != 0){
            struct stringNode* insert = newStringNode(ptr2->word, "meanFreq");
            double meanFreq = ptr2->frequency / 2;
            insert->frequency = meanFreq;
            insert->occurrences = 0;

            ptr2 = ptr2->next;
            mergedTail->next = insert;
            insert->prev = mergedTail;
            mergedTail = insert;
        }
    }
    else if(ptr2 == 0){
        while(ptr1 != 0){
            struct stringNode* insert = newStringNode(ptr1->word, "meanFreq");
            double meanFreq = ptr1->frequency / 2;
            insert->frequency = meanFreq;
            insert->occurrences = 0;

            ptr1 = ptr1->next;
            mergedTail->next = insert;
            insert->prev = mergedTail;
            mergedTail = insert;
        }
    }
    return mergedHead;
}


/**
 * Calculates the KLD (Kullbeck-Leibler Divergence) between each file and the
 * mean
 * @param file  WFD_Node created after a file is parsed
 * @param meanFreq mean frequency LL of words between two files
 */
double getKLD(struct WFD_Node* file, struct stringNode* meanFreq){

    struct stringNode* WFD_ptr;
    struct stringNode* meanFreq_ptr;
    double KLD = 0;
    //iterate through both lists until you find the matching word
    //then get frequencies and perform operations
    for(WFD_ptr = file->wordList; WFD_ptr != 0;
             WFD_ptr = WFD_ptr->next)
    {
        for(meanFreq_ptr = meanFreq; meanFreq_ptr != 0;
            meanFreq_ptr = meanFreq_ptr->next)
        {
            if(strcmp(WFD_ptr->word, meanFreq_ptr->word) == 0){

                double divisor = (WFD_ptr->frequency / meanFreq_ptr->frequency);


                KLD += (WFD_ptr->frequency * log2(divisor));
                break;
            }
        }
    }
    return KLD;
}


/**
 * Calculates JSD (Jensen-Shannon distance) betweeen files
 * @param 2 WFD Nodes
 * @return JSD as a double
 */
double getJSD(struct WFD_Node* firstNode, struct WFD_Node* secondNode){
    double JSD = 0;

    struct stringNode* meanFreq = getMeanFreq(firstNode, secondNode);

    double firstNode_KLD = getKLD(firstNode, meanFreq);
    double secondNode_KLD = getKLD(secondNode, meanFreq);

    JSD = sqrt((0.5 * firstNode_KLD) + (0.5 * secondNode_KLD));

    freeStringNodes(meanFreq);
    return JSD;
}


/**
 * Goes through file queue, creates WFD for the files and inserts WFD into linked list
 * @param : none
 */
void *fileThreadFunct(void *A){
        struct node* file;
        struct stringNode* head = 0;
        while((file = deQueue(fileQueue)) != NULL){
                if(fileQueue->active == 0){
                        break;
                }
                numFiles--;

                head = getWFD(head, file->fileName);
                pthread_mutex_lock(&fileQueue->lock);
                WFD_List = insert_WFD_Node(head, WFD_List);
                pthread_mutex_unlock(&fileQueue->lock);
        }
        fileDone = 1;
        pthread_exit(0);
}


/**
 * Analyzes the WFD_List given and calculates JSD for each comparison
 * @param analyzeArgs
 */
void *analyzeThreadFunc(void *A){
    	struct Aargs* args = (struct Aargs*)A;
	struct WFD_Node **WFD_Array = args->WFD_Array;
	struct pair *compareArr = args->compareArr;
	double *results = args->results;
	int start = args->start;
	int comparisons = args->comparisons;
    	for(int i = start; i < comparisons; i++){
    		if(compareArr[i].done == 1){
    			break;
    		}
    		results[i] = getJSD(WFD_Array[compareArr[i].first], WFD_Array[compareArr[i].second]);
    		compareArr[i].done = 1;
    	}
    	pthread_exit(0);
}
