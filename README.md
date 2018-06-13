
| **`Linux`** |
|-------------|
|![Build Status](./img/build_and_pass.svg)|

#### 说明
本项目为开发者将视觉算法上传到[极市平台](http://cvmart.net/)进行宣传销售等的帮助文档，如有问题，可以联系极市小助手（微信：Extreme-Vision）



## 1.算法提交流程

1. （假设算法已经开发完成）


2. 根据开发语言（C/C++/Python）选择对应的接口文件，并需要并构造函数实现算法,含图片和视频接口
   
   *C/C++
    * （必须）[ji.h](./ji.h)
   * （非必须）[ji_util.hpp](./ji_util.hpp)
		
  *Python
    * （必须）[ji.py](./ji.py)


3. 根据开发语言（C/C++/Python）的要求，完善开发者算法SDK所需要的所有文件。

* [C/C++参见这里](./doc/极市平台算法Docker镜像规范(C&C++)_20180205.md)
* Python正在更新

## 2.内容支持文档

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

成功上传算法后，极市会进行算法测试，并且将测试结果反馈给开发者。

## 4.技术支持

需要任何技术支持，请加小助手微信（Extreme-Vision）或者 QQ 3053890056。同时欢迎大家提交演示demo至developer@cvmart.net,我们将免费为每一位开发者推广变现算法。

