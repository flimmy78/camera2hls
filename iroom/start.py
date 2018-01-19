#!/usr/bin/python
import os
import time
import subprocess
import sys

#print sys.argv[1]

ip=sys.argv[1]
port=sys.argv[2]
user=sys.argv[3]
ttl=int(sys.argv[4])
print ip
print port
print user
print ttl
cmd='ps -ef|grep '+sys.argv[1]+'|grep -v grep|grep -v start.py'
cmd2='./camera2hls '+ip+' '+port+' '+user+' rtmp://127.0.0.1/live/'+ip
print cmd2
res1 = subprocess.Popen(cmd,stdout=subprocess.PIPE,shell=True)
attn1=res1.stdout.readlines()
if len(attn1) > 0:
        sys.exit()

while ttl > 0:
        res = subprocess.Popen(cmd,stdout=subprocess.PIPE,shell=True)
        attn=res.stdout.readlines()
        counts=len(attn)
        if counts < 1:
                print  "start ffmpeg"
                subprocess.Popen(cmd2,stdout=subprocess.PIPE,shell=True)
        ttl= ttl-1
        time.sleep(1)
os.system('ps aux|grep '+ip+'|grep -v grep|grep -v start.py|awk \'{print $2}\'|xargs kill -9')
