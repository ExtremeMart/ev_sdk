//
// Created by hrh on 2019-09-02.
//

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <glog/logging.h>
#include "SampleDetector.hpp"

SampleDetector::SampleDetector(double thresh, double nms, double hierThresh):
    mNetworkPtr(NULL), mNms(nms), mThresh(thresh), mHIERThresh(hierThresh) {
    LOG(INFO) << "Current config: nms:" << mNms << ", thresh:" << mThresh
            << ", HIERThresh:" << mHIERThresh;
}

int SampleDetector::init(char *namesFile, const char *modelCfgStr, char *weightsFile) {
    if (namesFile == nullptr || modelCfgStr == nullptr || weightsFile == nullptr) {
        LOG(ERROR) << "Invalid init args!";
        return ERROR_INVALID_INIT_ARGS;
    }

    LOG(INFO) << "Loading model...";
    mLabels = get_labels(namesFile);
    if (mLabels == nullptr) {
        LOG(ERROR) << "Failed getting labels from `" << namesFile << "`!";
        return SampleDetector::ERROR_INVALID_INIT_ARGS;
    }
    while (mLabels[mClasses] != nullptr) {
        mClasses++;
    }
    std::cout << "Num of Classes " << mClasses << std::endl;

    mNetworkPtr = load_network_from_string(modelCfgStr, weightsFile, 0);
    set_batch_network(mNetworkPtr, 1);
    srand(2222222);

    LOG(INFO) << "Done.";
    return SampleDetector::INIT_OK;
}

void SampleDetector::unInit() {
    if (mLabels) {
        for (int i = 0; i < mClasses; ++i) {
            if (mLabels[i]) {
                free(mLabels[i]);
                mLabels[i] = nullptr;
            }
        }
        free(mLabels);
        mLabels = nullptr;
    }
    if (mNetworkPtr) {
        free_network(mNetworkPtr);
        mNetworkPtr = nullptr;
    }
}

STATUS SampleDetector::processImage(const cv::Mat &cv_image, std::vector<Object> &result) {
    if (cv_image.empty()) {
        LOG(ERROR) << "Invalid input!";
        return ERROR_INVALID_INPUT;
    }
    // Read Image
    image im = mat_to_image(cv_image);
    // And scale it to the parameters define din *.cfg file.
    image sized = letterbox_image(im, mNetworkPtr->w, mNetworkPtr->h);

    // Uncomment this if you need sort predicted result.
    layer l = mNetworkPtr->layers[mNetworkPtr->n - 1];

    // Get actual data associated with test image.
    float *frame_data = sized.data;

    // Do prediction.
    double time = what_time_is_it_now();
    network_predict(mNetworkPtr, frame_data);

    // Get number fo predicted classes (objects).
    int num_boxes = 0;
    detection *detections = get_network_boxes(mNetworkPtr, im.w, im.h, mThresh, mHIERThresh, nullptr, 1, &num_boxes);

    // Uncomment this if you need sort predicted result.
    do_nms_sort(detections, num_boxes, l.classes, mNms);

    // Iterate over predicted classes and print information.
    for (int8_t i = 0; i < num_boxes; ++i) {
        for (uint8_t j = 0; j < mClasses; ++j) {
            if (detections[i].prob[j] > mThresh) {
                // More information is in each detections[i] item.
                // std::cout << mLabels[j] << " " << (int16_t) (detections[i].prob[j] * 100) << "%" << std::endl;
                cv::Rect rect;
                rect.width = detections[i].bbox.w * cv_image.cols;
                rect.height = detections[i].bbox.h * cv_image.rows;
                rect.x = detections[i].bbox.x * cv_image.cols - rect.width / 2;
                rect.y = detections[i].bbox.y * cv_image.rows - rect.height / 2;
                // 只输出检测到的目标`狗`
                if (std::string(mLabels[j]) == "dog") {
                    result.push_back({detections[i].prob[j], mLabels[j], rect});
                }
            }
        }
    }

    // Release resources
    free_image(im);
    free_image(sized);
    free_detections(detections, num_boxes);

    return SampleDetector::PROCESS_OK;
}

image SampleDetector::mat_to_image(const cv::Mat &m) {
    int h = m.size().height;
    int w = m.size().width;
    int c = m.channels();
    image im = make_image(w, h, c);
    unsigned char *data = m.data;
    int step = m.step;
    int i, j, k;

    for(i = 0; i < h; ++i){
        for(k= 0; k < c; ++k){
            for(j = 0; j < w; ++j){
                im.data[k*w*h + i*w + j] = data[i*step + j*c + k]/255.;
            }
        }
    }

    rgbgr_image(im);
    return im;
}

bool SampleDetector::setThresh(double thresh) {
    mThresh = thresh;
    return true;
}
