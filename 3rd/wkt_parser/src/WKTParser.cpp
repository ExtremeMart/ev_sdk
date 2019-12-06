#include "WKTParser.h"
#include <iostream>
#include <exception>

typedef boost::geometry::model::d2::point_xy<double> Boost_Point;

WKTParser::WKTParser(const cv::Size &size) : m_size(size) {

}

WKTParser::~WKTParser() {

}

bool WKTParser::parsePoint(const std::string &src, cv::Point *pointPtr) {
    Boost_Point bp;
    try {
        boost::geometry::read_wkt(src, bp);
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    m_points.emplace_back(bp.x() * m_size.width, bp.y() * m_size.height);

    if (pointPtr) {
        pointPtr->x = bp.x() * m_size.width;
        pointPtr->y = bp.y() * m_size.height;
    }

    return true;
}

bool WKTParser::parseLinestring(const std::string &src, VectorPoint *pvp) {
    boost::geometry::model::linestring<Boost_Point> linstring;
    try {
        boost::geometry::read_wkt(src, linstring);
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    VectorPoint vp;
    for (auto &item : linstring) {
        vp.emplace_back(item.x() * m_size.width, item.y() * m_size.height);
    }
    m_lines.emplace_back(vp);

    if (pvp) {
        *pvp = vp;
    }

    return true;
}

bool WKTParser::parsePolygon(const std::string &src, VectorPoint *pvp) {
    boost::geometry::model::polygon<Boost_Point> ploygon;
    try {
        boost::geometry::read_wkt(src, ploygon);
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    VectorPoint vp;
    for (auto &item : ploygon.outer()) {
        vp.emplace_back(item.x() * m_size.width, item.y() * m_size.height);
    }
    m_polygons.emplace_back(vp);

    if (pvp) {
        *pvp = vp;
    }

    return true;
}

bool WKTParser::inPolygons(const cv::Point &point) {
    if (empty()) return false;

    for (auto iter = m_polygons.cbegin(); iter != m_polygons.cend(); iter++) {
        if (WKTParser::inPolygon(*iter, point)) {
            return true;
        }
    }

    return false;
}

bool WKTParser::inPolygons(const cv::Rect &rect) {
    if (empty()) return false;

    for (auto iter = m_polygons.cbegin(); iter != m_polygons.cend(); iter++) {
        if (WKTParser::inPolygon(*iter, rect)) {
            return true;
        }
    }

    return false;
}

bool WKTParser::polygon2Rect(const VectorPoint &polygon, cv::Rect &rect) {
    int min_x, min_y, max_x, max_y;
    min_x = min_y = std::numeric_limits<int>::max();
    max_x = max_y = std::numeric_limits<int>::min();

    for (size_t i = 0; i < polygon.size(); i++) {
        if (polygon[i].x < min_x) min_x = polygon[i].x;
        if (polygon[i].x > max_x) max_x = polygon[i].x;
        if (polygon[i].y < min_y) min_y = polygon[i].y;
        if (polygon[i].y > max_y) max_y = polygon[i].y;
    }

    rect.x = min_x;
    rect.y = min_y;
    rect.width = max_x - min_x;
    rect.height = max_y - min_y;

    return true;
}

bool WKTParser::inPolygon(const VectorPoint &polygon, const cv::Point &point) {
    cv::Point2f pf(point.x, point.y);
    return (cv::pointPolygonTest(polygon, pf, false) >= 0);
}

bool WKTParser::inPolygon(const VectorPoint &polygon, const cv::Rect &rect) {
    return (WKTParser::inPolygon(polygon, rect.tl()) &&
            WKTParser::inPolygon(polygon, cv::Point(rect.x + rect.width, rect.y)) &&
            WKTParser::inPolygon(polygon, rect.br()) &&
            WKTParser::inPolygon(polygon, cv::Point(rect.x, rect.y + rect.height)));
}
