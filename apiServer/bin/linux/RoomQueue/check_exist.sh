#!/bin/bash

echo "[`date +'%Y-%m-%d %T'`] check_restart begin"

        process_num=`ps aux |grep bin|grep roomQueue |grep -v grep | grep -v data|wc -l`
	echo $process_num
        if [ $process_num -lt 1 ]
       then
		ulimit -c unlimited
		export LD_LIBRARY_PATH=./lib:$LD_LIBRARY_PATH
		./bin/roomQueue &
                echo "[`date +'%Y-%m-%d %T'`] roomQueue restart!"
        fi
echo "[`date +'%Y-%m-%d %T'`] check_restart end"
