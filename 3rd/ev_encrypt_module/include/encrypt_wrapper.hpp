#ifndef ENCRYPT_WRAPPER_H_
#define ENCRYPT_WRAPPER_H_

/**
 * 创建一个数据解密器
 * @param[in] data 需要解密密的数据
 * @param[in] len 所加密数据的长度
 * @param[in] key 加密原始数据所使用的随机密钥
 * @return 解密器句柄
 */
void* CreateDecryptor(const char* data, int len, const char* key);

/**
 * 从数据解密器获取解密后的数据
 * @param[in] handle 解密器句柄
 * @param[out] bufferLen 解密后的数据长度
 * @return 解密后的数据指针
 */
void* FetchBuffer(void* handle, int &bufferLen);

/**
 * 从数据解密器获取解密后的数据文件句柄
 * @param[in] handle 解密器句柄
 * @return 解密后的文件句柄
 */
void* FetchFile(void* handle);

/**
 * 释放解密器
 * @param handle 需要释放的解密器句柄
 */
void DestroyDecrtptor(void *handle);

#endif  // ENCRYPT_WRAPPER_H_