//
// Created by hrh on 2019/11/7.
//

#ifndef JI_LICENSE_JI_LICENSE_UTILS_HPP
#define JI_LICENSE_JI_LICENSE_UTILS_HPP

#ifdef __cplusplus
extern "C" {
#endif

#include <openssl/ossl_typ.h>
#include <stdio.h>
#include <stdbool.h>

/**
 * 从字符串key创建RSA公钥
 *
 * @param key
 * @return RSA公钥的指针
 */
RSA *createPublicRSA(const char *key);

/**
 * 从字符串key创建RSA私钥
 *
 * @param key
 * @return RSA私钥的指针
 */
RSA *createPrivateRSA(const char *key);

/**
 * 获取文件ifs的大小
 *
 * @param ifs 打开的输入流
 * @return ifs的大小
 */
size_t getFileSize(FILE *f);

/**
 * 使用公钥publickKey和签名signature验证文件ifs是否被篡改，计算signature之前使用的哈希算法必须是SHA256
 *
 * @param publicKey 公钥
 * @param f 需要验证的文件
 * @param signature 签名，使用SHA256对数据做哈希计算之后的签名
 * @param sigLen 签名的长度
 * @return 如果验证通过返回true，否则返回false
 */
bool verifyFileSignature(RSA *publicKey, FILE *f, const unsigned char *signature, unsigned int sigLen);

#ifdef __cplusplus
};
#endif

#endif //JI_LICENSE_JI_LICENSE_UTILS_HPP
