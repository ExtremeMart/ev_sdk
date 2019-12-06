#ifndef JI_UTILS
#define JI_UTILS

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

/**
 * 获取文件大小
 *
 * @param ifs 打开的文件
 * @return 文件大小
 */
size_t getFileLen(std::ifstream &ifs);

/**
 * 在图上画矩形框，并在框顶部画文字
 *
 * @param img   需要画的图
 * @param leftTopRightBottomRect    矩形框(x, y, width, height)，其中(x, y)是左上角坐标，(width, height)是框的宽高
 * @param text  需要画的文字
 * @param rectLineThickness 矩形框的线宽度
 * @param rectLineType 矩形框的线类型，当值小于0时，将使用颜色填充整个矩形框
 * @param rectColor    矩形框的颜色
 * @param alpha     矩形框的透明度，范围[0,1]
 * @param fontHeight    字体高度
 * @param textColor 字体颜色，BGR格式
 * @param textBg    字体背景颜色，BGR格式
 */
void drawRectAndText(cv::Mat &img, cv::Rect &leftTopRightBottomRect, const std::string &text, int rectLineThickness,
                     int rectLineType, cv::Scalar rectColor, float alpha,
                     int fontHeight, cv::Scalar textColor, cv::Scalar textBg);

/**
 * 在输入图img上画多边形框
 *
 * @param img   输入图
 * @param polygons  多边形数组，每个多边形由顺时针连接的点构成
 * @param color     多边形框的颜色，BGR格式
 * @param alpha     多边形框的透明度，范围[0,1]
 * @param lineType  多边形框的线类型
 * @param thickness 多边形框的宽度
 * @param isFill    是否使用颜色填充roi区域
 */
void
drawPolygon(cv::Mat &img, std::vector<std::vector<cv::Point> > polygons, const cv::Scalar &color, float alpha, int lineType,
            int thickness = 3, bool isFill = false);

/**
 * 在img上画文字text
 *
 * @param img   输入图
 * @param text  文字
 * @param fontHeight    文字大小
 * @param fgColor   文字颜色，BGR格式
 * @param bgColor   文字背景颜色，BGR格式
 * @param leftTop   所画文字的左上顶点所在位置
 */
void
drawText(cv::Mat &img, const std::string &text, int fontHeight, const cv::Scalar &fgColor, const cv::Scalar &bgColor,
         const cv::Point &leftTopShift);

#endif  // JI_UTILS