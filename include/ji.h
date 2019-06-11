#ifndef __JI_H__
#define __JI_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* 版本号,基于license:v10 */
#define EV_SDK_VERSION  "version:v2.5"

/* 函数返回值定义 */
#define JISDK_RET_SUCCEED               (0)             //成功
#define JISDK_RET_FAILED                (-1)            //失败
#define JISDK_RET_UNUSED                (-2)            //未实现
#define JISDK_RET_INVALIDPARAMS         (-3)            //参数错误
#define JISDK_RET_OVERMAXQPS            (-99)           //超过最大请求量
#define JISDK_RET_UNAUTHORIZED          (-999)          //未授权

/* 检测器类型值定义 */
/* 
**
1.如果算法仅支持不带状态的检测器,传任何参数均创建不带状态的检测器;
2.如果算法仅支持带状态的检测器,传任何参数均创建带状态的检测器;
3.如果算法同时支持不带或带状态的检测器时：
  JISDK_PREDICTOR_DEFAULT:       开发者自行决定创建;
  JISDK_PREDICTOR_SEQUENTIAL:    创建带状态的检测器;
  JISDK_PREDICTOR_NONSEQUENTIAL: 创建不带状态的检测器;
**
*/
#define JISDK_PREDICTOR_DEFAULT         (0)             //默认
#define JISDK_PREDICTOR_SEQUENTIAL      (1)             //连续的,即带状态的
#define JISDK_PREDICTOR_NONSEQUENTIAL   (2)             //非连续的,即不带状态的

/* 分析输出code值定义 */
#define JISDK_CODE_ALARM                (0)             //报警
#define JISDK_CODE_NORMAL               (1)             //正常
#define JISDK_CODE_FAILED               (-1)            //失败


/* 单帧信息，参考cv::Mat */
typedef struct {
	int rows;           //cv::Mat::rows
	int cols;           //cv::Mat::cols
	int type;           //cv::Mat::type()
	void *data;         //cv::Mat::data
	int step;           //cv::Mat::step
} JI_CV_FRAME;

/* 分析输出信息 */
typedef struct {
	int code;           //详见"分析输出code值定义"
	const char *json;   //算法输出结果,json格式
} JI_EVENT;

/*
函 数 名	: ji_init
功能描述	: sdk初始化,可在函数接入license,glog,log4cpp等
函数参数	: 变参原型,参数有开发者自行定义
返回参数	: 详见"函数返回值定义",成功返回JISDK_RET_SUCCEED,其它表示失败  
注     意: 无
*****************************************************************************/
int ji_init(int argc, char **argv);


/*
函 数 名 : ji_reinit
功能描述    : sdk反初始化函数,主要用于联网license校验线程终止等
函数参数    : 无
返回参数    : 无 
注     意: 无
*****************************************************************************/
void ji_reinit();


/*
函 数 名	: ji_create_predictor
功能描述	: 创建检测器,即每个检测实例的上下文
函数参数	: 
    pdtype: [必选参数] 输入参数,检测器实例类型,详见"检测器类型值定义"
返回参数	: 成功返回检测器实例指针,错误返回NULL  
注     意: 授权未通过或超过有效期时，也返回NULL
*****************************************************************************/
void* ji_create_predictor(int pdtype);


/*
函 数 名: ji_destroy_predictor
功能描述    : 释放检测器实例
函数参数    : 检测器实例
    predictor: [必选参数] 输入参数,检测器实例
返回参数    : 无  
注     意: 无
*****************************************************************************/
void ji_destroy_predictor(void *predictor);


/*
函 数 名: ji_calc_frame
功能描述    : 算法检测针对一帧
函数参数    : 
    predictor: [必选参数] 输入参数,不允许为NULL,检测器实例;有ji_create_predictor函数生成.
    inframe:   [必选参数] 输入参数,不允许为NULL,输入源帧.
    args:      [必选参数] 输入参数,允许为NULL,检测参数;有开发者自行定义，例如:roi.
    outframe:  [必选参数] 输出参数,允许为NULL,输出结果帧;outframe->data有开发者创建和释放.
    event:     [必选参数] 输出参数,允许为NULL,输出结果;event->json有开发者创建和释放.
返回参数    : 详见"函数返回值定义",成功返回JISDK_RET_SUCCEED,其它表示失败 
*****************************************************************************/
int ji_calc_frame(void *predictor, const JI_CV_FRAME *inframe, const char *args,
                  JI_CV_FRAME *outframe, JI_EVENT *event);


/*
函 数 名: ji_calc_buffer
功能描述    : 算法检测针对图片缓冲
函数参数    : 
    predictor: [必选参数] 输入参数,不允许为NULL,检测器实例;有ji_create_predictor函数生成.
    buffer:    [必选参数] 输入参数,不允许为NULL,源图片缓冲地址.
    length:    [必选参数] 输入参数,必须大于0,源图片缓冲大小.
    args:      [必选参数] 输入参数,允许为NULL,检测参数;有开发者自行定义，例如:roi.
    outfile:   [必选参数] 输入参数,允许为NULL,输出结果图片文件路径.
    event:     [必选参数] 输出参数,允许为NULL,输出结果;event->json有开发者创建和释放.
返回参数    : 详见"函数返回值定义",成功返回JISDK_RET_SUCCEED,其它表示失败 
*****************************************************************************/
int ji_calc_buffer(void *predictor, const void *buffer, int length,
                   const char *args, const char *outfile, JI_EVENT *event);

/*
函 数 名: ji_calc_file
功能描述    : 算法检测针对图片文件
函数参数    : 
   predictor: [必选参数] 输入参数,不允许为NULL,检测器实例;有ji_create_predictor函数生成.
   infile:    [必选参数] 输入参数,不允许为NULL,源图片文件路径.
   args:      [必选参数] 输入参数,允许为NULL,检测参数;有开发者自行定义，例如:roi.
   outfile:   [必选参数] 输入参数,允许为NULL,输出结果图片文件路径.
   event:     [必选参数] 输出参数,允许为NULL,输出结果;event->json有开发者创建和释放.
返回参数    : 详见"函数返回值定义",成功返回JISDK_RET_SUCCEED,其它表示失败 
*****************************************************************************/
int ji_calc_file(void *predictor, const char *infile, const char *args,
                 const char *outfile, JI_EVENT *event);


/*
函 数 名: ji_calc_video_file
功能描述    : 算法检测针对视频文件
函数参数    : 
    predictor: [必选参数] 输入参数,不允许为NULL,检测器实例;有ji_create_predictor函数生成
    infile:    [必选参数] 输入参数,不允许为NULL,源视频文件路径.
    args:      [必选参数] 输入参数,允许为NULL,检测参数;有开发者自行定义，例如:roi.
    outfile:   [必选参数] 输入参数,允许为NULL,输出结果视频文件路径.
    jsonfile:  [必选参数] 输入参数,允许为NULL,输出结果json文件路径.
返回参数    : 详见"函数返回值定义",成功返回JISDK_RET_SUCCEED,其它表示失败 
*****************************************************************************/
int ji_calc_video_file(void *predictor, const char *infile, const char* args,
                       const char *outfile, const char *jsonfile);
		
#ifdef __cplusplus
}
#endif

#endif
