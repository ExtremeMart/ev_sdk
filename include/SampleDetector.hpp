//
// Created by hrh on 2019-09-02.
//

#ifndef JI_SAMPLEDETECTOR_HPP
#define JI_SAMPLEDETECTOR_HPP
#include <darknet.h>
#include <string>
#include <opencv2/core/mat.hpp>

#define STATUS int

/**
 * 使用darknet实现的目标分类器，分类类别使用imagenet1k数据
 */

class SampleDetector {

public:
    typedef struct {
        float prob;
        std::string name;
        cv::Rect rect;
    } Object;

    SampleDetector(double thresh, double nms, double hierThresh);

    /**
     * 初始化模型
     * @param[in] namesFile 检测物体的名字配置文件
     * @param[in] model_cfg_str 使用字符串表示的darknet模型
     * @param[in] weightfile darknet模型的权重文件路径
     * @return 如果初始化正常，INIT_OK
     */
    STATUS init(char *namesFile, const char *modelCfgStr, char *weightsFile);

    /**
     * 反初始化函数
     */
    void unInit();

    /**
     * 对cv::Mat格式的图片进行分类，并输出预测分数前top排名的目标名称到mProcessResult
     * @param[in] image 输入图片
     * @param[out] detectResults 检测到的结果
     * @return 如果处理正常，则返回PROCESS_OK，否则返回`ERROR_*`
     */
    STATUS processImage(const cv::Mat &image, std::vector<Object> &detectResults);

    /**
     * 将cv:Mat转换成darknet定义的格式
     * @param m
     * @return
     */
    static image mat_to_image(const cv::Mat &m);

    bool setThresh(double thresh);


public:
    static const int ERROR_BASE = 0x0100;
    static const int ERROR_INVALID_INPUT = 0x0101;
    static const int ERROR_INVALID_INIT_ARGS = 0x0102;

    static const int PROCESS_OK = 0x1001;
    static const int INIT_OK = 0x1002;

private:
    network* mNetworkPtr{nullptr};

    char *mProcessResult;

    char **mLabels = nullptr;

    double mThresh = 0.5;
    double mHIERThresh = 0.5;
    double mNms = 0.6;

    size_t mClasses = 0;
};

#endif //JI_SAMPLEDETECTOR_HPP
