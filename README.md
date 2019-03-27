# 目录
* [简介](#简介)
* [ev_sdk目录结构](#ev_sdk目录结构)
* [编译说明](#编译说明)
* [C&C++算法SDK规范](#C&C++算法SDK规范)
* [极市算法环境要求（CPU/GPU)](#极市算法环境要求（CPU/GPU))
* [反馈](#反馈)

# 简介
* ev_sdk是极市平台算法sdk标准的实现示例
* 一键sdk及专家模式均可以使用该示例作为实现模板
* 极市平台提供的算法docker镜像均提供该使用示例

# ev_sdk目录结构

    /usr/local/ev_sdk
    ├── README.md       # 本帮助文档
    ├── bin             # 运行目录
    ├── data            # 测试数据目录
    ├── lib             # 库文件目录（libji.so及依赖的其它库文件）
    ├── include         # 头文件目录
    ├── model           # 模型及模型数据(加入模型加密功能后模型硬编码进ev_sdk)
    ├── doc             # 文档目录，包括part4,5部分的文档均可放在此处
    ├── 3rd             # 第三方源码或库目录，包括boost for wkt(roi),cJSON,license,enctry model
        ├── boost_interface         # boost for wkt(roi)
        ├── cJSON                   # c版json库,简单易用
        ├── license                 # sdk授权库,详见readme.docx
        └── encrypted_module-master # 模型加密库,详见README.md
	
	├── src             # libji.so源码目录
        ├── Makefile        # 编译成功后，会自动拷贝至{lib}目录
        ├── ji.cpp          # libji.so实现示例[由开发者完善]
        └──  ...           
    ├── test            # 测试源码目录(该目录的文件，请不要修改，特别是Makefile)
        ├── Makefile        # 编译成功后，会自动拷贝至{bin}目录
        ├── test.cpp        # libji.so测试实现
        └──  ...           
    ├── oneKeyTest.sh   # 一键完成编译、测试脚本[待实现]
    ├── clearSrcCode.sh # 开发完成后但在发布前，用于清除开发者源码等[由开发者完善]
    └── ... 更多内容省略

# 编译说明

## libji.so编译
 运行`cd /usr/local/ev_sdk && cd src && make`成功后，会在{lib}目录下生成libji.so库文件
 
## 测试工具编译
 运行`cd /usr/local/ev_sdk && cd test && make`成功后，会在{bin}目录下成生test可运行文件
 
 运行`./test --help`,可输出使用帮助如下
 
    usage:
      -h  --help        show help information
      -f  --function    test function for 
                        [1.ji_calc_frame,2.ji_calc_buffer,3.ji_calc_file,4.ji_calc_video_file]
      -l  --license     license file. default: license.txt
      -i  --infile      source file
      -a  --args        for example roi
      -o  --outfile     result file
      -r  --repeat      number of repetitions. default: 1
                        <= 0 represents an unlimited number of times
                        for example: -r 100

 例如

    ./test -f 1 -i ../data/test_sub.jpg -o ../data/test_sub_out.jpg
    ./test -f 2 -i ../data/test_sub.jpg -o ../data/test_sub_out.jpg -r -1
    ./test -f 3 -i ../data/test_sub.jpg -o ../data/test_sub_out.jpg -r 10
    ./test -f 4 -i ../data/test_sub.mp4 -o ../data/test_sub_out.mp4 -r 100
 
---

# C&C++算法SDK规范

## 算法SDK详细说明文档
 如算法开发语言为c&c++，需要选择对应的接口，构造函数，并完善算法SDK所需要的所有文件，SDK说明文档见下方。

 *(**必须**)**[SDK说明文档](./极市算法SDK说明文档(V2.0).md)**

## 接口规范及实现文件

 - 算法需要满足接口规范ji.h文件，并在ji.cpp里进行接口实现（接口参数在ji.h内有详细介绍）
    - （必须）[ji.h](./include/ji.h) **注意：所有检测结果返回的图片或视频(outfn)和帧(outframe)大小,请按照原始输入图片(buffer)或帧(infn)的分辨率大小返回，不能有所裁剪** 
    - （必须）[ji.cpp](./src/ji.cpp)
 - (必须)[test.cpp](./test/test.cpp)推荐使用极市Demo文档进行开发与测试，无需添加自定义头文件。

# 极市算法环境要求（CPU/GPU)

* Linux Ubuntu 16.04/Windows
* CUDA 10.0 & 8.0 （GPU)
* GCC 5.4.0
* ffmpeg 3.1.11，支持x264
* Opencv 3.4.0

**注意**：如有特殊要求请联系极视工作人员

# 反馈

文档会不定时更新，请见谅。如有任何问题，欢迎加小助手微信（Extreme-Vision）或者 QQ 3053890056。如有任何问题，欢迎大家提交issue。
