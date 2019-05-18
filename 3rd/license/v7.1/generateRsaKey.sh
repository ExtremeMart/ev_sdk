#! /bin/bash
openssl genrsa -out privateKey.pem 1024 &&
openssl rsa -in privateKey.pem -pubout -out pubKey.pem
