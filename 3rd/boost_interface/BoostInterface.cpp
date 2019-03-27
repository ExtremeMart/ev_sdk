/*
 * BoostInterface.cpp
 *
 *  Created on: Sep 1, 2018
 *      Author: root
 */

#include "BoostInterface.h"

typedef boost::geometry::model::d2::point_xy<double> Boost_Point;

void BoostInterface::parsePoint(const std::string& input, const cv::Size& curSize, std::vector<cv::Point>& points)
{
    Boost_Point point;
    boost::geometry::read_wkt(input, point);
    //points.push_back(cv::Point(point.x() * curSize.width, point.y() * curSize.height));
    points.emplace_back(point.x() * curSize.width, point.y() * curSize.height);    
}

void BoostInterface::parseLinestring(const std::string& input, const cv::Size& curSize, std::vector<cv::Point>& points)
{
    boost::geometry::model::linestring<Boost_Point> linstring;
    boost::geometry::read_wkt(input, linstring);

    for(auto& item : linstring)
    {
        points.emplace_back(item.x() * curSize.width, item.y() * curSize.height);
    }
}

void BoostInterface::parsePolygon(const std::string& input, const cv::Size& curSize, std::vector<cv::Point>& points)
{
    boost::geometry::model::polygon<Boost_Point> ploygon;
    boost::geometry::read_wkt(input, ploygon);

    for (auto& item : ploygon.outer())
    {
        points.emplace_back(item.x() * curSize.width, item.y() * curSize.height);
    }
}

cv::Rect BoostInterface::polygon2Rect(const std::vector<cv::Point>& points)
{
    int min_x, min_y, max_x, max_y;
    min_x = min_y = std::numeric_limits<int>::max();
    max_x = max_y = std::numeric_limits<int>::min();

    for(size_t i = 0; i < points.size(); i++)
    {
        if (points[i].x < min_x) min_x = points[i].x;
        if (points[i].x > max_x) max_x = points[i].x;
        if (points[i].y < min_y) min_y = points[i].y;
        if (points[i].y > max_y) max_y = points[i].y;
    }

    cv::Rect rect(min_x, min_y, max_x - min_x, max_y - min_y);
    return rect;
    }


