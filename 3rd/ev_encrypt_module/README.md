# 模型加密工具
## 文件结构
1. `bin/encrypt_tool`，加密模型的可执行文件
2. `lib/libev_encrypt_module.so`，加解密依赖的库文件
3. `include/encrypt_wrapper.hpp`，解密模型接口文件

## 使用方法
1. 使用`encrypt_tool`将模型加密，并生成`C++`头文件
    ```shell
    ./encrypt_tool -i model.cfg -o model_str.hpp
    ```
   完成后，模型文件将被编码成字符串，放到`model_str.hpp`中
2. 将头文件移动到自己的代码区
    ```shell
   mv model_str.hpp /usr/local/ev_sdk/src/ 
   ``` 
3. 在`ji.cpp`的实现中，获取解密后的模型：
    ```shel
   // 使用加密后的模型配置文件
   void *h = CreateDecryptor(model_str.c_str(), model_str.size(), key.c_str());
   // 获取解密后的字符串
   int fileLen = 0;
   void *decryptedModel = (FetchBuffer(h, fileLen); 
   ```
4. 编译`libji.so`，**请将加密模块的头文件和库文件加入对对应位置**。

## 注意：最终交付`libji.so`时，请务必删除`model_str.hpp`文件