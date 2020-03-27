#! /bin/bash

#./generateRsaKey.sh
#ret=$?; if [[ 0 -ne ${ret} ]]; then echo "generateRsaKey failed!"; exit $ret; fi

./ev_license -r r.txt && ./ev_license -l privateKey.pem r.txt l.txt && ./ev_license -c  pubKey.pem l.txt
ret=$?; if [[ 0 -ne ${ret} ]]; then echo "ev_license failed!"; exit $ret; fi

#./clear.sh
exit 0
