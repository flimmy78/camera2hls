#!/usr/bin/python
import os
import time
import subprocess
import sys

#print sys.argv[1]

ip=sys.argv[1]
os.system('ps aux|grep '+ip+'|grep -v grep|awk \'{print $2}\'|xargs kill -9')
