#!/bin/bash

# 一键生成公私钥以及公钥对应的代码头文件

NORMAL_TXT_COLOR="\e[92m"
ERROR_TXT_COLOR="\e[31m"
END_TAG="\e[0m"

# 公钥与私钥的存储路径
auth_key_path=/usr/local/ev_sdk/authorization
public_key=${auth_key_path}/pubKey.pem
private_key=${auth_key_path}/privateKey.pem
# 公钥对应的头文件位置
public_key_header=/usr/local/ev_sdk/include/pubKey.hpp

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
echo -e "${NORMAL_TXT_COLOR}Using ${ev_license_path} and ${ev_codec_path} to generate license.${END_TAG}"

# 生成公私钥
echo -e "${NORMAL_TXT_COLOR}Generating public key and private key...${END_TAG}"
if [[ ! -f "${public_key}" || ! -f "${private_key}" ]]; then
    openssl genrsa -out ${private_key} 1024 && openssl rsa -in ${private_key} -pubout -out ${public_key};
    echo -e "${NORMAL_TXT_COLOR}Key pair generated: ${public_key}, ${private_key}${END_TAG}"
else
  # 判断是否覆盖已有的公私钥
    while true; do
    read -r -p "${public_key} or ${private_key} exist, override? Yes/[No] " yn
    case $yn in
        [Yy]* ) openssl genrsa -out ${private_key} 1024 && openssl rsa -in ${private_key} -pubout -out ${public_key} &&
                echo -e "${NORMAL_TXT_COLOR}Key pair generated: ${public_key}, ${private_key}${END_TAG}";
                break;;
        [Nn]* ) break;;
        "" )    break;;
    esac
    done
fi

# 生成公钥对应的头文件
echo -e "${NORMAL_TXT_COLOR}Generate header file for public key...${END_TAG}"
if [[ -f "${public_key_header}" ]]; then
    rm ${public_key_header}
fi
${ev_codec_path} -c ${public_key} ${public_key_header}
sed -i 's|key|pubKey|' ${public_key_header}
echo -e "${NORMAL_TXT_COLOR}Header file saved to ${public_key_header}${END_TAG}"

echo "Done."