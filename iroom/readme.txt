#install python 2.7.12

wget https://www.python.org/ftp/python/2.7.12/Python-2.7.12.tar.xz
tar xvf ./Python-2.7.12.tar.xz
./configure 
make all  
make install
make clean
make distclean
/usr/local/bin/python2.7 -V
mv /usr/bin/python /usr/bin/python2.6.6
ln -s /usr/local/bin/python2.7 /usr/bin/python

#然后编辑/usr/bin/yum，将第一行的#!/usr/bin/python修改成#!/usr/bin/python2.6.6 (不然执行yum 会出问题)

#upgrade pip
wget https://bootstrap.pypa.io/get-pip.py
rm -rf /usr/bin/pip
ln -s /usr/local/bin/pip2.7 /usr/bin/pip

#install flask
pip install Flask








