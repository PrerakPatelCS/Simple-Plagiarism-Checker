#include "wordFrequencyDistribution.h"
#include "files.h"
#include "analysis.h"


struct WFD_Node* WFD_List = 0;
struct queue* dirQueue;
struct queue* fileQueue;
int fileDone = 0;
int numFiles = 0;
int fileThreads = 1;
int dirThreads = 1;
int analysisThreads = 1;
char *suffix = ".txt";

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

/**
 * Checks the flags from input to set the number of 
 * FileThreads, DirectoryThreads, AnalysisThreads, and the Suffix
 * @param : flags from input
 */
void optionalArguement(char *arguement){
	arguement++; // get rid of first character which is '-'
	char letter = ' ';
	int numberThreads = 1;
	if(strlen(arguement) >= 2){
		letter = arguement[0];
		arguement++; // get rid of letter only a number should be left
		if(checkIfValidNumber(arguement)){
			numberThreads = atoi(arguement);
			if(numberThreads < 0){
				numberThreads = 1;
			}
		}
	}
	if(letter == 'f'){
		fileThreads = numberThreads;
	}
	else if(letter == 'd'){
		dirThreads = numberThreads;
	}
	else if(letter == 'a'){
		analysisThreads = numberThreads;
	}
	else if(letter == 's'){
		suffix = arguement;
	}
}


int main(int argc, char *argv[]) {
	if(argc == 1){
		printf("Invalid Number of Arguements\n");
		return EXIT_SUCCESS;
	}
        dirQueue = createQueue();
        fileQueue = createQueue();
        pthread_t *dir_tids;
        pthread_t *file_tids;
	pthread_t *analysis_tids;
        char *fileName;
        for(int i = 1; i < argc; i++){
                if(argv[i][0] == '-'){
                	optionalArguement(argv[i]);
                }
                else{
                        struct stat data;
                        fileName = argv[i];
                        fileName = realpath(fileName, NULL);
                        if(stat(fileName, &data)){
                        	printf("%s NOT A FILE", argv[i]);
				return EXIT_SUCCESS;
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
                                	numFiles++;
                                        enQueue(fileQueue, fileName);
                                }
                        }
                        else if (S_ISDIR(data.st_mode)){
                                enQueue(dirQueue, fileName);
                        }
                        free(fileName);
                }
        }

	// Feed Directory Threads into its function
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
        if(numFiles < 2){
        	free(dir_tids);
        	free(Dargs);
        	freeQueue(deQueue(fileQueue));
        	printf("Invalid number of files\n");
		return EXIT_SUCCESS;
        }
        
	struct node* fileQ_head = fileQueue->front; //used to free queue later
        // Feed File Threads into its function
        file_tids = malloc(fileThreads * sizeof(pthread_t));
        fileQueue->active = fileThreads;
        for (int i = 0; i < fileThreads; i++){
                err = pthread_create(&file_tids[i], NULL, fileThreadFunct, NULL);
                if(err != 0){
                        perror("pthread_create");
                }
        }
        for(int i = 0; i < fileThreads; i++){
                pthread_join(file_tids[i], NULL);
        }

	// Feed Analysis Threads into its function
        analysis_tids = malloc((analysisThreads) * sizeof(pthread_t));
        struct Aargs* Aargs = malloc(sizeof(struct Aargs));
       	int num_files = get_WFD_Length(WFD_List);
    	int comparisons = (num_files * (num_files - 1)) / 2;
    	struct pair *compareArr = malloc(comparisons * sizeof(struct pair));
    	double *results = (double *)malloc(comparisons * sizeof(double));
    	int counter = 0;
    	for(int i = 0; i < num_files; i++){
    		for(int j = i + 1; j < num_files; j++){
    			results[counter] = 0.0;
    	    		compareArr[counter].first = i;
	    		compareArr[counter].second = j;
    	    		counter++;
    		}
    	}
        
        struct WFD_Node** WFD_Array = malloc(num_files * sizeof(struct WFD_Node));
	struct WFD_Node* temp = WFD_List;
        int index = 0;
        for(struct WFD_Node* ptr = temp; ptr != 0; ptr = ptr->next){
        	WFD_Array[index] = ptr;
        	index++;
        }
        
	int start = 0;
        int increment = comparisons/analysisThreads;
        if(increment == 0){
        	increment = 1;
        	analysisThreads = comparisons;
        }
	/**
	 * compareArr helps with how to compare every file with each other
	 * results holds all the JSD's from those comparisons
	 * WFD_Array is an array of WFD_Nodes and with compareArr
	 * I can use first and second's indices to compare really quickly
	 */

        for (int i = 0; i < analysisThreads; i++) {
        	start = increment * i;
                Aargs->WFD_Array = WFD_Array;
                Aargs->compareArr = compareArr;
		Aargs->results = results;
		Aargs->start = start;
		Aargs->comparisons = comparisons;
                err = pthread_create(&analysis_tids[i], NULL, analyzeThreadFunc, Aargs);
                if(err != 0){
                        perror("pthread_create");
                }
		usleep(10);
        }

        for(int i = 0; i < analysisThreads; i++){
            pthread_join(analysis_tids[i], NULL);
        }
        char* red = "\033[1;31m";
        char* yellow = "\033[1;33m";
        char* green = "\033[1;32m";
        char* cyan = "\033[1;36m";
        char* blue = "\033[1;34m";
        char* white = "\033[1;37m";
        char* reset = "\033[0m";
        char* color;
        
        for(int i = 0; i < comparisons; i++){
        	if(results[i] < .1){
        		color = red;
        	}
        	else if(results[i] >= .1 && results[i] < .15){
        		color = yellow;
        	}
		else if(results[i] >= .15 && results[i] < .2){
        		color = green;
        	}
		else if(results[i] >= .2 && results[i] < .25){
        		color = cyan;
        	}
		else if(results[i] >= .25 && results[i] < .3){
        		color = blue;
        	}
		else if(results[i] >= .3){
        		color = white;
        	}
        	
                printf("%s%lf %s%s %s\n", color, results[i], reset,  WFD_Array[compareArr[i].first]->filename, WFD_Array[compareArr[i].second]->filename);
                
        }
        
        if(DEBUG){
            printf("------------------------------------------------------\n");
            printf("Files = %d\nDirectories = %d\nAnalysis = %d\nSuffix = %s\n", fileThreads, dirThreads, analysisThreads, suffix);
            printf("num_files = %d\nComparisons = %d\nWFD_Length = %d\n", num_files, comparisons, get_WFD_Length(WFD_List));

        }

        free(file_tids);
        free(dir_tids);
        free(analysis_tids);
        free(Dargs);
        free(Aargs);
        free(results);
        free(compareArr);
	free_WFD_Nodes(WFD_List);
        free(WFD_Array);
	freeQueue(fileQ_head);
        return EXIT_SUCCESS;
}
