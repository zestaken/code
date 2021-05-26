#! /bin/bash
clear

echo "Input file name: "
read filename
totalCount=0
changeCount=0

if [ ! -f $filename ]
then 
    echo "Can't find the file [$filename]!"
    exit
fi

fileStatus=$(ls -l "$filename" | awk '{printf("fileStatus = %s", $1)}')

echo "fileStatus: $fileStatus"

fileSize=$(ls -l "$filename" | awk '{printf("fileSize = %s", $5)}')

echo "totalCount: $totalCount"
echo "changeCount: $changeCount"

while [[ $totalCount -lt 10 &&  $changeCount -lt 2 ]]
do
     sleep 5
    echo "Test file status..."
    newFileSize=$(ls -l "$filename" | awk '{printf("fileSize = %s", $5)}')
    if [[ $newFileSize -ne  $fileSize ]]
    then
        fileSize=$newFileSize
        echo "file [$filename] size changed to [ $fileSize ]"
        changeCount=`expr $changeCount + 1`
        totalCount=0
    else
        totalCount=`expr $totalCount + 1`
    fi
done

if [ $totalCount -eq 10 ]
then    
    echo "test number exceed ten, test end."
fi

if [ $changeCount -eq 2 ]
then   
    echo "change number exceed two, test end."
fi


    
