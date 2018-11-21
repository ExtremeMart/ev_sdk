/*
 * BoostInterface.cpp
 *
 *  Created on: Sep 1, 2018
 *      Author: root
 */

#include "BoostInterface.h"

BoostInterface::BoostInterface() {
	// TODO Auto-generated constructor stub

}

BoostInterface::~BoostInterface() {
	// TODO Auto-generated destructor stub
}

void BoostInterface::parsePolygon(const std::string& input,
		cv::Size curSize, vector<cv::Point> &points) {
	boost::geometry::model::polygon<boost_point> ploygon;
	boost::geometry::read_wkt(input, ploygon);

	for (auto& item : ploygon.outer()) {
		cv::Point2f pointf = cv::Point2f(boost::geometry::get<0>(item),
				boost::geometry::get<1>(item));
		cv::Point point;
		//         if (pointf.x > 1.0)
		//            point = cv::Point(pointf.x * cur_size.width / 1280,
		//                             pointf.y * cur_size.height / 720);
		//         else
		point = cv::Point(pointf.x * curSize.width,
				pointf.y * curSize.height);
		//cout<<"X"<<point.x<<endl;
		//cout<<"Y"<<point.y<<endl;
		points.push_back(point);

	}

}

cv::Rect BoostInterface::ploygon2Rect(vector<cv::Point> points){
	//analysis_frame_size_ = analysis_frame_size;
	int min_x, min_y, max_x, max_y;
	min_x = min_y = std::numeric_limits<int>::max();
	max_x = max_y = std::numeric_limits<int>::min();

	for(int i = 0; i < points.size(); i++){
		if(points[i].x < min_x) min_x = points[i].x;
		if(points[i].x > max_x) max_x = points[i].x;
		if(points[i].y < min_y) min_y = points[i].y;
		if(points[i].y > max_y) max_y = points[i].y;
	}
	cv::Rect rect(min_x, min_y, max_x - min_x, max_y - min_y);

	return rect;

}

void BoostInterface::parsePoint(const std::string& input, cv::Size curSize, vector<cv::Point> &points) {
	boost_point point;
	boost::geometry::read_wkt(input, point);
	//    if (point.x() > 1.0)
	//    	//parameter wrong
	//    	points = cv::Point(point.x() * curSize.width / 1280,
	//                       point.y() * curSize.height / 720);
	//    else
	cv::Point pointResult = cv::Point((point.x() * curSize.width), (point.y() * curSize.height));
	points.push_back(pointResult);
}

void BoostInterface::parseLinestring(const std::string& input, cv::Size curSize, vector<cv::Point> &points) {
	boost::geometry::model::linestring<boost_point> linstring;
	boost::geometry::read_wkt(input, linstring);
	for(auto& item : linstring){
		cv::Point2f pointf = cv::Point2f(boost::geometry::get<0>(item), boost::geometry::get<1>(item));
		cv::Point pointResult;
		pointResult = cv::Point((pointf.x * curSize.width), (pointf.y * curSize.height));
		points.push_back(pointResult);
	}
}

