| **`Linux`**                               |
| ----------------------------------------- |
| ![Build Status](./img/build_and_pass.svg) |



#### 说明

本项目为开发者将视觉算法上传到[极市平台](http://cvmart.net/)进行测试，宣传，销售等的帮助文档，如有问题，可以联系极市小助手（微信：Extreme-Vision)  




## 1.算法提交流程

1. （假设算法已经开发完成）


2. 如开发语言为C/C++，需要选择对应的接口文件，并需要并构造函数实现算法,含图片和视频接口，并完善算法SDK所需要的所有文件。

   * （必须）**[SDK说明文档参见这里](./doc/极市算法SDK说明文档V1.0(C&C++)_180911.md)**  




  ### 相关文件

   * 接口规范ji.h及接口实现文件ji.cpp：
        * （必须）[ji.h](./ji.h)
        * （必须）[ji.cpp](./ji.cpp) 



   * 服装图片风格识别算法ji.cpp实现样例及调用demo

       * [ji-image.cpp](./ji-image.cpp)
       * [main-image.cpp](./main-image.cpp)



   * 视频监控人员是否在岗算法ji.cpp实现样例及调用demo  

      * [ji-video.cpp](./ji-video.cpp)

      * [main-video.cpp](./main-video.cpp)



  * 非必须

       * [ji_util.hpp](./ji_util.hpp)  


    
  3. 如开发语言为Python，按照下面接口进行实现。

     * （必须）[ji.py](./ji.py)
     * SDK正在更新中  



## 2.相关文档

1. 示例代码
  * [C/C++代码示例](./sample_c)
    * [如何把图片文件转换为buffer输入并做测试](./sample_c/standard_sample(convert_file_to_buffur)/)
    * [如何从buffer转换到opencv的Mat格式](./sample_c/convert_buffer_to_opencv_mat/)
    * [如何把算法代码编译位动态库](./sample_c/gcc-so.sh)
    * JSON输出格式建议使用[cJSON](https://github.com/DaveGamble/cJSON)库标准输出，输出格式验证可到[bejson](https://www.bejson.com/)进行验证
    * 持续更新...
  * [Python代码示例](./sample_py)
    * 持续更新  



## 3.上传测试

成功上传算法后，极市会进行算法测试审核，并且将测试结果反馈给开发者。



## 4.技术支持

需要任何技术支持，请加小助手微信（Extreme-Vision）或者 QQ 3053890056。如有任何问题，欢迎大家提交issue.
