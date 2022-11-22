#!/bin/bash
echo "------------------------------------"
echo "Testing Answers For Correctness"
echo "------------------------------------"

#input directory
testDir=$1
answerDir=$2
# get full path of current working directory using pwd
cpath=$(pwd)
# get full path to inserted directory
fullpathTest=$cpath$"/"$testDir
fullpathAnswer=$cpath$"/"$answerDir
# check the directory
if [ -d $testDir ] && [ -d $answerDir ]
then
# loop files
# ls fullpath gets all the files of the directory
# declare an array for the files
# -A for AssociativeArray and -a for NumericArray
declare -a test
declare -a answer
# @ is all

index=0
for file in $(ls $fullpathTest)
do
	test[index]=$fullpathTest$"/"$file
	((index=index+1))
done

index=0
for file in $(ls $fullpathAnswer)
do
	answer[index]=$fullpathAnswer$"/"$file
	((index=index+1))
done

# get length of array
len=${#test[@]}

for (( i=0; i<len; i++))
do
	testLine=$(cat ${test[i]})
	echo "${answer[i]}"
	for line in $testLine
	do
		if fgrep -q $line ${answer[i]}
		then
			continue
		else
			echo "Incorrect $line"
		fi
	done

done
# stop the for loop
else
	echo "$dir is not a directory."
# close if loop
fi

