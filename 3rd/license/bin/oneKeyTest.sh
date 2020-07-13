#! /bin/bash

# 测试公私钥是否匹配，以及是否可以正常生成授权文件

NORMAL_TXT_COLOR="\e[92m"
ERROR_TXT_COLOR="\e[31m"
END_TAG="\e[0m"

# 公钥与私钥的存储路径
auth_key_path=/usr/local/ev_sdk/authorization
default_public_key=${auth_key_path}/pubKey.pem
default_private_key=${auth_key_path}/privateKey.pem

# 授权文件生成程序和头文件编码程序路径
ev_license_path=/usr/local/ev_sdk/3rd/license/bin/ev_license
ev_codec_path=/usr/local/ev_sdk/3rd/license/bin/ev_codec

if [[ ! -d ${auth_key_path} ]]; then
    mkdir -p ${auth_key_path}
fi

if [[ ! -f ${ev_license_path} ]]; then
    echo "${ev_license_path} not found!"
    exit 1
elif [[ ! -x ${ev_license_path} ]]; then
    chmod +x ${ev_license_path}
fi

if [[ ! -f ${ev_codec_path} ]]; then
    echo "${ev_codec_path} not found!"
    exit 1
elif [[ ! -x ${ev_codec_path} ]]; then
    chmod +x ${ev_codec_path}
fi
echo -e "${NORMAL_TXT_COLOR}Using ${ev_license_path} and ${ev_codec_path}${END_TAG}"

# 测试公私钥是否匹配
if [[ ! -f ${default_public_key} || ! -f ${default_private_key} ]]; then
    echo -e "${ERROR_TXT_COLOR} ${default_public_key} or ${default_private_key} does not exist!${END_TAG}"
    exit 1
fi

echo -e "${NORMAL_TXT_COLOR}Checking if public key ${default_public_key} and priavte key ${default_private_key} match...${END_TAG}"
rm -rf /tmp/tmp_pubKey.pem
openssl rsa -in ${default_private_key} -pubout -out /tmp/tmp_pubKey.pem
diff -Z -B -s "/tmp/tmp_pubKey.pem" ${default_public_key} > /dev/null
ret=$?
if [[ 0 -ne ${ret} ]]; then
    echo -e "${ERROR_TXT_COLOR}Public key and private key does not match!${END_TAG}"
    echo -e "${NORMAL_TXT_COLOR}Failed.${END_TAG}"
    exit ${ret}
else
    echo -e "${ERROR_TXT_COLOR}Success.${END_TAG}"
fi

# 测试是否可以正常生成密钥
echo -e "${NORMAL_TXT_COLOR}Testing license generation..."
rm ${auth_key_path}/license.txt; rm ${auth_key_path}/reference.txt;
${ev_license_path} -r ${auth_key_path}/reference.txt && \
    ${ev_license_path} -l ${default_private_key} ${auth_key_path}/reference.txt ${auth_key_path}/license.txt && \
    ${ev_license_path} -c ${default_public_key} ${auth_key_path}/license.txt
ret=$?
if [[ 0 -ne ${ret} ]]; then
    echo -e "${ERROR_TXT_COLOR}Failed!${END_TAG}"
else
    echo -e "${NORMAL_TXT_COLOR}Success.${END_TAG}"
fi
exit ${ret}