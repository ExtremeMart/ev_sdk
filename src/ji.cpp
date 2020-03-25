/**
 * 示例代码：实现ji.h定义的图像接口，开发者需要根据自己的实际需求对接口进行实现
 */

#include <cstdlib>
#include <cstring>
#include <fstream>

#include <opencv2/opencv.hpp>
#include <glog/logging.h>

#include "encrypt_wrapper.hpp"
#include "ji_license.h"
#include "ji_license_impl.h"
#include "WKTParser.h"
#include "cJSON.h"
#include <ji_utils.h>
#include "ji.h"

#include "pubKey.hpp"
#include "model_str.hpp"
#include "SampleDetector.hpp"

#define JSON_ALERT_FLAG_KEY ("alert_flag")
#define JSON_ALERT_FLAG_TRUE 1
#define JSON_ALERT_FLAG_FALSE 0

// 如果需要添加授权功能，请保留该宏定义，并在ji_init中实现授权校验
#define ENABLE_JI_AUTHORIZATION
// 如果需要加密模型，请保留该宏定义，并在ji_create_predictor中实现模型解密
#define ENABLE_JI_MODEL_ENCRYPTION

#ifndef EV_SDK_DEBUG
#define EV_SDK_DEBUG 1
#endif
const int BGRA_CHANNEL_SIZE = 4;
typedef float COLOR_BGRA_TYPE[BGRA_CHANNEL_SIZE];
cv::Mat outputFrame;        // 用于存储算法处理后的输出图像，根据ji.h的接口规范，接口实现需要负责释放该资源
char *jsonResult = nullptr; // 用于存储算法处理后输出到JI_EVENT的json字符串，根据ji.h的接口规范，接口实现需要负责释放该资源

// 算法与画图的可配置参数及其默认值
typedef struct {
    // 算法配置可选的配置参数
    double nms;
    double thresh;
    double hierThresh;
} ALGO_CONFIG_TYPE;

std::map<std::string, ALGO_CONFIG_TYPE> mAlgoConfigs;   // 针对不同的cid（即camera id）所对应的算法配置
ALGO_CONFIG_TYPE mAlgoConfigDefault = {0.6, 0.5, 0.5};     // 默认的算法配置，当没有传入cid时使用

bool drawROIArea = false;   // 是否画ROI
COLOR_BGRA_TYPE roiColor = {120, 120, 120, 1.0f};  // ROI框的颜色
int roiLineThickness = 4;   // ROI框的粗细
bool roiFill = false;       // 是否使用颜色填充ROI区域
bool drawResult = true;         // 是否画检测框
bool drawConfidence = false;    // 是否画置信度
int dogRectLineThickness = 4;   // 目标框粗细


std::string dogRectText("dog");    // 检测目标框顶部文字
COLOR_BGRA_TYPE dogRectColor = {0, 255, 0, 1.0f};      // 检测框`mark`的颜色
COLOR_BGRA_TYPE textFgColor = {0, 0, 0, 0};         // 检测框顶部文字的颜色
COLOR_BGRA_TYPE textBgColor = {255, 255, 255, 0};   // 检测框顶部文字的背景颜色
int dogTextHeight = 30;  // 目标框顶部字体大小

int warningTextSize = 40;   // 画到图上的报警文字大小
std::string warningText("WARNING!");    // 画到图上的报警文字
COLOR_BGRA_TYPE warningTextFg = {255, 255, 255, 0}; // 报警文字颜色
COLOR_BGRA_TYPE warningTextBg = {0, 0, 255, 0}; // 报警文字背景颜色
cv::Point warningTextLeftTop(0, 0); // 报警文字左上角位置

std::vector<cv::Rect> currentROIRects; // 矩形roi区域
std::vector<VectorPoint> currentROIOrigPolygons;    // Original roi polygons
std::vector<std::string> origROIArgs;
cv::Size currentInFrameSize(0, 0);  // 当前处理帧的尺寸

/**
 * 从cJSON数组中获取RGB的三个通道值，并填充到color数组中
 *
 * @param[out] color 填充后的数组
 * @param[in] bgraArr 存储有BGRA值的cJSON数组
 */
void getBGRAColor(COLOR_BGRA_TYPE &color, cJSON *rgbArr) {
    if (rgbArr == nullptr || rgbArr->type != cJSON_Array || cJSON_GetArraySize(rgbArr) != BGRA_CHANNEL_SIZE) {
        LOG(ERROR) << "Invalid RGBA value!";
        return;
    }
    for (int i = 0; i < BGRA_CHANNEL_SIZE; ++i) {
        cJSON *channelObj = cJSON_GetArrayItem(rgbArr, i);
        color[i] = channelObj->valuedouble;
    }
}

/**
 * 当输入图片尺寸变更时，更新ROI
 **/
void onInFrameSizeChanged(int newWidth, int newHeight) {
    LOG(INFO) << "on input frame size changed:(" << newWidth << ", " << newHeight << ")";
    if (newWidth <= 0 || newHeight <= 0) {
        return;
    }

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
        if (!isPolygonValid) {
            LOG(ERROR) << "roi " << roiStr << " not valid! skipped!";
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
 * 解析json格式的配置参数
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
    if (drawROIArea) {
        cJSON *roiColorRootObj = cJSON_GetObjectItem(confObj, "roi_color");
        if (roiColorRootObj != nullptr && roiColorRootObj->type == cJSON_Array) {
            getBGRAColor(roiColor, roiColorRootObj);
        }
        cJSON *roiThicknessObj = cJSON_GetObjectItem(confObj, "roi_line_thickness");
        if (roiThicknessObj != nullptr && roiThicknessObj->type == cJSON_Number) {
            roiLineThickness = roiThicknessObj->valueint;
        }
        cJSON *roiFillObj = cJSON_GetObjectItem(confObj, "roi_fill");
        if (roiThicknessObj != nullptr && (roiFillObj->type == cJSON_True || roiFillObj->type == cJSON_False)) {
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
                onInFrameSizeChanged(currentInFrameSize.width, currentInFrameSize.height);
            }
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
    ALGO_CONFIG_TYPE algoConfig{0.6, 0.5, 0.5};
    cJSON *threshObj = cJSON_GetObjectItem(confObj, "thresh");
    if (threshObj != nullptr && threshObj->type == cJSON_Number) {
        algoConfig.thresh = threshObj->valuedouble;     // 获取默认的阈值
    }
    cJSON *cidObj = cJSON_GetObjectItem(confObj, "cid");
    if (cidObj != nullptr && cidObj->type == cJSON_Number) {
        // 如果能够找到cid，当前配置就针对对应的cid进行更改
        if (mAlgoConfigs.empty()) {
            mAlgoConfigs.emplace(std::make_pair(cidObj->valuestring, algoConfig));
        } else if (mAlgoConfigs.find(cidObj->valuestring) != mAlgoConfigs.end()) {
            mAlgoConfigs[cidObj->valuestring] = algoConfig;
        } else {
            mAlgoConfigs.emplace(std::make_pair(cidObj->valuestring, algoConfig));
        }
    } else {
        // 如果没有找到cid，就改变默认的配置
        mAlgoConfigDefault = algoConfig;
    }

    cJSON_Delete(confObj);
    return algoConfig;
}

/**
 * 使用predictor对输入图像inFrame进行处理
 *
 * @param[in] predictor 算法句柄
 * @param[in] inFrame 输入图像
 * @param[in] args 处理当前输入图像所需要的输入参数，例如在目标检测中，通常需要输入ROI，由开发者自行定义和解析
 * @param[out] outFrame 输入图像，由内部填充结果，外部代码需要负责释放其内存空间
 * @param[out] event 以JI_EVENT封装的处理结果
 * @return 如果处理成功，返回JISDK_RET_SUCCEED
 */
int processMat(SampleDetector *detector, const cv::Mat &inFrame, const char* args, cv::Mat &outFrame, JI_EVENT &event) {
    // 处理输入图像
    if (inFrame.empty()) {
        return JISDK_RET_FAILED;
    }

#ifdef ENABLE_JI_AUTHORIZATION
    // 检查授权，统计QPS
    int ret = ji_check_expire();
    if (ret != JISDK_RET_SUCCEED) {
        switch (ret) {
            case EV_OVERMAXQPS:
                return JISDK_RET_OVERMAXQPS;
                break;
            case EV_OFFLINE:
                return JISDK_RET_OFFLINE;
                break;
            default:
                return JISDK_RET_UNAUTHORIZED;
        }
    }
#endif
    if (currentInFrameSize.width != inFrame.cols || currentInFrameSize.height != inFrame.rows) {
        onInFrameSizeChanged(inFrame.cols, inFrame.rows);
    }

    /**
     * 解析参数并更新，根据接口规范标准，接口必须支持配置文件/usr/local/ev_sdk/model/algo_config.json内参数的实时更新功能
     * （即通过ji_calc_*等接口传入）
     */
    ALGO_CONFIG_TYPE algoConfig = parseAndUpdateArgs(args);
    detector->setThresh(algoConfig.thresh);

    // 针对每个ROI进行算法处理
    std::vector<SampleDetector::Object> detectedObjects;
    std::vector<cv::Rect> detectedTargets;

    // 算法处理
    for (auto &roiRect : currentROIRects) {
        LOG(WARNING) << "current roi:" << roiRect;
        // Fix darknet save_image bug, image width and height should be divisible by 2
        if (roiRect.width % 2 != 0) {
            roiRect.width -= 1;
        }
        if (roiRect.height % 2 != 0) {
            roiRect.height -= 1;
        }

        cv::Mat cropedMatRef = inFrame(roiRect);
        cv::Mat cropedMat;
        cropedMatRef.copyTo(cropedMat);

        std::vector<SampleDetector::Object> objects;
        int processRet = detector->processImage(cropedMat, objects);
        if (processRet != SampleDetector::PROCESS_OK) {
            return JISDK_RET_FAILED;
        }
        for (auto &object : objects) {
            object.rect.x += roiRect.x;
            object.rect.y += roiRect.y;
            detectedObjects.emplace_back(object);
        }
    }

    // 此处示例业务逻辑：当算法检测到有`dog`时，就报警
    bool isNeedAlert = false;   // 是否需要报警
    std::vector<SampleDetector::Object> dogs;   // 检测到的`dog`

    // 创建输出图
    inFrame.copyTo(outFrame);
    // 画ROI区域
    if (drawROIArea && !currentROIOrigPolygons.empty()) {
        drawPolygon(outFrame, currentROIOrigPolygons, cv::Scalar(roiColor[0], roiColor[1], roiColor[2]), roiColor[3], cv::LINE_AA, roiLineThickness, roiFill);
    }
    // 判断是否要要报警并将检测到的目标画到输出图上
    for (auto &object : detectedObjects) {
        // 如果检测到有`狗`就报警
        if (strcmp(object.name.c_str(), "dog") == 0) {
            LOG(INFO) << "Found " << object.name;
            if (drawResult) {
                std::stringstream ss;
                ss << dogRectText;
                if (drawConfidence) {
                    ss.precision(2);
                    ss << std::fixed << (dogRectText.empty() ? "" : ": ") << object.prob * 100 << "%";
                }
                drawRectAndText(outFrame, object.rect, ss.str(), dogRectLineThickness, cv::LINE_AA,
                        cv::Scalar(dogRectColor[0], dogRectColor[1], dogRectColor[2]), dogRectColor[3], dogTextHeight,
                        cv::Scalar(textFgColor[0], textFgColor[1], textFgColor[2]),
                        cv::Scalar(textBgColor[0], textBgColor[1], textBgColor[2]));
            }

            isNeedAlert = true;
            dogs.push_back(object);
        }
    }

    if (isNeedAlert) {
        drawText(outFrame, warningText, warningTextSize,
                 cv::Scalar(warningTextFg[0], warningTextFg[1], warningTextFg[2]),
                 cv::Scalar(warningTextBg[0], warningTextBg[1], warningTextBg[2]), warningTextLeftTop);
    }

    // 将结果封装成json字符串
    cJSON *rootObj = cJSON_CreateObject();
    int jsonAlertCode = JSON_ALERT_FLAG_FALSE;
    if (isNeedAlert) {
        jsonAlertCode = JSON_ALERT_FLAG_TRUE;
    }
    cJSON_AddItemToObject(rootObj, JSON_ALERT_FLAG_KEY, cJSON_CreateNumber(jsonAlertCode));
    cJSON *dogsObj = cJSON_CreateArray();
    for (auto &dog : dogs) {
        cJSON *odbObj = cJSON_CreateObject();
        int xmin = dog.rect.x;
        int ymin = dog.rect.y;
        int xmax = xmin + dog.rect.width;
        int ymax = ymin + dog.rect.height;
        cJSON_AddItemToObject(odbObj, "xmin", cJSON_CreateNumber(xmin));
        cJSON_AddItemToObject(odbObj, "ymin", cJSON_CreateNumber(ymin));
        cJSON_AddItemToObject(odbObj, "xmax", cJSON_CreateNumber(xmax));
        cJSON_AddItemToObject(odbObj, "ymax", cJSON_CreateNumber(ymax));
        cJSON_AddItemToObject(odbObj, "confidence", cJSON_CreateNumber(dog.prob));

        cJSON_AddItemToArray(dogsObj, odbObj);
    }
    cJSON_AddItemToObject(rootObj, "dogs", dogsObj);

    char *jsonResultStr = cJSON_Print(rootObj);
    int jsonSize = strlen(jsonResultStr);
    if (jsonResult == nullptr) {
        jsonResult = new char[jsonSize + 1];
    } else if (strlen(jsonResult) < jsonSize) {
        free(jsonResult);   // 如果需要重新分配空间，需要释放资源
        jsonResult = new char[jsonSize + 1];
    }
    strcpy(jsonResult, jsonResultStr);

    // 注意：JI_EVENT.code需要根据需要填充，切勿弄反
    if (isNeedAlert) {
        event.code = JISDK_CODE_ALARM;
    } else {
        event.code = JISDK_CODE_NORMAL;
    }
    event.json = jsonResult;

    if (rootObj)
        cJSON_Delete(rootObj);
    if (jsonResultStr)
        free(jsonResultStr);

    return JISDK_RET_SUCCEED;
}

int ji_init(int argc, char **argv) {
    int authCode = JISDK_RET_SUCCEED;
#ifdef ENABLE_JI_AUTHORIZATION
    // 检查license参数
    if (argc < 6) {
        return JISDK_RET_INVALIDPARAMS;
    }

    if (argv[0] == NULL || argv[5] == NULL) {
        return JISDK_RET_INVALIDPARAMS;
    }

    int qps = 0;
    if (argv[4]) qps = atoi(argv[4]);

    // 使用公钥校验授权信息
    int ret = ji_check_license(pubKey, argv[0], argv[1], argv[2], argv[3], qps > 0 ? &qps : NULL, atoi(argv[5]));
    if (ret != EV_SUCCESS) {
        authCode = JISDK_RET_UNAUTHORIZED;
    }
#endif
    if (authCode != JISDK_RET_SUCCEED) {
        LOG(ERROR) << "ji_check_license failed!";
        return authCode;
    }

    return authCode;
}

void ji_reinit() {
#ifdef ENABLE_JI_AUTHORIZATION
    ji_check_license(NULL, NULL, NULL, NULL, NULL, NULL, 0);
#endif
    if (jsonResult) {
        free(jsonResult);
        jsonResult = nullptr;
    }
}


void *ji_create_predictor(int pdtype) {
#ifdef ENABLE_JI_AUTHORIZATION
    if (ji_check_expire_only() != EV_SUCCESS) {
        return nullptr;
    }
#endif

    // 从统一的配置文件读取配置参数，SDK实现必须支持从这个统一的配置文件中读取算法&业务逻辑相关的配置参数
    const char *configFile = "/usr/local/ev_sdk/config/algo_config.json";
    LOG(INFO) << "Parsing configuration file: " << configFile;

    std::ifstream confIfs(configFile);
    if (confIfs.is_open()) {
        size_t len = getFileLen(confIfs);
        char *confStr = new char[len + 1];
        confIfs.read(confStr, len);
        confStr[len] = '\0';

        parseAndUpdateArgs(confStr);
        delete[] confStr;
        confIfs.close();
    }

    auto *detector = new SampleDetector(mAlgoConfigDefault.thresh, mAlgoConfigDefault.nms, mAlgoConfigDefault.hierThresh);
    char *decryptedModelStr = nullptr;

#ifdef ENABLE_JI_MODEL_ENCRYPTION
    LOG(INFO) << "Decrypting model...";
    // 如果使用了模型加密功能，需要将加密后的模型（放在`model_str.hpp`内）进行解密
    void *h = CreateDecryptor(model_str.c_str(), model_str.size(), key.c_str());

    // 获取解密后的字符串
    int fileLen = 0;
    decryptedModelStr = (char *) FetchBuffer(h, fileLen);
    char *tmp = new char[fileLen + 1];
    strncpy(tmp, decryptedModelStr, fileLen);
    tmp[fileLen] = '\0';
    decryptedModelStr = tmp;
    LOG(INFO) << "Decrypted model size:" << strlen(decryptedModelStr);

    // 如何想要使用解密后的文件句柄，请调用这个接口
    // FILE *file = (file *) FetchFile(h);

    DestroyDecrtptor(h);
#else
    // 不使用模型加密功能，直接从模型文件读取
    std::ifstream ifs = std::ifstream("/usr/local/ev_sdk/model/yolov3-tiny.cfg", std::ios::binary);
    long len = getFileLen(ifs);
    decryptedModelStr = new char[len + 1];
    ifs.read(decryptedModelStr, len);
    decryptedModelStr[len] = '\0';
#endif

    int iRet = detector->init("/usr/local/ev_sdk/config/coco.names",
            decryptedModelStr,
            "/usr/local/ev_sdk/model/model.dat");
    if (decryptedModelStr != nullptr) {
        free(decryptedModelStr);
    }
    if (iRet != SampleDetector::INIT_OK) {
        return nullptr;
    }
    LOG(INFO) << "SamplePredictor init OK.";

    return detector;
}

void ji_destroy_predictor(void *predictor) {
    if (predictor == NULL) return;

    auto *detector = reinterpret_cast<SampleDetector *>(predictor);
    detector->unInit();
    delete detector;
}

int ji_calc_frame(void *predictor, const JI_CV_FRAME *inFrame, const char *args,
                  JI_CV_FRAME *outFrame, JI_EVENT *event) {
    if (predictor == NULL || inFrame == NULL) {
        return JISDK_RET_INVALIDPARAMS;
    }

    auto *detector = reinterpret_cast<SampleDetector *>(predictor);
    cv::Mat inMat(inFrame->rows, inFrame->cols, inFrame->type, inFrame->data, inFrame->step);
    if (inMat.empty()) {
        return JISDK_RET_FAILED;
    }
    cv::Mat outMat;
    int processRet = processMat(detector, inMat, args, outMat, *event);

    if (processRet == JISDK_RET_SUCCEED) {
        if ((event->code != JISDK_CODE_FAILED) && (!outMat.empty()) && (outFrame)) {
            outFrame->rows = outMat.rows;
            outFrame->cols = outMat.cols;
            outFrame->type = outMat.type();
            outFrame->data = outMat.data;
            outFrame->step = outMat.step;
        }
    }
    return processRet;
}

int ji_calc_buffer(void *predictor, const void *buffer, int length, const char *args, const char *outFile,
                   JI_EVENT *event) {
    if (predictor == NULL || buffer == NULL || length <= 0) {
        return JISDK_RET_INVALIDPARAMS;
    }

    auto *classifierPtr = reinterpret_cast<SampleDetector *>(predictor);

    const unsigned char *b = (const unsigned char *) buffer;
    std::vector<unsigned char> vecBuffer(b, b + length);
    cv::Mat inMat = cv::imdecode(vecBuffer, cv::IMREAD_COLOR);
    if (inMat.empty()) {
        return JISDK_RET_FAILED;
    }

    cv::Mat outMat;
    int processRet = processMat(classifierPtr, inMat, args, outMat, *event);

    if (processRet == JISDK_RET_SUCCEED) {
        if ((event->code != JISDK_CODE_FAILED) && (!outMat.empty()) && (outFile)) {
            cv::imwrite(outFile,outMat);
        }
    }
    return processRet;
}

int ji_calc_file(void *predictor, const char *inFile, const char *args, const char *outFile, JI_EVENT *event) {
    if (predictor == NULL || inFile == NULL) {
        return JISDK_RET_INVALIDPARAMS;
    }

    auto *classifierPtr = reinterpret_cast<SampleDetector *>(predictor);
    cv::Mat inMat = cv::imread(inFile);
    if (inMat.empty()) {
        return JISDK_RET_FAILED;
    }

    cv::Mat outMat;
    int processRet = processMat(classifierPtr, inMat, args, outMat, *event);
    if (processRet == JISDK_RET_SUCCEED) {
        if ((event->code != JISDK_CODE_FAILED) && (!outMat.empty()) && (outFile)) {
            cv::imwrite(outFile, outMat);
        }
    }

    return processRet;
}

int ji_calc_video_file(void *predictor, const char *infile, const char* args,
                       const char *outfile, const char *jsonfile) {
    // 没有实现的接口必须返回`JISDK_RET_UNUSED`
    if (predictor == NULL || infile == NULL) {
        return JISDK_RET_INVALIDPARAMS;
    }
    auto *classifierPtr = reinterpret_cast<SampleDetector *>(predictor);

    cv::VideoCapture videoCapture(infile);
    if (!videoCapture.isOpened()) {
        return JISDK_RET_FAILED;
    }

    cv::VideoWriter vwriter;
    cv::Mat inMat, outMat;
    JI_EVENT event;
    int iRet = JISDK_RET_FAILED;
    int totalFrames, alertFrames, timestamp;
    totalFrames = alertFrames = timestamp = 0;

    cJSON *jsonRoot, *jsonDetail;
    jsonRoot = jsonDetail = NULL;

    while (videoCapture.read(inMat)) {
        timestamp = videoCapture.get(cv::CAP_PROP_POS_MSEC);

        iRet = processMat(classifierPtr, inMat, args, outMat, event);

        if (iRet == JISDK_RET_SUCCEED) {
            ++totalFrames;

            if (event.code != JISDK_CODE_FAILED) {
                if (event.code == JISDK_CODE_ALARM) {
                    ++alertFrames;
                }

                if (!outMat.empty() && outfile) {
                    if (!vwriter.isOpened()) {
                        vwriter.open(outfile,
                                /*videoCapture.get(cv::CAP_PROP_FOURCC)*/cv::VideoWriter::fourcc('X', '2', '6', '4'),
                                     videoCapture.get(cv::CAP_PROP_FPS), outMat.size());
                        if (!vwriter.isOpened()) {
                            return JISDK_RET_FAILED;
                        }
                    }
                    vwriter.write(outMat);
                }

                if (event.json && jsonfile) {
                    if (jsonDetail == NULL) {
                        jsonDetail = cJSON_CreateArray();
                    }

                    cJSON *jsonFrame = cJSON_Parse(event.json);
                    if (jsonFrame) {
                        cJSON_AddItemToObjectCS(jsonFrame, "timestamp", cJSON_CreateNumber(timestamp));
                        cJSON_AddItemToArray(jsonDetail, jsonFrame);
                    }
                }
            }
        } else {
            break;
        }
    }

    if (iRet == JISDK_RET_SUCCEED) {
        if (jsonfile) {
            jsonRoot = cJSON_CreateObject();
            cJSON_AddItemToObjectCS(jsonRoot, "total_frames", cJSON_CreateNumber(totalFrames));
            cJSON_AddItemToObjectCS(jsonRoot, "alert_frames", cJSON_CreateNumber(alertFrames));

            if (jsonDetail) {
                cJSON_AddItemToObjectCS(jsonRoot, "detail", jsonDetail);
            }

            char *buff = cJSON_Print(jsonRoot);
            std::ofstream fs(jsonfile);
            if (fs.is_open()) {
                fs << buff;
                fs.close();
            }
            free(buff);
        }
    }

    if (jsonRoot) {
        cJSON_Delete(jsonRoot);
    } else if (jsonDetail) {
        cJSON_Delete(jsonDetail);
    }

    return iRet;
}