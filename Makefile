all : compare

compare : compare.c analysis.c wordFrequencyDistribution.c files.c
	gcc -pthread  -g -Wall -fsanitize=address compare.c analysis.c wordFrequencyDistribution.c files.c -lm -o compare

clean:
	rm -f compare
	rm -f testFile
	rm -f testWFDs

testCompare :
	./compare testing/emptyDirectory testing/test1.txt testing/test2.txt testing2 > ./testCompareFile/testCasesForCompare/testCase1.txt
	./compare -a10 -f10 -d100 -s.md README.md README.md > ./testCompareFile/testCasesForCompare/testCase2.txt
	./compare -a0 -f0 -d0 testing > ./testCompareFile/testCasesForCompare/testCase3.txt
	./compare > ./testCompareFile/testCasesForCompare/testCase4.txt
	./compare hi there hello > ./testCompareFile/testCasesForCompare/testCase5.txt
	./compare testing/test1.txt > ./testCompareFile/testCasesForCompare/testCase6.txt
	
checkCompare :
	bash checkAnswer.sh testCompareFile/answersForCompare testCompareFile/testCasesForCompare

testFile : testFiles.c files.c
	gcc -pthread -g -Wall -fsanitize=address testFiles.c files.c -lm -o testFile

testCasesFiles : 
	./testFile testing/emptyDirectory > ./testFiles/testCasesForFiles/testCase1.txt
	./testFile testing > ./testFiles/testCasesForFiles/testCase2.txt
	./testFile -d11 testing testing2 > ./testFiles/testCasesForFiles/testCase3.txt
	./testFile -d2 testing2 > ./testFiles/testCasesForFiles/testCase4.txt

checkFiles :
	bash checkAnswer.sh testFiles/answersForFiles testFiles/testCasesForFiles

testWFD : testWFD.c wordFrequencyDistribution.c
	gcc -pthread -g -Wall -fsanitize=address testWFD.c wordFrequencyDistribution.c -lm -o testWFDs

testCasesWFD : 
	./testWFDs testWFD/testWFDFiles/alice29.txt > ./testWFD/testCasesForWFD/testCase1.txt
	./testWFDs testWFD/testWFDFiles/emptyFile.txt > ./testWFD/testCasesForWFD/testCase2.txt
	./testWFDs testWFD/testWFDFiles/test1.txt > ./testWFD/testCasesForWFD/testCase3.txt
	
checkWFD :
	bash checkAnswer.sh testWFD/answersForWFD testWFD/testCasesForWFD

#-std=gnu99
#
