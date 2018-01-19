#!/usr/bin/env python
import os
from flask import Flask
from flask import request
import platform
app = Flask(__name__)

@app.route('/api/stoplivestream', methods=["GET", 'POST'])
def stoplivestreamHandler():
    if platform.system() == "Linux":
        os.system('./stop.py ' + request.args.get("cameraip"))
    else:
        print request.args
    return "OK"

@app.route('/api/getlivestream', methods=["GET", 'POST'])
def getlivestreamHandler():
    if platform.system() == "Linux":
        os.system('./start.py ' + request.args.get("cameraip") + ' ' + request.args.get("cameraport") + ' ' + request.args.get(
            "username") + ' ' + request.args.get("livelimit") + ' &')
    else:
        print request.args
    return "{\"Url\":\"http://106.14.62.202/live/" + request.args.get("cameraip") + ".m3u8\"}"

@app.route('/api/getvodstream', methods=["GET", 'POST'])
def getvodstreamHandler():
    if platform.system() == "Linux":
        #find the input H.264 file
        #os.path.exists(request.args.get("filepath"))
        os.system('./vod.py ' + os.path.basename(request.args.get("filepath")))
    else:
        print request.args
    return "{\"Url\":\"http://106.14.62.202/live/" + os.path.basename(request.args.get("filepath")) + ".m3u8\"}"

@app.route('/')
def hello_world():
    return 'Hello Flask!'


@app.route('/favicon.ico')
def favicon():
    return 'Favicon'


if __name__ == '__main__':
    print 'Begin...'
    app.run(host='0.0.0.0', port=808)
