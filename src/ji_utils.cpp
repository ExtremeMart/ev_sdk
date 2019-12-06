#include <ji_utils.h>
#include <opencv2/freetype.hpp>
#include <iostream>
#include <fstream>

size_t getFileLen(std::ifstream &ifs) {
    int origPos = ifs.tellg();
    ifs.seekg(0, std::fstream::end);
    size_t len = ifs.tellg();
    ifs.seekg(origPos);
    return len;
}

void drawRectAndText(cv::Mat &img, cv::Rect &leftTopRightBottomRect, const std::string &text, int rectLineThickness,
                     int rectLineType, cv::Scalar rectColor, float rectAlpha, int fontHeight, cv::Scalar textColor,
                     cv::Scalar textBg) {
    cv::Mat originalData;
    if (rectAlpha < 1.0f && rectAlpha > 0.0f) {
        img.copyTo(originalData);
    }
    // Draw rectangle
    cv::Point rectLeftTop(leftTopRightBottomRect.x, leftTopRightBottomRect.y);
    cv::rectangle(img, leftTopRightBottomRect, rectColor, rectLineThickness, rectLineType, 0);

    // Draw text and text background
    cv::Ptr<cv::freetype::FreeType2> ft2;
    int baseline = 0;
    ft2 = cv::freetype::createFreeType2();
    ft2->loadFontData("/usr/local/ev_sdk/lib/fonts/DejaVuSans.ttf", 0);

    cv::Size textSize = ft2->getTextSize(text, fontHeight, -1, &baseline);
    cv::Point textLeftBottom(leftTopRightBottomRect.x, leftTopRightBottomRect.y);
    textLeftBottom -= cv::Point(0, rectLineThickness);
    textLeftBottom -= cv::Point(0, baseline);   // (left, bottom) of text
    cv::Point textLeftTop(textLeftBottom.x, textLeftBottom.y - textSize.height);    // (left, top) of text
    // Draw text background
    cv::rectangle(img, textLeftTop, textLeftTop + cv::Point(textSize.width, textSize.height + baseline), textBg,
                  cv::FILLED);
    // Draw text
    ft2->putText(img, text, textLeftBottom, fontHeight, textColor, -1, cv::LINE_AA, true);

    if (!originalData.empty()) {    // Need to transparent drawing with alpha
        cv::addWeighted(originalData, rectAlpha, img, (1 - rectAlpha), 0, img);
    }
}

void drawPolygon(cv::Mat &img, std::vector<std::vector<cv::Point> > polygons, const cv::Scalar &color, float alpha,
                 int lineType, int thickness, bool isFill) {
    cv::Mat originalData;
    bool fill = (isFill && alpha < 1.0f && alpha > 0.0f);
    if (fill) {
        img.copyTo(originalData);
    }
    for (size_t i = 0; i < polygons.size(); i++) {
        const cv::Point *pPoint = &polygons[i][0];
        int n = (int) polygons[i].size();
        if (fill) {
            cv::fillPoly(img, &pPoint, &n, 1, color, lineType);
        } else {
            cv::polylines(img, &pPoint, &n, 1, true, color, thickness, lineType);
        }
    }
    if (!originalData.empty()) { // Transparent drawing
        cv::addWeighted(originalData, alpha, img, (1 - alpha), 0, img);
    }
}

void drawText(cv::Mat &img, const std::string &text, int fontHeight, const cv::Scalar &fgColor,
        const cv::Scalar &bgColor, const cv::Point &leftTopShift) {
    if (text.empty()) {
        printf("text cannot be empty!\n");
        return;
    }

    cv::Ptr<cv::freetype::FreeType2> ft2;
    int baseline = 0;
    ft2 = cv::freetype::createFreeType2();
    ft2->loadFontData("/usr/local/ev_sdk/lib/fonts/DejaVuSans.ttf", 0);
    cv::Size textSize = ft2->getTextSize(text, fontHeight, -1, &baseline);
    cv::Point textLeftBottom(0, textSize.height);
    textLeftBottom -= cv::Point(0, baseline);   // (left, bottom) of text
    cv::Point textLeftTop(textLeftBottom.x, textLeftBottom.y - textSize.height);    // (left, top) of text
    // Draw text background
    textLeftTop += leftTopShift;
    cv::rectangle(img, textLeftTop, textLeftTop + cv::Point(textSize.width, textSize.height + baseline), bgColor,
                  cv::FILLED);
    textLeftBottom += leftTopShift;
    ft2->putText(img, text, textLeftBottom, fontHeight, fgColor, -1, cv::LINE_AA, true);
}

