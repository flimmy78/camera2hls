#!/bin/sh

echo $1
echo $2

cd /root/zhiguangq/camera2hls/iroom/objs/nginx/html/live
if [ ! -f "$2.m3u8" ]; then
	ffmpeg  -i $1 -vcodec copy -an -f hls -hls_playlist_type vod -hls_time 10 $2.m3u8 2>/dev/null &
fi

