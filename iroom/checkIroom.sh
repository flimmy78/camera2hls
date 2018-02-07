#!/bin/sh
# * * * * * sh /root/zhiguangq/camera2hls/iroom/checkIroom.sh  >/dev/null 2>&1 &
checkProc(){
cd /root/zhiguangq/camera2hls/iroom
srsNum=`ps aux | grep objs/srs|grep -v grep |grep -v check| wc -l`
nginxNum=`ps aux | grep sbin/nginx|grep -v grep |grep -v check| wc -l`
iroomNum=`ps aux | grep unicorn|grep -v grep |grep -v check| wc -l`
#echo $srsNum
#echo $nginxNum
#echo $iroomNum
if [ $srsNum -lt 1 ];
then
	./objs/srs -c conf/srs.conf >/dev/null
fi

if [ $nginxNum -lt 1 ];
then
	./sbin/nginx -c ./conf/nginx.conf
fi

if [ $iroomNum -lt 1 ];
then
	nohup /usr/local/bin/python /usr/local/bin/gunicorn -w 3 -b 0.0.0.0:808 iroom:app >log &
fi


}

for ((i=1;i<=11;i++));
do
	checkProc
	sleep 5
done

