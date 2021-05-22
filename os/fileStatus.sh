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

echo "$fileStatus"

fileSize=$(ls -lh "$filename" | awk '{printf("fileSize = %s", $5)}')

echo "$fileSize"

echo "$totalCount"
echo "$changeCount"

while [ $totalCount -lt 10 -o $changeCount -lt 2 ]
do
    # sleep 5
    echo "Test file status..."
    newFileSize=$(ls -lh "$filename" | awk '{printf("fileSize = %s", $5)}')
    if [ $newFileSize -ne $fileSize ] 
    then
        $fileSize=$newFileSize
        echo "file [$filename] size changed to [$filesize]"
        temp1=$changeCount
        changeCount=$( $temp1 + 1)
        totalCount=0
    else
        temp2=$totalCount
        totalCount=$($temp2 +1)
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


    
