# EV_SDK

## 说明
### EV_SDK的目标
开发者专注于算法开发及优化，最小化业务层编码，即可快速部署到生产环境，共同打造商用级高质量算法。
### 极市平台做了哪些
1. 统一定义算法接口：针对万千视频和图片分析算法，抽象出接口，定义在`include`目录下的`ji.h`文件中
2. 提供工具包：比如算法授权（必须实现）、模型加密，在`3rd`目录下
3. 应用层服务：此模块不在ev_sdk中，比如视频处理服务、算法对外通讯的http服务等

### 开发者需要做什么
1. 模型的训练和调优
2. 实现`ji.h`约定的接口，同时包括授权、支持分析区域等功能
3. 实现约定的输入输出
4. 其他后面文档提到的功能

## 目录

### 代码目录结构

```
ev_sdk
|-- 3rd             # 第三方源码或库目录，发布时请删除
|   |-- wkt_parser          # 针对使用WKT格式编写的字符串的解析器
|   |-- cJSON               # c版json库，简单易用
|   |-- ev_encrypt_module   # 模型加密库及相关工具
|   |-- darknet             # 示例项目依赖的库
|   `-- license             # SDK授权库及相关工具
|-- CMakeLists.txt          # 本项目的cmake构建文件
|-- README.md       # 本说明文件
|-- model           # 模型数据存放文件夹
|-- config          # 程序配置目录
|   |-- README.md   # algo_config.json文件各个参数的说明和配置方法
|   `-- algo_config.json    # 程序配置文件
|-- doc
|-- include         # 库头文件目录
|   `-- ji.h        # libji.so的头文件，理论上仅有唯一一个头文件
|-- lib             # 本项目编译并安装之后，默认会将依赖的库放在该目录，包括libji.so
|-- src             # 实现ji.cpp的代码
`-- test            # 针对ji.h中所定义接口的测试代码，请勿修改！！！
```
## 使用示例
作为示例，我们提供了一个使用`darknet`实现的图像检测器，并将其使用`EV_SDK`规范进行封装，需要实现的业务逻辑是当检测到**狗**时，需要返回相关的报警信息。使用如下步骤尝试编译和测试该项目：

#### 下载`EV_SDK`

```shell
git clone https://github.com/ExtremeMart/dev-docs
mv dev-docs /usr/local/ev_sdk
```

#### 编译

编译和安装`libji.so`：

```shell
mkdir -p /usr/local/ev_sdk/build
cd /usr/local/ev_sdk/build
cmake ..
make install
```

#### 测试示例程序和接口规范

执行完成之后，`/usr/local/ev_sdk/lib`下将生成`libji.so`和相关的依赖库，以及`/usr/local/ev_sdk/bin/`下的测试程序`test-ji-api`。

1. 要使用`/usr/local/ev_sdk/bin/test-ji-api`测试`EV_SDK`的接口，需要重新生成授权所使用的参考码`reference.txt`，并使用私钥对其进行加密后重新生成授权文件`license.txt`

   ```shell
   # 生成公私钥
   mkdir -p /usr/local/ev_sdk/authorization
   cd /usr/local/ev_sdk/authorization/
   /usr/local/ev_sdk/3rd/license/v20_0/bin/generateRsaKey.sh
   # 生成参考文件
   /usr/local/ev_sdk/3rd/license/v20_0/bin/ev_license -r reference.txt
   # 生成授权文件
   /usr/locak/ev_sdk/3rd/license/v20_0/bin/ev_license -l privateKey.pem reference.txt license.txt
   ```

2. 使用`test-ji-api`测试`ji_calc_frame`接口，测试添加了一个`ROI`参数

   ```shell
   /usr/local/ev_sdk/bin/test-ji-api -f ji_calc_frame -i /usr/local/ev_sdk/data/dog.jpg -o /tmp/output.jpg -l /usr/local/ev_sdk/authorization/license.txt -a "{\"roi\":[\"POLYGON((0.21666666666666667 0.255,0.6924242424242424 0.1375,0.8833333333333333 0.72,0.4106060606060606 0.965,0.048484848484848485 0.82,0.2196969696969697 0.2575))\"]}"
   ```

   输出内容样例：

   ```shell
    code: 0
    json: {
    "alert_flag":   1,
    "dogs": [{
            "xmin": 129,
            "ymin": 186,
            "xmax": 369,
            "ymax": 516,
            "confidence":   0.566474
        }]
    }
   ```

## 使用`EV_SDK`快速封装算法

假设项目需要检测输入图像中是否有**狗**，如果检测到**狗**，就需要输出报警信息，以下示例开发算法与使用`EV_SDK`进行封装的流程

#### 实现自己的模型

假设我们使用`darknet`开发了针对**狗**的检测算法，程序需要在检测到狗时输出报警信息。

#### 下载`EV_SDK`

```shell
git clone https://github.com/ExtremeMart/dev-docs
mv dev-docs /usr/local/ev_sdk
```

#### 生成授权功能所依赖的文件

1. 使用`EV_SDK`提供的工具生成公钥和私钥

   ```shell
   mkdir -p /usr/local/ev_sdk/authorization
   cd /usr/local/ev_sdk/authorization
   /usr/local/3rd/license/v20_0/bin/generateRsaKey.sh
   ```

   执行成功后将生成公钥`authorization/pubKey.perm`和私钥`authorization/privateKey.pem`。

2. 将公钥转换成`C++`头文件

   ```shell
   /usr/local/ev_sdk/3rd/license/v20_0/bin/ev_codec -c authorization/pubKey.perm authorization/pubKey.hpp
   # 将头文件移动到代码处
   mv /usr/local/ev_sdk/authorization/pubKey.hpp /usr/local/ev_sdk/include
   ```

   这个包含公钥的头文件将被**硬编码**到`libji.so`。

3. 在`ji_init(int argc, char **argv)`的接口实现中，添加校验授权文件的功能。

   > **注：这部分代码在示例代码`ji.cpp`中已经实现，可以无需变动，直接使用。**
   
   ```c++
   // 使用公钥校验授权信息
   int ret = ji_check_license(pubKey, license, url, activation, timestamp, qps, version);
   return ret == EV_SUCCESS ? JISDK_RET_SUCCEED : JISDK_RET_UNAUTHORIZED;
   ```

> 更多授权功能的原理，请参考[算法授权](doc/Authorization.md)。

#### 添加模型加密功能

1. 使用`EV_SDK`提供的工具加密模型，并生成`C++`头文件，**这里仅仅示例加密`yolov3-tiny.cfg`文件，请根据实际需要，对重要的模型/权重文件进行加密**

   ```shell
   mkdir -p /usr/local/ev_sdk/model_encryption/
   cd /usr/local/ev_sdk/model_encryption/
   /usr/local/3rd/ev_encrypt_module/bin/encrypt_tool /usr/local/ev_sdk/model/yolov3-tiny.cfg
   ```

   执行成功后会生成加密后的文件`model_str.hpp`，`encrypt_tool`程序支持在加密模型时指定一个混淆字符串，具体方法请执行`encrypt_tool`参考帮助文档。将头文件移动到代码区

   ```shell
   mv /usr/local/ev_sdk/model_encryption/model_str.hpp /usr/local/ev_sdk/include
   ```

   这个加密后的模型将被**硬编码**到`libji.so`。

2. 在`ji_create_predictor(int)`的接口实现中，添加模型解密的功能。

   **注：示例代码`ji.cpp`里面提供了解密的方法，对于加密文本类型的模型文件的场景可以直接使用，无需更改。**
   
   ```c++
   // 创建解密句柄
   void *h = CreateEncryptor(model_str.c_str(), model_str.size(), key.c_str());
   // 获取解密后的字符串
   int fileLen = 0;
   model_struct_str = (char *) FetchBuffer(h, fileLen);
   // 获取解密后的文件句柄
   // file *file = (file *) FetchFile(h);
   DestroyEncrtptor(h);
   ```
   
   模型解密的详细接口函数请参考其头文件[encrypt_wrapper.h](3rd/ev_encrypt_module/include/encrypt_wrapper.hpp)

#### 实现`ji.h`中的接口

`ji.h`中定义了所有`EV_SDK`规范的接口，详细的接口定义和实现示例，请参考头文件[ji.h](include/ji.h)和示例代码[ji.cpp](src/ji.cpp)。

将代码编译成`libji.so`

```shell
mkdir -p /usr/local/ev_sdk/build
cd /usr/local/ev_sdk/build
cmake ..
make install
```

编译完成后，将在`/usr/local/ev_sdk/lib`下生成`libji.so`和其他依赖的库。

#### 测试接口功能

测试`libji.so`的授权功能是否正常工作以及`ji.h`的接口规范

1. 生成授权文件

   1. 使用`EV_SDK`提供的工具生成与当前主机关联的硬件参考码

      ```shell
      cd /usr/local/ev_sdk/
      /usr/local/ev_sdk/3rd/license/v20_0/bin/ev_license -r authorization/reference.txt
      ```

   2. 使用私钥加密参考码，并生成授权文件`license.txt`

      ```shell
      /usr/local/ev_sdk/3rd/license/v20_0/tools/ev_license -l authorization/privateKey.pem authorization/reference.txt authorization/license.txt
      ```

2. 检查授权功能和`ji.h`的接口规范性

   `EV_SDK`代码中提供了测试所有接口的测试程序，**编译并安装**`libji.so`之后，会在`/usr/local/ev_sdk/bin`下生成`test-ji-api`可执行文件，`test-ji-api`用于测试`ji.h`的接口实现是否正常，例如，测试`ji_calc_frame`接口以及授权功能是否正常：

   ```shell
   /usr/local/ev_sdk/bin/test-ji-api -f ji_calc_frame \
   -i /usr/local/ev_sdk/data/dog.jpg \
   -o /tmp/output.jpg \
   -l /usr/local/ev_sdk/authorization/license.txt \
   -a "{\"roi\":[\"POLYGON((0.21666666666666667 0.255,0.6924242424242424 0.1375,0.8833333333333333 0.72,0.4106060606060606 0.965,0.048484848484848485 0.82,0.2196969696969697 0.2575))\"]}"
   ```
   
   接口测试程序的详细功能请查阅`test-ji-api --help`的帮助文档及其代码[test.cpp](test/src/test.cpp)

## 哪些内容必须完成才能通过测试？
#### 按照需求实现接口（由项目经理告知）
1. `ji_calc_frame` ，用于实时视频流分析
2. `ji_calc_buffer` ，用于分析图片`buffer`
3. `ji_calc_file` ，用于分析图片文件
4. `ji_calc_video_file` ：用于极市平台测试组测试和开发者自测视频文件

#### 规范要求

规范测试大部分内容依赖于内置的`/usr/local/ev_sdk/test`下面的代码，这个测试程序会链接`/usr/local/ev_sdk/lib/libji.so`库，`EV_SDK`封装完成提交后，极市方会使用`test-ji-api`程序测试`ji.h`中的所有接口。测试程序与`EV_SDK`的实现没有关系，所以请**请不要修改`/usr/local/ev_sdk/test`目录下的代码！！！**

1. 接口功能要求
  
   - 确定`test-ji-api`能够正常编译，并且将`test-ji-api`和`license.txt`移动到任意目录，都需要能够正常运行；
   
   - 在提交算法之前，请自行通过`/usr/local/ev_sdk/bin/test-ji-api`测试接口功能是否正常；
   
   - 未实现的接口需要返回`JISDK_RET_UNUSED`；
   
   - 实现的接口，如果传入参数异常时，需要返回`JISDK_RET_INVALIDPARAMS`；
   
   - 对于实现多个接口的情况，请确保每个接口对同样的输入数据保持一致的算法分析结果，比如`ji_calc_frame`和`ji_calc_file`两个接口对于同样的输入图片数据，应该保持一样的分析结果；
   
   - 输入图片和输出图片的尺寸应保持一致；
   
   - 对于接口中传入的参数`args`（如，`ji_calc_frame(void *, const JI_CV_FRAME *, const char *args, JI_CV_FRAME *, JI_EVENT *)`中中`args`），根据项目需求，算法实现需要支持`args`实际传入的参数。
   
     例如，如果项目需要支持在`args`中传入`roi`参数，使得算法只对`roi`区域进行分析，那么**算法内部必须实现只针对`roi`区域进行分析的功能**；
   
   - 对于接口`ji_calc_video`接口，其保存的`json`文件格式必须与`ji_calc_frame`、`ji_calc_file`、`ji_calc_buffer`中的`JI_EVENT.json`格式保持一致；
   
   - 通常输出图片中需要画`roi`区域、目标框等，请确保这一功能正常，包括但不仅限于：
   
     - `args`中输入的`roi`需要支持多边形
     - 算法默认分析区域必须是全尺寸图，如当`roi`传入为空时，算法对整张图进行分析；
     
   - 为了保证多个算法显示效果的一致性，与画框相关的功能必须优先使用`ji_utils.hpp.h`中提供的工具函数；
   
   > 1. ` test-ji-api`的使用方法可以参考上面的使用示例以及运行`test-ji-api --help`；
   > 2. 以上要求在示例程序`ji.cpp`中有实现；
   
2. 业务逻辑要求

   针对需要报警的需求，算法必须按照以下规范输出结果：
   * 报警时输出：`JI_EVENT.code=JISDK_CODE_ALARM`，`JI_EVENT.json`内部填充`"alert_flag"=1`；
   * 未报警时输出：`JI_EVENT.code=JISDK_CODE_NORMAL`，`JI_EVENT.json`内部填充`"alert_flag"=0`；
   * 处理失败的接口返回`JI_EVENT.code=JISDK_CODE_FAILED`

   * 算法输出的`json`数据必须与项目需求保持一致；

3. 授权功能要求

   需要实现授权功能，并且在调用接口（比如`ji_calc_frame`）时，如果授权没有通过，必须返回`JISDK_RET_UNAUTHORIZED`。

   > **注：授权功能已经在示例代码实现，基本不需要修改**

4. 算法配置选项要求

   - `EV_SDK`的实现需要使用标准[JSON](https://www.json.cn/wiki.html)格式的配置文件，所有算法与`SDK`可配置参数**必须**存放在统一的配置文件：`/usr/local/ev_sdk/config/algo_config.json`中；
   - 配置文件中必须实现的参数项：
     - `gpu_id`：整型，`-1`表示不使用GPU，大于`0`表示表示算法使用的GPU ID，算法必须能够根据GPU ID来使用特定的GPU；
     - `draw_roi_area`：`true`或者`false`，是否在输出图中绘制`roi`分析区域；
     - `roi_line_thickness`：ROI区域的边框粗细；
     - `roi_fill`：是否使用颜色填充ROI区域；
     - `roi_color`：`roi`框的颜色，以BGRA表示的数组，如`[0, 255, 0, 0]`，参考[model/README.md](model/README.md)；
     - `roi`：针对图片的感兴趣区域进行分析，如果没有此参数或者此参数解析错误，则roi默认值为整张图片区域；
     - ` thresh`：算法阈值，需要有可以调整算法灵敏度、召回率、精确率的阈值参数，如果算法配置项有多个参数，请自行扩展，所有与算法效果相关并且可以变动的参数**必须**在`/usr/local/ev_sdk/config/README.md`中提供详细的配置方法和说明（包括类型、取值范围、建议值、默认值、对算法效果的影响等）；
     - ` draw_result`：`true`或者`false`，是否绘制分析结果，比如示例程序中，如果检测到狗，是否将检测框和文字画在输出图中；
     - `draw_confidence`：`true`或者`false`，是否将置信度画在检测框顶部，小数点后保留两位；
     - 所有`json`内的键名称必须是小写字母，并且单词间以下划线分隔，如上面几个示例。
   - **必须支持参数实时更新**。除了`gpu_id`等必须在算法初始化时才能够更新的参数外，所有`/usr/local/ev_sdk/config/algo_config.json`内的可配置参数必须支持能够在调用`ji_calc_frame`、`ji_calc_buffer`、`ji_calc_file`、`ji_calc_video_file`四个接口时，进行实时更新。也就是必须要在`ji_calc_*`等接口的`args`参数中，加入这些可配置项。

5. 算法输出规范要求

   算法输出结果，即`JI_EVENT.json`必须是使用`json`格式填充的字符串，`json`字符串内所有的**键名称**必须是小写字母，并且单词之间使用下划线分隔，如`alert_flag`；

6. 文件结构规范要求

   * 授权功能生成的公私钥必须放在`/usr/local/ev_sdk/bin` ，且一次生成后，请勿再次更新公私钥（极市方会保存第一版的私钥），如果在后续更新中，重新生成了公私钥，会导致公私钥不匹配；
   * 与模型相关的文件必须存放在`/usr/local/ev_sdk/model`目录下，例如权重文件、目标检测通常需要的名称文件`coco.names`等。
   * 最终编译生成的`libji.so`必须自行链接必要的库，`test-ji-api`不会链接除`/usr/local/ev_sdk/lib/libji.so`以外的算法依赖库；
   * 如果`libji.so`依赖了系统动态库搜索路径（如`/usr/lib`，`/lib`等）以外的库，必须将其安装到`/usr/local/ev_sdk/lib`下，可以使用`ldd /usr/local/ev_sdk/lib/libji.so`查看`libji.so`是否正确链接了所有的依赖库。
   * **授权文件位置**
     * 请务必将生成的私钥`privateKey.pem`和公钥`publicKey.pem`放到`/usr/local/ev_sdk/authorization`下。**并请自行保存一份，后续算法迭代过程都会使用第一次提交的公私钥，不能重新生成**。


## FAQ

### 如何使用接口中的`args`？

通常，在实际项目中，外部需要将多种参数（例如`ROI`）传入到算法，使得算法可以根据这些参数来改变处理逻辑。`EV_SDK`接口（如`int ji_calc_frame(void *, const JI_CV_FRAME *, const char *args, JI_CV_FRAME *, JI_EVENT *)`中的`args`参数通常由开发者自行定义和解析，但只能使用[JSON](https://www.json.cn/wiki.html)格式。格式样例：

```shell
{
    "cid": "1000",
    "roi": [
        "POLYGON((0.0480.357,0.1660.0725,0.3930.0075,0.3920.202,0.2420.375))",
        "POLYGON((0.5130.232,0.790.1075,0.9280.102,0.9530.64,0.7590.89,0.510.245))",
        "POLYGON((0.1150.497,0.5920.82,0.5810.917,0.140.932))"
    ],
    "cross_line": ["LINESTRING(0.070.21,0.360.245,0.580.16,0.970.27)"],
    "point": [
            "POINT(0.38 0.10)",
            "POINT(0.47 0.41)"
    ]
}
```

例如当算法支持输入`ROI`参数时，那么开发者需要在`EV_SDK`的接口实现中解析上面示例中`roi`这一值，提取其中的`ROI`参数，并使用`WKTParser`对其进行解析，应用到自己的算法逻辑中。
### 为什么不能且不需要修改`/usr/local/ev_sdk/test`下的代码？

1. `/usr/local/ev_sdk/test`下的代码是用于测试`ji.h`接口在`libji.so`中是否被正确实现，这一测试程序与`EV_SDK`的实现无关，且是极市方的测试标准，不能变动；
2. 编译后`test-ji-api`程序只会依赖`libji.so`，如果`test-ji-api`无法正常运行，很可能是`libji.so`没有按照规范进行封装；

### 为什么运行`test-ji-api`时，会提示找不到链接库？

由于`test-ji-api`对于算法而言，只链接了`/usr/local/ev_sdk/lib/libji.so`库，如果`test-ji-api`运行过程中，找不到某些库，那么很可能是`libji.so`依赖的某些库找不到了。此时

1. 可以使用`ldd /usr/local/ev_sdk/lib/libji.so`检查是否所有链接库都可以找到；
2. 请按照规范将系统动态库搜索路径以外的库放在`/usr/local/ev_sdk/lib`目录下。

### 如何使用`test-ji-api`进行测试？

1. 输入单张图片和授权文件，并调用`ji_calc_frame`接口：

   ```shell
   ./test-ji-api -f ji_calc_frame -i /path/to/test.jpg -l /path/to/license.txt
   ```

2. 输入`json`格式的`roi`参数到`args`参数：

   ```shell
   ./test-ji-api \
   -f ji_calc_frame \
   -i /path/to/test.jpg \
   -l /path/to/license.txt \
   -a "{\"roi\":[\"POLYGON((0.21666666666666667 0.255,0.6924242424242424 0.1375,0.8833333333333333 0.72,0.4106060606060606 0.965,0.048484848484848485 0.82,0.2196969696969697 0.2575))\"]}"
   ```

3. 保存输出图片：

   ```shell
   ./test-ji-api -f ji_calc_frame -i /path/to/test.jpg -l /path/to/license.txt -o /path/to/out.jpg
   ```

更多选项，请参考`test-ji-api --help`