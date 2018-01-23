rm -rf ./camera2hls
cp ../librtmp/research/camera/camera2hls ./
nohup gunicorn -w 4 -b 0.0.0.0:808 iroom:app >log &
