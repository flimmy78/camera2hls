#!/usr/bin/python
import os
import time
import subprocess
import sys

#print sys.argv[1]

inputfile=sys.argv[1]
filename=sys.argv[2]
cmd='ffmpeg -i '+inputfile+' -vcodec copy -an -f hls -hls_playlist_type vod -hls_time 60 '+filename+'.m3u8 2>/dev/null'
os.system(cmd)
os.system('mv *.ts *.m3u8 objs/nginx/html/live/ -f')
