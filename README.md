# 目录
* [简介](#简介)
* [ev_sdk目录结构](#ev_sdk目录结构)
* [编译sdk](#编译sdk)
* [测试sdk](#测试sdk)
* [发布sdk](#发布sdk)

# 简介
* ev_sdk是极市算法sdk2.0标准的实现示例，一键sdk及专家模式均可以使用该示例作为实现模板；
* 极市提供的算法docker镜像均提供该实现示例，请根据算法的产品需求选择对应docker镜像;
* 请在docker container内开发及调试。

# ev_sdk目录结构

    /usr/local/ev_sdk
    ├── README.md       # 本帮助文档
    ├── bin             # 发布运行目录
        ├── onKeySdk.sh             # 一键生成sdk，包括生成算法密钥、编译sdk、编译test
    	├── oneKeyTest.sh           # 一键测试，自动测试
		├── clearSrcCode.sh         # 开发完成后但在发布前，用于清除开发者源码等[有开发者补充完善]
        ├── ev_codec                # 混编工具
        ├── ev_license              # 授权工具
        ├── encrypt_model           # 模型加密工具
        ├── test                    # sdk测试工具，有{../test}项目生成
                                    以下是onKeySdk.sh脚本生成的文件 
        ├── privateKey.pem          # rsa私钥,请妥善保管，发布时请务必删除
        ├── pubKey.pem              # rsa公钥
        ├── r.tx                    # 本docker容器的参考码
        ├── license.txt             # 本docker容器的授权码
        └── licenses.manifest	
		
    ├── lib             # 发布库文件目录，包括libji.so及其依赖的其它库文件，但不包括镜像本身提供的库(例如：openv,ffmpeg)
        ├── libji.so                # ev_sdk最终生成的库文件，有{../src}项目生成
        └──  ...                    # 其它依赖库 
    ├── include         # 发布库头文件目录
        └── ji.h        # libji.so的头文件，理论上仅有唯一一个头文件
    ├── data            # 测试数据目录，例如开发者准备的测试图片或视频文件，其中视频文件尽量选择avi或mp4的封装格式及h264编码的文件
    ├── model           # 模型及模型数据，导入模型加密功能后模型硬编码进ev_sdk，请参考{3rd/encrypted_module-master}目录下文件档
		                            以下文件有算法内部使用，除algo_config.json文件外其它文件名称不受限
        ├── algo_config.json        # 算法配置文件，需要开发者补充实现
        ├── model.dat               # 模型文件
        └──  ...           	         
    ├── doc             # 文档目录，包括part4,5部分的文档均可放在此处
    ├── 3rd             # 第三方源码或库目录，包括boost for wkt(roi),cJSON,license,enctry model，发布时需移除
        ├── boost_interface         # boost for wkt(roi)
        ├── cJSON                   # c版json库,简单易用
        ├── license                 # sdk授权库,详见readme.docx
        └── encrypted_module-master # 模型加密库,详见README.md	
	├── src             # libji.so源码目录，发布时需移除
		                            CMakeLists.txt Makefile两者二选一
        ├── CMakeLists.txt          # 仅用于示例，演示基于Makefile
        ├── build                   # cmake临时目录
        ├── Makefile                # makfile，编译成功后，会自动拷贝至{../bin}目录
        ├── ji.cpp                  # libji.so实现示例[有开发者完善]
        └──  ...                    # 导入模型加密后会生成model_str.hpp文件;导入license后生成pubKey.hpp文件
    ├── test            # 测试源码目录，该目录的文件，请不要修改，特别是Makefile。发布时建议手动编译下以确定是否缺少libji.so相关依赖库，发布时建议保留
        ├── Makefile                # 编译成功后，会自动拷贝至{../bin}目录
        ├── test.cpp                # libji.so测试实现，其中提供cJSON及jsoncpp使用示例，详见cJSON_sample及jsoncpp_sample函数
        └──  ...           
    └── ... 更多内容省略

# 编译sdk
## 准备
* 第1步：将要实际生效的模型拷贝到 `/usr/local/ev_sdk/model`;
* 第2步：模型加密，有更新才运行，例如：

 `./3rd/encrypted_module-master/encrypt_model 算法模型文件 您的aes密钥(可选参数--32个可见字符)`  
 out file: ./model_str.hpp, copy this file to "./src" directory .
 
## 编译
* 运行`./bin/onKeySdk.sh`，一键生成sdk，包括生成算法密钥、编译sdk、编译test

## 手动编译
如果运行过以上操作，为简化操作，平时编译建议手动编译
 
* 运行`cd /usr/local/ev_sdk/src  && make`成功后，会在{lib}目录下成生libji.so库文件
* 运行`cd /usr/local/ev_sdk/test && make`成功后，会在{bin}目录下成生test可运行文件
 
# 测试sdk
## 测试
* 运行`cd /usr/local/ev_sdk/bin && ./onKeyTest.h`,一键自动测试

## 手动测试
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

    ./test -f 1 -i ../data/test_sub.jpg
    ./test -f 1 -i ../data/test_sub.mp4
    ./test -f 1 -i ../data/test_sub.jpg -o ../data/test_sub_out.jpg
    ./test -f 2 -i ../data/test_sub.jpg -o ../data/test_sub_out.jpg -r 10
    ./test -f 3 -i ../data/test_sub.jpg -o ../data/test_sub_out.jpg -r -1
    ./test -f 4 -i ../data/test_sub.mp4 -o ../data/test_sub_out.mp4
	
# 发布sdk
1. 2.0版本[极市平台](http://amber.extremevision.com.cn)流程：
	* 极市运营同学提供GPU资源；
	* 加载对应数据集，并创建虚拟机，如果没有对应数据集，需自己上传；
	* 在线训练；
	* 提交模型自动测试；
	* 测试完成后，按流程提交模型测试：
		 模型测试 -> 新建测试->刷新页面查看测试状态 ->模型测试结果通知后查看测试详情
	* 测试通过后，按如下流程提交sdk:
		生成服务 -> 专家模式 -> 填写表单 -> 点击提交
		->刷新页面查看SDK生成状态 
		->查看服务生成结果
	* 显示发布成功即表示发布sdk完成！
2. 1.0版本算法发布流程：
	* GPU：联系极市运营同学，获取GPU资源；
	* CPU：登录[极市平台](http://amber.extremevision.com.cn)，点击我的算法，点击“+”，创建算法，填写相关信息，点击申请上架后，极市开始对算法进行测试审核。审核通过后算法将发布在极市算法市场。
	
---
