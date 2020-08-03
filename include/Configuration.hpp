#ifndef JI_ALGORITHM_CONFIGURATION
#define JI_ALGORITHM_CONFIGURATION
#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "cJSON.h"
#include "SampleDetector.hpp"

using ALGO_CONFIG_TYPE = typename SampleDetector::ALGO_CONFIG_TYPE;

#define BGRA_CHANNEL_SIZE 4

/**
 * @brief 存储和更新配置参数
 */
struct Configuration {
    typedef cv::Scalar COLOR_BGRA_TYPE;

    // 算法与画图的可配置参数及其默认值
    // 1. 算法配置参数
    std::map<std::string, ALGO_CONFIG_TYPE> mAlgoConfigs;   // 针对不同的cid（即camera id）所对应的算法配置
    ALGO_CONFIG_TYPE mAlgoConfigDefault = {0.6, 0.5, 0.5};     // 默认的算法配置

    // 2. roi配置
    std::vector<cv::Rect> currentROIRects; // 矩形roi区域
    std::vector<VectorPoint> currentROIOrigPolygons;    // Original roi polygons
    std::vector<std::string> origROIArgs;
    cv::Size currentInFrameSize{0, 0};  // 当前处理帧的尺寸

    // 3. 画图相关的配置
    bool drawROIArea = false;   // 是否画ROI
    COLOR_BGRA_TYPE roiColor = {120, 120, 120, 1.0f};  // ROI框的颜色
    int roiLineThickness = 4;   // ROI框的粗细
    bool roiFill = false;       // 是否使用颜色填充ROI区域
    bool drawResult = true;         // 是否画检测框
    bool drawConfidence = false;    // 是否画置信度
    int dogRectLineThickness = 4;   // 目标框粗细

    std::string dogRectText{"dog"};    // 检测目标框顶部文字
    COLOR_BGRA_TYPE dogRectColor = {0, 255, 0, 1.0f};      // 检测框`mark`的颜色
    COLOR_BGRA_TYPE textFgColor = {0, 0, 0, 0};         // 检测框顶部文字的颜色
    COLOR_BGRA_TYPE textBgColor = {255, 255, 255, 0};   // 检测框顶部文字的背景颜色
    int dogTextHeight = 30;  // 目标框顶部字体大小

    bool drawWarningText = true;
    int warningTextSize = 40;   // 画到图上的报警文字大小
    std::string warningText{"WARNING!"};    // 画到图上的报警文字
    COLOR_BGRA_TYPE warningTextFg = {255, 255, 255, 0}; // 报警文字颜色
    COLOR_BGRA_TYPE warningTextBg = {0, 0, 255, 0}; // 报警文字背景颜色
    cv::Point warningTextLeftTop{0, 0}; // 报警文字左上角位置

    /**
     * @brief 解析json格式的配置参数
     *
     * @param[in] configStr json格式的配置参数字符串
     * @return 当前参数解析后，生成的算法相关配置参数
     */
    ALGO_CONFIG_TYPE parseAndUpdateArgs(const char *confStr) {
        if (confStr == nullptr) {
            return mAlgoConfigDefault;
        }

        cJSON *confObj = cJSON_Parse(confStr);
        if (confObj == nullptr) {
            LOG(ERROR) << "Failed parsing `" << confStr << "`";
            return mAlgoConfigDefault;
        }
        cJSON *drawROIObj = cJSON_GetObjectItem(confObj, "draw_roi_area");
        if (drawROIObj != nullptr && (drawROIObj->type == cJSON_True || drawROIObj->type == cJSON_False)) {
            drawROIArea = drawROIObj->valueint;
        }
        cJSON *roiColorRootObj = cJSON_GetObjectItem(confObj, "roi_color");
        if (roiColorRootObj != nullptr && roiColorRootObj->type == cJSON_Array) {
            getBGRAColor(roiColor, roiColorRootObj);
        }
        cJSON *roiThicknessObj = cJSON_GetObjectItem(confObj, "roi_line_thickness");
        if (roiThicknessObj != nullptr && roiThicknessObj->type == cJSON_Number) {
            roiLineThickness = roiThicknessObj->valueint;
        }
        cJSON *roiFillObj = cJSON_GetObjectItem(confObj, "roi_fill");
        if (roiFillObj != nullptr && (roiFillObj->type == cJSON_True || roiFillObj->type == cJSON_False)) {
            roiFill = roiFillObj->valueint;
        }
        cJSON *roiArrObj = cJSON_GetObjectItem(confObj, "roi");
        if (roiArrObj != nullptr && roiArrObj->type == cJSON_Array && cJSON_GetArraySize(roiArrObj) > 0) {
            std::vector<std::string> roiStrs;
            for (int i = 0; i < cJSON_GetArraySize(roiArrObj); ++i) {
                cJSON *roiObj = cJSON_GetArrayItem(roiArrObj, i);
                if (roiObj == nullptr || roiObj->type != cJSON_String) {
                    continue;
                }
                roiStrs.emplace_back(std::string(roiObj->valuestring));
            }
            if (!roiStrs.empty()) {
                origROIArgs = roiStrs;
                updateROIInfo(currentInFrameSize.width, currentInFrameSize.height);
            }
        }
        cJSON *drawResultObj = cJSON_GetObjectItem(confObj, "draw_result");
        if (drawResultObj != nullptr && (drawResultObj->type == cJSON_True || drawResultObj->type == cJSON_False)) {
            drawResult = drawResultObj->valueint;
        }
        cJSON *drawConfObj = cJSON_GetObjectItem(confObj, "draw_confidence");
        if (drawConfObj != nullptr && (drawConfObj->type == cJSON_True || drawConfObj->type == cJSON_False)) {
            drawConfidence = drawConfObj->valueint;
        }

        cJSON *markTextObj = cJSON_GetObjectItem(confObj, "mark_text");
        if (markTextObj != nullptr && markTextObj->type == cJSON_String) {
            dogRectText = markTextObj->valuestring;
        }
        cJSON *textFgColorRootObj = cJSON_GetObjectItem(confObj, "object_text_color");
        if (textFgColorRootObj != nullptr && textFgColorRootObj->type == cJSON_Array) {
            getBGRAColor(textFgColor, textFgColorRootObj);
        }
        cJSON *textBgColorRootObj = cJSON_GetObjectItem(confObj, "object_text_bg_color");
        if (textBgColorRootObj != nullptr && textBgColorRootObj->type == cJSON_Array) {
            getBGRAColor(textBgColor, textBgColorRootObj);
        }
        cJSON *objectRectLineThicknessObj = cJSON_GetObjectItem(confObj, "object_rect_line_thickness");
        if (objectRectLineThicknessObj != nullptr && objectRectLineThicknessObj->type == cJSON_Number) {
            dogRectLineThickness = objectRectLineThicknessObj->valueint;
        }
        cJSON *markRectColorObj = cJSON_GetObjectItem(confObj, "dog_rect_color");
        if (markRectColorObj != nullptr && markRectColorObj->type == cJSON_Array) {
            getBGRAColor(dogRectColor, markRectColorObj);
        }

        cJSON *markTextSizeObj = cJSON_GetObjectItem(confObj, "object_text_size");
        if (markTextSizeObj != nullptr && markTextSizeObj->type == cJSON_Number) {
            dogTextHeight = markTextSizeObj->valueint;
        }

        cJSON *drawWarningTextObj = cJSON_GetObjectItem(confObj, "draw_warning_text");
        if (drawWarningTextObj != nullptr && (drawWarningTextObj->type == cJSON_True || drawWarningTextObj->type == cJSON_False)) {
            drawWarningText = drawWarningTextObj->valueint;
        }

        cJSON *warningTextSizeObj = cJSON_GetObjectItem(confObj, "warning_text_size");
        if (warningTextSizeObj != nullptr && warningTextSizeObj->type == cJSON_Number) {
            warningTextSize = warningTextSizeObj->valueint;
        }

        cJSON *warningTextObj = cJSON_GetObjectItem(confObj, "warning_text");
        if (warningTextObj != nullptr && warningTextObj->type == cJSON_String) {
            warningText = warningTextObj->valuestring;
        }
        cJSON *warningTextFgObj = cJSON_GetObjectItem(confObj, "warning_text_color");
        if (warningTextFgObj != nullptr && warningTextFgObj->type == cJSON_Array) {
            getBGRAColor(warningTextFg, warningTextFgObj);
        }
        cJSON *warningTextBgObj = cJSON_GetObjectItem(confObj, "warning_text_bg_color");
        if (warningTextBgObj != nullptr && warningTextBgObj->type == cJSON_Array) {
            getBGRAColor(warningTextBg, warningTextBgObj);
        }

        cJSON *warningTextLefTopObj = cJSON_GetObjectItem(confObj, "warning_text_left_top");
        if (warningTextLefTopObj != nullptr && warningTextLefTopObj->type == cJSON_Array) {
            cJSON *leftObj = cJSON_GetArrayItem(warningTextLefTopObj, 0);
            cJSON *topObj = cJSON_GetArrayItem(warningTextLefTopObj, 0);
            if (leftObj != nullptr && leftObj->type == cJSON_Number) {
                warningTextLeftTop.x = leftObj->valueint;
            }
            if (topObj != nullptr && topObj->type == cJSON_Number) {
                warningTextLeftTop.y = topObj->valueint;
            }
        }

        // 针对不同的cid获取算法配置参数，如果没有cid参数，就使用默认的配置参数
        ALGO_CONFIG_TYPE algoConfig{mAlgoConfigDefault.nms, mAlgoConfigDefault.thresh, mAlgoConfigDefault.hierThresh};
        bool isNeedUpdateThresh = false;
        float newThresh = algoConfig.thresh;
        cJSON *threshObj = cJSON_GetObjectItem(confObj, "thresh");
        if (threshObj != nullptr && threshObj->type == cJSON_Number) {
            newThresh = threshObj->valuedouble;     // 获取默认的阈值
            isNeedUpdateThresh = true;
            algoConfig.thresh = newThresh;
        }
        cJSON *cidObj = cJSON_GetObjectItem(confObj, "cid");
        if (cidObj != nullptr && cidObj->type == cJSON_String) {
            // 如果能够找到cid，当前配置就针对对应的cid进行更改
            if (mAlgoConfigs.find(cidObj->valuestring) == mAlgoConfigs.end()) {
                mAlgoConfigs.emplace(std::make_pair(cidObj->valuestring, mAlgoConfigDefault));
            }
            if (isNeedUpdateThresh) {
                mAlgoConfigs[cidObj->valuestring].thresh = newThresh;
            }
            algoConfig = mAlgoConfigs[cidObj->valuestring];
        }

        cJSON_Delete(confObj);
        return algoConfig;
    }
    /**
     * @brief 当输入图片尺寸变更时，更新ROI
     **/
    void updateROIInfo(int newWidth, int newHeight) {
        currentInFrameSize.width = newWidth;
        currentInFrameSize.height = newHeight;
        currentROIOrigPolygons.clear();
        currentROIRects.clear();

        VectorPoint currentFramePolygon;
        currentFramePolygon.emplace_back(cv::Point(0, 0));
        currentFramePolygon.emplace_back(cv::Point(currentInFrameSize.width, 0));
        currentFramePolygon.emplace_back(cv::Point(currentInFrameSize.width, currentInFrameSize.height));
        currentFramePolygon.emplace_back(cv::Point(0, currentInFrameSize.height));

        WKTParser wktParser(cv::Size(newWidth, newHeight));
        for (auto &roiStr : origROIArgs) {
            LOG(WARNING) << "parsing roi:" << roiStr;
            VectorPoint polygon;
            wktParser.parsePolygon(roiStr, &polygon);
            bool isPolygonValid = true;
            for (auto &point : polygon) {
                if (!wktParser.inPolygon(currentFramePolygon, point)) {
                    LOG(ERROR) << "point " << point << " not in polygon!";
                    isPolygonValid = false;
                    break;
                }
            }
            if (!isPolygonValid || polygon.empty()) {
                LOG(ERROR) << "roi `" << roiStr << "` not valid! skipped!";
                continue;
            }
            currentROIOrigPolygons.emplace_back(polygon);
        }
        if (currentROIOrigPolygons.empty()) {
            currentROIOrigPolygons.emplace_back(currentFramePolygon);
            LOG(WARNING) << "Using the whole image as roi!";
        }

        for (auto &roiPolygon : currentROIOrigPolygons) {
            cv::Rect rect;
            wktParser.polygon2Rect(roiPolygon, rect);
            currentROIRects.emplace_back(rect);
        }
    }

    /**
     * @brief 从cJSON数组中获取BGR的三个通道值，并填充到color数组中
     *
     * @param[out] color 填充后的数组
     * @param[in] bgraArr 存储有BGRA值的cJSON数组
     */
    static void getBGRAColor(COLOR_BGRA_TYPE &color, cJSON *bgraArr) {
        if (bgraArr == nullptr || bgraArr->type != cJSON_Array || cJSON_GetArraySize(bgraArr) != BGRA_CHANNEL_SIZE) {
            LOG(ERROR) << "Invalid BGRA value!";
            return;
        }
        for (int i = 0; i < BGRA_CHANNEL_SIZE; ++i) {
            cJSON *channelObj = cJSON_GetArrayItem(bgraArr, i);
            color[i] = channelObj->valuedouble;
        }
    }
};

#endif