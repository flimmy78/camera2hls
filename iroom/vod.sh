#!/bin/sh

echo $1
echo $2

cd /root/zhiguangq/camera2hls/iroom/objs/nginx/html/live
if [ ! -f "$2.m3u8" ]; then
	ffmpeg  -i $1 -vcodec libx264 -vprofile baseline -g 25 -s 704x576 -an -f hls -hls_list_size 0 -hls_time 2 -y -threads 4 $2.m3u8 2>/dev/null &
fi

