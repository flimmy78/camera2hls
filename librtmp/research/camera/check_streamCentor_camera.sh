#!/bin/sh

INTERVAL=5

while true

do

COUNT=`ps -ef |grep "camera2hls"| grep -v "grep"|grep -v "check"|grep -c "camera2hls"`


if [ $COUNT -lt 1 ]; then

echo "[`date +'%Y-%m-%d %T'`] process number:$COUNT, fork it!"

export LD_LIBRARY_PATH=~/camera2hls/librtmp/research/camera
nohup ./camera2hls www.hslink.top 9500 test rtmp://127.0.0.1/live/ct > /dev/null &


else

echo "[`date +'%Y-%m-%d %T'`] process number is normal:$COUNT" >> /dev/null

fi

sleep $INTERVAL

done