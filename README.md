| **`Linux/Windows`**                               |




#### 说明

本项目为[极市](http://cvmart.net/)开发者技术文档，包括极市平台算法SDK规范，算法示例及环境要求。如有问题，请直接联系极市小助手（微信：Extreme-Vision)  反馈或者提交issue，感谢您的支持。




## 1.C/C++算法SDK规范

### 1.1 算法SDK详细说明文档

如算法开发语言为C/C++，需要选择对应的接口文件，构造函数实现算法,含图片和视频接口，并完善算法SDK所需要的所有文件，SDK说明文档见下方。

*（**必须**）**[SDK说明文档参见这里](./极市算法SDK说明文档V1.0(C&C++)_180911.md)**  



### 1.2 接口规范文件及实现文件

- 算法需要满足接口规范ji.h文件，并在ji.cpp里进行接口实现（接口参数在ji.h内有详细介绍）
  - （必须）[ji.h](./ji.h) **注意：所有检测结果返回的图片(outfn)和帧(outframe)大小,请按照原始输入图片(buffer)或帧(infn)的分辨率大小返回** 
  - （必须）[ji.cpp](./ji.cpp) 
- (必须)[demo.cpp](./Test_Demo.cpp）推荐使用极市文档，并且不要在里面添加任何自定义的头文件（main.cpp内已经包含头文件 ji_util.hpp除外（这个是极市推荐的图片转buffer接口 .h文件） 详情请看[demo](./convert_File2Buffer)）。



## 2. ji.h及ji.cpp具体算法样例

### 2.1 算法图片封装接口示例

- 算法图片封装接口ji.cpp实现样例及调用demo 文件 main.cpp

  - [ji.cpp](./algorithm_sample(image)/ji.cpp)
  - [main.cpp](./algorithm_sample(image)/main.cpp) 



### 2.2 算法视频接口封装算法示例 

  - 算法视频接口封装ji.cpp实现样例及调用demo  
    - [ji.cpp](./algorithm_sample(video)/ji.cpp)
    - [main.cpp](./algorithm_sample(video)/main.cpp)



### 2.3 图片转buffer示例

   - 图片文件转buffer的示例代码[Linux版本](./convert_File2Buffer/linux/ji_util.h)   [Windows版本](./convert_File2Buffer/windows/ji_util.h) （用ji_file2buffer 函数实现,buffer为Vector类型）
   - [buffer转opencv的Mat格式](./convert_Buffer2Mat/ji_sample_mat.cpp)(用imdecode 实现，在ji_calc 接口里实现）



## 3.Python 接口规范

如开发语言为Python，按照下面接口进行实现。

* （必须）[ji.py](./ji.py)  



## 4.极市算法环境要求（CPU/GPU)

* Linux Ubuntu 16.04/Windows
* CUDA 8.0 （GPU)
* GCC 5.4.0
* ffmpeg 3.1，支持x264



## 5. 反馈

如有任何问题，欢迎加小助手微信（Extreme-Vision）或者 QQ 3053890056。如有任何问题，欢迎大家提交issue。
