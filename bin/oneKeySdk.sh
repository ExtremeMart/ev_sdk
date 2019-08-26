#! /bin/bash

#添加权限
chmod 775 *

check_error() {
        if [ $? -ne 0 ]
        then
                echo "Error:run [$1] failed!"
                exit 1
        fi
}

#添加依赖库
#apt-get install nvme-cli dmidecode sg3-utils

# 删除旧的license
#rm -f privateKey.pem pubKey.pem r.txt license.txt

#生成公钥私钥
: <<'END'
if [ ! -f ./privateKey.pem ]
then
    openssl genrsa -out privateKey.pem 1024
    check_error "openssl genrsa -out privateKey.pem 1024"

    openssl rsa -in privateKey.pem -pubout -out pubKey.pem
    check_error "openssl rsa -in privateKey.pem -pubout -out pubKey.pem"
else
   read  -p "检测到rsa密钥已经存在[privateKey.pem,publickey.pem],若更新算法时请勿重新生成密钥,否则已有授权将失效！确认要更新密钥吗? [Y/N]?" answer
   case $answer in
   Y | y)
      echo "updating rsa screct...";
      openssl genrsa -out privateKey.pem 1024
      check_error "openssl genrsa -out privateKey.pem 1024"

      openssl rsa -in privateKey.pem -pubout -out pubKey.pem
      check_error "openssl rsa -in privateKey.pem -pubout -out pubKey.pem"
   ;;
   N | n)
      echo "publickey is newest!"
   ;;
   *)
      echo "publickey is newest"
   ;;

   esac
fi
END

#生成公钥私钥
if [ ! -f ./privateKey.pem ]
then
    openssl genrsa -out privateKey.pem 1024
    check_error "openssl genrsa -out privateKey.pem 1024"

    openssl rsa -in privateKey.pem -pubout -out pubKey.pem
    check_error "openssl rsa -in privateKey.pem -pubout -out pubKey.pem"
else
   echo "检测到rsa密钥已经存在[privateKey.pem,publickey.pem],若想重新生成,请删除这两个文件,并重新运行该脚本!"
fi

#将公钥转换为c常量字符串
./ev_codec -c pubKey.pem ../src/pubKey.hpp && sed -i "s|key|pubKey|g" ../src/pubKey.hpp
check_error "./ev_codec -c pubKey.pem ../src/pubKey.hpp"

#生成摘要
./ev_license -r r.txt
check_error "./ev_license -r r.txt"

#生成license
./ev_license -l privateKey.pem r.txt license.txt
check_error "./ev_license -l privateKey.pem r.txt license.txt"

#进入src路径，编译源码
cd /usr/local/ev_sdk/src/
make clean
make -j
check_error "make -j"

#进入test路径，编译测试工具
cd /usr/local/ev_sdk/test/
make clean
make -j
check_error "make -j"

echo "finished."

