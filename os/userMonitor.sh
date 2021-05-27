#! /bin/bash

# 判断用户是否在命令行输入了参数
if [[ $# -lt 1 ]]
then    
    echo "Usage: userMonitor username"
    exit
fi

# 标志是否检测到用户登录的标志位
flag=0

# 标志位不为1 说明未检测到指定用户，循环检测
while [[ $flag -ne 1 ]]
do

    # 通过who命令获取当前登录的用户，存入数组中
    userArray=()
    i=0
    count=1
    echo ""
    until [[ "y$(who | awk 'NR=='$count'{print $1}')" == "y" ]] 
    do  
        userArray[$i]=$(who | awk 'NR=='$count'{print $1}')
        echo "userArray:${userArray[$i]}"
        i=`expr $i+1`
        count=`expr $count + 1`
    done

    # 遍历用户数组，与指定用户名比较
    j=0
    while [[ -n "${userArray[$j]}" ]]
    do
        if [ "y$1" == "y${userArray[$j]}" ]
        then
            echo "user [$1] is logon"
            flag=`expr $flag + 1`
            exit
        fi
        j=`expr $j + 1`
    done

    # 如果未检测到指定用户登录则等待5秒再继续检测
    if [[ $flag -ne 1 ]] 
    then 
        echo -e "waiting user [$1]...\c"
        sleep 5      
    fi

done
