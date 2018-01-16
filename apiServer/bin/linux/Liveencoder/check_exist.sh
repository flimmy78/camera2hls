#!/bin/bash

echo "[`date +'%Y-%m-%d %T'`] check_restart begin"

	process_num=`ps aux |grep bin|grep liveencoder|grep -v grep|grep -v data|wc -l`
	echo $process_num
	if [ $process_num -lt 1 ]
	then
		export LD_LIBRARY_PATH=./lib:./ffmpeg:$LD_LIBRARY_PATH:
		./bin/liveencoder &
		echo "[`date +'%Y-%m-%d %T'`] liveencoder restart!"
	fi
echo "[`date +'%Y-%m-%d %T'`] check_restart end"
