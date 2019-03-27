/*
 * BoostInterface.h
 *
 *  Created on: Sep 1, 2018
 *      Author: root
 */

#ifndef BOOSTINTERFACE_H_
#define BOOSTINTERFACE_H_

#include <vector>
#include <boost/geometry/io/wkt/read.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <opencv2/opencv.hpp>

//用于解析点,线，框
class BoostInterface {
public:
    BoostInterface()  {}
    virtual ~BoostInterface() {}

    void parsePoint(const std::string& input, const cv::Size& curSize, std::vector<cv::Point>& points);

    void parseLinestring(const std::string& input, const cv::Size& curSize, std::vector<cv::Point>& points);

    void parsePolygon(const std::string& input, const cv::Size& curSize, std::vector<cv::Point>& points);

    cv::Rect polygon2Rect(const std::vector<cv::Point>& points);
};
#endif /* BOOSTINTERFACE_H_ */
