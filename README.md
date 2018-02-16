### README

![LOGO](./img/LOGO.jpeg)

#### 算法提交流程

1. （假设算法已经开发完成）


2. 根据支持的接口形式（C/C++/Python）选择相应的接口文件，并构造函数实现算法


3. 根据开发语言（C/C++/Python）的要求，完善开发者算法SDK所需要的所有文件并使用Docker进行打包提交

* [C/C++参见这里](./doc/极市平台算法Docker镜像规范(C&C++)_20180205.md)
* Python正在更新...

#### 本项目是为了给极市平台的开发者上传算法提供以下内容的支持:

1. 算法的接口文件
	* C/C++: 
		* （必须）[ji.h](./ji.h) 
		* （非必须）[ji_util.h](./ji_util.h)
	* Python
		* （必须）[ji.py](./ji.py)
2. [Docker操作指导与上传方式](./doc/Docker安装与简单使用指南.docx)
3. 示例代码
	* [C/C++代码示例](./sample_c)
		* [如何把图片文件转换为buffer输入并做测试](./sample_c/standard_sample(convert_file_to_buffur)/)
		* [如何从buffer转换到opencv的Mat格式](./sample_c/convert_buffer_to_opencv_mat/)
		* [如何把算法代码编译位动态库](./sample_c/gcc-so.sh)
		* 持续更新...
	* [Python代码示例](./sample_py)
		* 持续更新


