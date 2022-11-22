#include "wordFrequencyDistribution.h"
#include <sys/types.h>
#include <sys/stat.h>


int main(int argc, char *argv[]){
	struct stat data;
	char *suffix = ".txt";
	char *fileName;
	fileName = argv[1];
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
                	// is a file
			struct stringNode* head = 0;
			head = getWFD(head, fileName);
			printStringNodes(head);
			freeStringNodes(head);
                }
	}
	free(fileName);


	return EXIT_SUCCESS;
}
