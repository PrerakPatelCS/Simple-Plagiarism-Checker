# Simple Plagiarism Checker
## Inputs
Takes in files driectories and flags
```
-f# -d# -a# -sString file directory
-f sets the number of File Threads. Default 1
-d sets the number of Directory Threads. Default 1
-a sets the number of Analysis Threads. Default 1
-s sets the suffix of what files you want to compare. Default .txt
```
## Directory Threads
These threads work on searching through the directories and any subdirectories by inserting all directories into a directory queue. Going through the directories, any files with the correct suffix will be entered into a file queue.

## File Threads
Works on the shared file queue by dequeueing and creating a word frequency distribution tokenizing the file by white space and then dividing each word by the total amount of words in the file. These words are stored in an alphabetically sorted Linked List. The file parsing will ignore all punctuation and convert all letters to lower case. Each File will have it's own WFD.

## Analysis Threads
The WFD's are what will be compared. 
1. The mean distribution of the two WFD's is computed by going through each word and finding the mean occurence for that word in the two files.
```
MeanProb(wordX)=(firstWFD(word(X)) + secondWFD(word(X)))/2
```
1. The Kullbeck-Leibler Divergence is computed by, for each word in the WFD that is also in the mean, multiple the discrete probability of its occurence in the list by the logarithm of its probability in the WFD divided by the probability computed for the mean of that word.
1. Compute the Jensen-Shannon Distance which is computed by finding the mean of the two Kullbeck-Leibler Divergences. 
The JSD will return a number from [0, 1) where the JSD is 0 if the files are the same, generally the lower the JSD the more likely plagiarism has occured. 

The JSD will show as different colors for different ranges.
```
red for [0, 0.1)
yellow for [0.1, 0.15)
green for [0.15, 0.2)
cyan for [0.2, 0.25)
blue for [0.25, 0.3)
and white for (.3, 1)
```

## Files

#### Compare
compare.c is used as the controller file and connects all the WFD, files, and analysis defeinitions together to create the simple plagiarism checker. The global variables are initialized in here and all the input goes through here first. 
> make testCompare
Runs the test cases and Pipes them into the testCompare directory.

#### Word Frequency Distribution
wordFrequencyDistribution.c is used to create the WFD and the wordFrequencyDistribution.h file is used to hold all of the definitions and struct data. The testWFD.c file is used to test out WFD and from the make file, using 
> make testCasesWFD
Runs the test cases and Pipes them into the testWFD directory.

#### Files
files.c is used to create the file queues and is where the directory thread function is in.The files.h file holds all of the definitions and struct data. The testFiles.c file is used to test out the output of files.c and from the make file, using 
> make testCasesFiles
Runs the test cases and Pipes them into the testFiles directory. 

#### Analysis
analysis.c is used to do all the math to compute the Jensen Shannon Distance. The file and analysis thread functions are in here and the analysis.h file holds all the definitons and struct data needed.

#### Bash CheckAnswers
This works by having two inputs, an answers directory and a test cases output directory and both directories are equal size. The Bash File compares each line of an answers file to a testcases file to check if it is in there. If it is not then it will print that line.

