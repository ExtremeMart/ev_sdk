#ifndef JI_H
#define JI_H

/*
 * Notes:
 * 1. If use init funciton, no need to release library or model.
 * 2. Please release frame inside the functions(ji_calc_video_frame).
 * 3. Please release filebuffer inside the functions(ji_calc).
 */

/*
 *  算法分析分为图片、视频、视频单帧三种类型接口,请开发者根据自己算法功能选择接口进行实现,并自行实现接口调用的demo演示（推荐使用极市模板）,方便测试人员进行接口性能测试
 *  接口文件.h文件无需修改,未使用或未实现的接口在.cpp文件中,并统一返回int型-2(未实现/未使用),使用的接口统一返回 0(成功)或者-1(失败)
 *  接口实现包括ji_init（插件、license初始化）,ji_create_predictor（创建检测器实例）,ji_destory_predictor(释放检测器实例)
 *  ji_calc(图片buffer分析接口),ji_calc_file(图片文件分析接口),ji_calc_video_file（视频分析接口）,ji_calc_video_frame（视频单帧分析接口）
 *  重要事情说三遍:所有检测结果返回的图片(outfn)和帧(outframe)大小,请按照原始输入图片(buffer)或帧(infn)的分辨率大小返回
 *  图片、视频单帧接口输入一次返回一次结果（图片、单帧文件保存路径和json)；视频接口输入视频实时返回视频结果保存在本地文件夹,分析完成,根据返回结果长度实例化json,打印所有报警帧json信息
 */

extern "C" {

/*
 refer to cv::Mat definition
 @单帧分析输入帧定义
 */
typedef struct {
	int rows; //Number of rows in a 2D array.
	int cols; //Number of columns in a 2D array.
	int type; //Array type. Use CV_8UC1, ..., CV_64FC4 to create 1-4 channel matrices, or CV_8UC(n), ..., CV_64FC(n) to create multi-channel (up to CV_CN_MAX channels) matrices.
	void * data; //Pointer to the user data.
	int step; //Number of bytes each matrix row occupies.
} JI_CV_FRAME;

/*
 event defination
 @算法检测器分析结果输出,根据算法实际使用场景由极视角相关工作人员定义
 */
typedef struct {
	int code; //event code
	char* json; //event
} JI_EVENT;

/*
 @可选项,初始化应用级插件,例如：log4cpp license等
 return:
 0:success
 -2: not used
 others : fail
 */
int ji_init(int argc, char** argv);

/*
 @创建检测器实例（必选项）
 creat instance and initiallized that can be used to analysis
 */
void *ji_create_predictor();

/*
 @释放检测器实例（必选项）
 destory instance
 */
void ji_destory_predictor(void* predictor);

/*
 @分析图片Buffer（图片处理必选项）
 analysis image buffer
 parameters:
 @para1：检测器实例
 @para2: 输入图片文件Buffer,统一用C++标准库进行图片到二进制流转换,自行参考ji_file2buffer(imgfile,buffer)函数,请勿用opencv等其它图像处理库函数对图片进行转换（自行管理图片缓存释放）
 @para3: 输入图片Buffer长度
 @para4: 可选项,图片感兴趣区域等绘制（规范请看 极市文档 BoostInterface ）
 @para5: 输出文件名称（自行管理图片缓存释放）（如果需要保存，需要在算法内部实现，传入的参数是一个绝对路径）
 @para6: 分析图片输出Json信息,主函数中释放JSON
 return :
 0:success
 -2: not used
 others : fail
 */
int ji_calc(void* predictor, const unsigned char* buffer, int length,
		const char* args, const char* outfn, char** json);

/*
 @分析图片文件（图片处理必选项）
 analysis image
 parameters:
 @para1：检测器实例
 @para2: 输入图片文件名称（传入的参数是一个绝对路径）
 @para3: 可选项,视频感兴趣区域的绘制（规范请看 极市文档 BoostInterface ）
 @para4: 输出文件名称（如果需要保存，需要在算法内部实现，传入的参数是一个绝对路径）
 @para5: 分析图片输出Json信息,主函数中释放JSON
 @调用此接口
 @return :
 0:success
 -2: not used
 others : fail
 */
int ji_calc_file(void* predictor, const char* infn, const char* args,
		const char* outfn, char** json);

/*
 analysis video file（视频处理必选项）
 @para1: 检测器实例
 @para2: 输入视频地址（传入的参数是一个绝对路径）
 @para3: 可选项,图片感兴趣区域等的绘制（规范请看 极市文档 BoostInterface ）
 @para4: 输出视频地址（如果需要保存，需要在算法内部实现，传入的参数是一个绝对路径）
 @para5: 分析视频Json信息,主函数中释放Json
 @return：
 0: success
 -2: not used
 others: fail
 */
int ji_calc_video_file(void* predictor, const char* infn, const char* args,
		const char* outfn, JI_EVENT* event);

/*
 analysis video frame（视频处理必选项）
 @para1: 检测器实例
 @para2: 输入单帧 （自行管理帧释放）
 @para3: 可选项,单帧分析感兴趣区域的绘制等（规范请看 极市文档 BoostInterface ）
 @para4: 输出单帧 （自行管理帧释放）（返回处理后的单帧，是否保存，调用者决定）
 @para5: 分析视频Json信息,主函数中释放Json
 @return：
 0: success
 -2: not used
 others: fail
 */
int ji_calc_video_frame(void* predictor, JI_CV_FRAME* inframe, const char* args,
		JI_CV_FRAME* outframe, JI_EVENT* event);

}
#endif
