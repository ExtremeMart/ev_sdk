#ifndef __JI_H__
#define __JI_H__

#ifdef __cplusplus
extern "C"
{
#endif

// 版本号,基于license:v20
#define EV_SDK_VERSION  "3.0.3"

// 函数返回值定义
#define JISDK_RET_SUCCEED               (0)             // 成功
#define JISDK_RET_FAILED                (-1)            // 失败
#define JISDK_RET_UNUSED                (-2)            // 未实现
#define JISDK_RET_INVALIDPARAMS         (-3)            // 参数错误
#define JISDK_RET_OFFLINE               (-9)            // 联网校验时离线状态
#define JISDK_RET_OVERMAXQPS            (-99)           // 超过最大请求量
#define JISDK_RET_UNAUTHORIZED          (-999)          // 未授权

// ji_create_predictor可选输入参数定义
#define JISDK_PREDICTOR_DEFAULT         (0)             // 默认
#define JISDK_PREDICTOR_SEQUENTIAL      (1)             // 连续的，即带状态的
#define JISDK_PREDICTOR_NONSEQUENTIAL   (2)             // 非连续的，即不带状态的

// JI_EVENT.code值定义
#define JISDK_CODE_ALARM                (0)             // 报警
#define JISDK_CODE_NORMAL               (1)             // 正常
#define JISDK_CODE_FAILED               (-1)            // 失败


// 图像存储结构体，参考cv::Mat
typedef struct {
	int rows;           // cv::Mat::rows
	int cols;           // cv::Mat::cols
	int type;           // cv::Mat::type()
	void *data;         // cv::Mat::data
	int step;           // cv::Mat::step
} JI_CV_FRAME;

// 算法处理后存储输出信息的结构体
typedef struct {
	int code;           // 详见"JI_EVENT.code值定义"
	const char *json;   // 算法输出结果，json格式的字符串
} JI_EVENT;

/**
 * sdk初始化，可在函数接入license授权功能
 *
 * @param[in] argc 参数数量
 * @param[in] argv 参数数组
 * @return 详见"函数返回值定义"，成功返回JISDK_RET_SUCCEED，其它表示失败
 */
int ji_init(int argc, char **argv);

/**
 * sdk反初始化函数，主要用于联网license校验线程终止等
 */
void ji_reinit();

/**
 * 创建算法实例，算法在处理过程中，如果当前帧的处理依赖于前面若干帧的结果，那么称为算法是有状态的；相反，如果算法处理只依赖于当前输入帧，那
 * 么称为算法是无状态的。针对这一情况，目前该接口有三种可选参数：
 * 1. JISDK_PREDICTOR_DEFAULT：  算法默认创建的算法类型
 * 2. JISDK_PREDICTOR_SEQUENTIAL：   创建带状态的算法类型
 * 3. JISDK_PREDICTOR_NONSEQUENTIAL：创建不带状态的算法类型
 * 若算法`仅`支持不带状态的算法类型，传任何参数均创建不带状态的算法类型，若算法`仅`支持带状态的算法类型，传任何参数均创建带状态的算法类型
 *
 * @param[in] pdtype [必选]检测器实例类型,详见"检测器类型值定义"
 * @return 成功返回检测器实例指针,错误返回NULL, 注意：授权未通过或超过有效期时，也返回NULL
 */
void* ji_create_predictor(int pdtype);

/**
 * 释放算法实例
 *
 * @param[in] predictor [必选] 算法句柄
 */
void ji_destroy_predictor(void *predictor);

/**
 * 算法处理JI_CV_FRAME格式的图像
 *
 * @param[in] predictor [必选] 不允许为NULL，算法句柄，有ji_create_predictor函数创建
 * @param[in] inFrame   [必选] 不允许为NULL，输入源帧
 * @param[in] args      [必选] 允许为NULL，检测参数; 由开发者自行定义，例如：roi
 * @param[out] outFrame [必选] 允许为NULL，输出结果帧; outFrame->data由开发者创建和释放
 * @param[out] event    [必选] 允许为NULL，输出结果; event->json由开发者创建和释放
 * @return 成功返回JISDK_RET_SUCCEED，其它表示失败，详见"函数返回值定义"
 */
int ji_calc_frame(void *predictor, const JI_CV_FRAME *inFrame, const char *args,
                  JI_CV_FRAME *outFrame, JI_EVENT *event);

/**
 * 算法处理指定地址的图片缓冲区
 *
 * @param[in] predictor[in] [必选] 不允许为NULL，算法句柄，由ji_create_predictor函数创建
 * @param[in] buffer[in]    [必选] 不允许为NULL，源图片缓冲地址
 * @param[in] length[in]    [必选] 必须大于0，源图片缓冲大小
 * @param[in] args[in]      [必选] 允许为NULL，算法处理参数，由开发者自行定义，例如：roi
 * @param[out] outFile[in]  [必选] 允许为NULL，输出结果图片文件路径
 * @param[out] event[out]   [必选] 允许为NULL，输出结果，event->json由开发者创建和释放
 * @return 成功返回JISDK_RET_SUCCEED，其它表示失败，详见"函数返回值定义"
 */
int ji_calc_buffer(void *predictor, const void *buffer, int length,
                   const char *args, const char *outFile, JI_EVENT *event);

/**
 * 算法处理指定路径的图片文件，并将输出图片保存到指定路径
 *
 * @param[in] predictor [必选] 不允许为NULL，算法句柄，有ji_create_predictor函数创建
 * @param[in] inFile    [必选] 不允许为NULL，源图片文件路径
 * @param[in] args      [必选] 允许为NULL，算法处理参数，由开发者自行定义，例如：roi
 * @param[in] outFile   [必选] 允许为NULL，输出结果图片文件路径
 * @param[out] event    [必选] 允许为NULL，输出结果，event->json由开发者创建和释放
 * @return 成功返回JISDK_RET_SUCCEED，其它表示失败，详见"函数返回值定义"
 */
int ji_calc_file(void *predictor, const char *inFile, const char *args, const char *outFile, JI_EVENT *event);

/**
 * 算法处理指定路径下的视频文件，并将输出结果保存为视频和json文件
 *
 * @param[in] predictor [必选] 不允许为NULL，算法句柄，由ji_create_predictor函数创建
 * @param[in] inFile    [必选] 不允许为NULL，源视频文件路径
 * @param[in] args      [必选] 允许为NULL，算法处理参数，由开发者自行定义，例如：roi
 * @param[in] outFile   [必选] 允许为NULL，输出结果视频文件路径
 * @param[in] jsonFile  [必选] 允许为NULL，输出结果json文件路径
 * @return 成功返回JISDK_RET_SUCCEED，其它表示失败，详见"函数返回值定义"
 */
int ji_calc_video_file(void *predictor, const char *inFile, const char* args,
                       const char *outFile, const char *jsonFile);
		
#ifdef __cplusplus
}
#endif

#endif
