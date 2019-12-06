#ifndef BOOSTINTERFACE_H_
#define BOOSTINTERFACE_H_

#include <vector>
#include <boost/geometry/io/wkt/read.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <opencv2/opencv.hpp>


typedef std::vector<cv::Point> VectorPoint;
typedef std::vector<VectorPoint> VectorLine;
typedef std::vector<VectorPoint> VectorPolygon;

/**
 * WKTParser类用于解析使用WKT格式表示的字符串，并将这些字符串转换成点、线、多边形等易于使用的类对象，其功能包括：
 * 1. 用于解析WKT格式的点,线，框(多边形)
 * 2. 判断点或矩形是否在多边形区域内
 * WKT格式的详细定义请参考: https://en.wikipedia.org/wiki/Well-known_text_representation_of_geometry
 */
class WKTParser {
public:
    /**
     * 所有输入的坐标必须是归一化后在[0, 1]范围的点，解析之后，坐标会被size表示的宽高进行反归一化，例如：当点坐标为"POINT(0.1, 0.2)"，
     * size=(100, 200)时，反归一化后点坐标为(0.1*100, 0.2*200)=(10, 40)
     *
     * @param size 将坐标反归一化的尺寸，x轴将使用size.width反归一化，y轴将使用size.height反归一化
     */
    explicit WKTParser(const cv::Size &size);

    ~WKTParser();

    /**
     * 解析WKT点格式的字符串
     *
     * @param src 使用WKT格式表示的点字符串，例如："POINT(0.20 0.5)"
     * @param pp 当不为NULL时，存储解析后的点，使用cv::Point存储，由外部分配空间
     * @return 解析成功返回true，否则返回false
     */
    bool parsePoint(const std::string &src, cv::Point *pointPtr = nullptr);

    /**
     * 解析WKT线格式的字符串
     *
     * @param src 使用WKT格式表示的线字符串，例如："LINESTRING(0.20 0.5,0.6 0.8)"
     * @param pvp 当不为NULL时，存储解析后的点，由外部分配空间
     * @return 成功返回true，否则返回false
     */
    bool parseLinestring(const std::string &src, VectorPoint *pvp = nullptr);

    /**
     * 解析WKT格式表示的多边形字符串
     *
     * @param src 使用WKT格式表示的多边形字符串，例如："POLYGON((0.20 0.5,0.6 0.8,0.5 0.5 0.99 0.1))"
     * @param pvp 当不为NULL时，保存解析后的多边形，由外部分配空间
     * @return 成功返回true，否则返回false
     */
    bool parsePolygon(const std::string &src, VectorPoint *pvp = nullptr);

    /**
     * 判断点是否在m_polygons多表示的多个多边形中的某个多边形内部
     *
     * @param point 点
     * @return 如果point在m_polygons中的某个多边形内部返回true，否则返回false
     */
    bool inPolygons(const cv::Point &point);

    /**
     * 判断rect是否在m_polygons所表示的若干个多边形内部
     *
     * @param rect 矩形
     * @return 如果rect在m_polygons的某个多边形内部返回true，否则返回false
     */
    bool inPolygons(const cv::Rect &rect);

    /**
     * 获取WKTParser::parsePoint已经正确解析出的点
     *
     * @return  所有解析正确的点
     */
    const VectorPoint &getPoints() const { return m_points; }

    /**
     * 获取WKTParser::parseLinestring已经正确解析的线
     *
     * @return 所有解析正确的线
     */
    const VectorLine &getLines() const { return m_lines; }

    /**
     * 获取WKTParser::getPolygons已经正确解析的多边形
     *
     * @return 所有解析正确的多边形
     */
    const VectorPolygon &getPolygons() const { return m_polygons; }

    /**
     * 判断所解析的点、线、多边形是否有效是否有效
     *
     * @return 如果m_points、m_lines、m_polygons都不为空，则返回true，否则返回false
     */
    bool empty() const {
        return (m_size.empty() || (m_points.empty() && m_lines.empty() && m_polygons.empty()));
    }

public:
    /**
     * 将多边形polygon转换成矩形rect，转换后的矩形是在两边分别和坐标轴平行时的面积最小矩形
     *
     * @param polygon 由一系列点连接而成的多边形
     * @param rect 返回的矩形
     * @return 成功返回true，否则返回false
     */
    static bool polygon2Rect(const VectorPoint &polygon, cv::Rect &rect);

    /**
     * 判断点point是否在多边形polygon内部
     *
     * @param polygon 多边形
     * @param point 点
     * @return 如果point在polygon内部，返回true，否则返回false
     */
    static bool inPolygon(const VectorPoint &polygon, const cv::Point &point);

    /**
     * 判断矩形框rect是否则多边形polygon内部
     *
     * @param polygon 多边形
     * @param rect 矩形
     * @return 如果rect在polygon内部，返回true，否则返回false
     */
    static bool inPolygon(const VectorPoint &polygon, const cv::Rect &rect);

private:
    cv::Size m_size;    // 通常是图像的宽高，用于反归一化WKT格式的坐标值

    VectorPoint m_points;  // 存储正确解析的点
    VectorLine m_lines;    // 存储正确解析的线
    VectorPolygon m_polygons;  // 存储正确解析的多边形
};

#endif 
