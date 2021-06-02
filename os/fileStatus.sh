#! /bin/bash

#清屏
clear

#请用户输入需要检测的文件名，并读取
echo "Input file name: "
read filename

#设置表示程序执行次数的变量
totalCount=0
changeCount=0

# 检测用户是否输入文件名
if [ ! -f $filename ]
then 
    echo "Can't find the file [$filename]!"
    exit
fi

#获取该文件的当前权限信息
fileStatus=$(ls -l "$filename" | awk '{printf("%s", $1)}')
echo "fileStatus: $fileStatus"

#获取该文件的当前大小信息
fileSize=$(ls -l "$filename" | awk '{printf("%s", $5)}')
echo "filesize: $fileSize"

#循环检测文件的大小，直到没有任何改变的检测了10次或者检测到文件大小改变了两次
while [[ $totalCount -lt 10 &&  $changeCount -lt 2 ]]
do
    #每一次检测之间间隔五秒
    sleep 5
    
    #检测文件状态
    echo "Test file status..."
    newFileSize=$(ls -l "$filename" | awk '{printf("%s", $5)}')
    if [[ $newFileSize -ne  $fileSize ]]
    then #如果文件大小较之前改变了
        fileSize=$newFileSize
        echo "file [$filename] size changed to [ $fileSize ]"
        changeCount=`expr $changeCount + 1`
        totalCount=0
    else #如果文件大小较之前没有改变
        totalCount=`expr $totalCount + 1`
    fi
done

#如果是因为连续10次检测到文件大小没有改变
if [ $totalCount -eq 10 ]
then    
    echo "test number exceed ten, test end."
fi

#如果是因为检测到文件大小改变了两次
if [ $changeCount -eq 2 ]
then   
    echo "change number exceed two, test end."
fi


    
