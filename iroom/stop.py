#!/usr/bin/python
import os
import time
import subprocess
import sys


ip=sys.argv[1]
port=sys.argv[2]
user=sys.argv[3]
os.system('ps aux|grep '+ip+'|grep '+port+'|grep '+user+'|grep -v grep|awk \'{print $2}\'|xargs kill -9')
