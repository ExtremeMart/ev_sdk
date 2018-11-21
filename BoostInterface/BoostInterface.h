/*
 * BoostInterface.h
 *
 *  Created on: Sep 1, 2018
 *      Author: root
 */

#ifndef BOOSTINTERFACE_H_
#define BOOSTINTERFACE_H_



#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/io/wkt/read.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

typedef boost::geometry::model::d2::point_xy<double> boost_point;
class BoostInterface {
public:
	BoostInterface();
	virtual ~BoostInterface();
	cv::Rect ploygon2Rect(vector<cv::Point> points);
	//parsePolygon input格式
	void parsePolygon(const std::string& input,
			cv::Size curSize, vector<cv::Point> &points);
	//parsePoint input格式
	void parsePoint(const std::string& input, cv::Size curSize, vector<cv::Point> &points);
	//parseLinestring input格式
	void parseLinestring(const std::string& input, cv::Size curSize, vector<cv::Point> &points);
};
#endif /* BOOSTINTERFACE_H_ */
