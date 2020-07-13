#! /bin/bash

# 测试联网校验

#./generateRsaKey.sh
#ret=$?; if [[ 0 -ne ${ret} ]]; then echo "generateRsaKey failed!"; exit $ret; fi

./ev_license -rn r.txt && ./ev_license -lua privateKey.pem r.txt "http://192.168.1.175/api/encrypt/check-license" "0123456789" l.txt && ./ev_license -c  pubKey.pem l.txt
ret=$?; if [[ 0 -ne ${ret} ]]; then echo "ev_license failed!"; exit $ret; fi

#./clear.sh
exit 0
