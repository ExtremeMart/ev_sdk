/*
**
** 用于解析wkt格式的点,线，框(多边形)
** 判断点或矩形是否在多边形区域内
**
*/
#ifndef BOOSTINTERFACE_H_
#define BOOSTINTERFACE_H_

#include <vector>
#include <boost/geometry/io/wkt/read.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <opencv2/opencv.hpp>


typedef std::vector<cv::Point>    Vector_Point;
typedef std::vector<Vector_Point> Vector_Line;
typedef std::vector<Vector_Point> Vector_Polygon;

class BoostInterface 
{
public:
    explicit BoostInterface(const cv::Size &size);
    ~BoostInterface();


    /* 解析点,线,框(多边形)函数: parsePoint,parseLinestring,parsePolygon */
    /*
    ** 函数:   parsePoint
    ** 作用:   解析点信息
    ** 返回：成功返回true,并且会在m_points中增加该点信息；失败返回false
    ** 参数说明：
    **     src：输入参数，wkt格式。例如："POINT(0.20 0.5)"
    **     pp : 可选输入输出参数，传入有效指针用于存放解析出的点信息
    ** 其它: 无
    */
    bool parsePoint(const std::string   &src, cv::Point    *pp  = NULL);

    /*
    ** 函数:   parseLinestring
    ** 作用:   解析线信息
    ** 返回：成功返回true,并且会在m_lines中增加该行信息；失败返回false
    ** 参数说明：
    **     src：输入参数，wkt格式。例如："LINESTRING(0.20 0.5,0.6 0.8)"
    **     pvp: 可选输入输出参数，传入有效指针用于存放解析出的线信息
    ** 其它: 无
    */
    bool parseLinestring(const std::string    &src, Vector_Point *pvp = NULL);

    /*
    ** 函数:   parsePolygon
    ** 作用:   解析多边形信息
    ** 返回：成功返回true,并且会在m_polygons中增加该框信息；失败返回false
    ** 参数说明：
    **     src：输入参数，wkt格式。例如："POLYGON((0.20 0.5,0.6 0.8,0.5 0.5 0.99 0.1))"
    **     pvp: 可选输入输出参数，传入有效指针用于存放解析出的多边形信息
    ** 其它: 无
    */
    bool parsePolygon(const std::string &src, Vector_Point *pvp = NULL);

    /* 判断点或矩形是否在多边形区域内(可能多个): inPolygons*/
    bool inPolygons(const cv::Point &point);
    bool inPolygons(const cv::Rect &rect);

    /* 获取已正确解析出的点,线，框(多边形)信息 */
    const Vector_Point& getPoints() const      { return m_points;   }
    const Vector_Line& getLines()   const      { return m_lines;    }
    const Vector_Polygon& getPolygons() const  { return m_polygons; }

    /* 数据是否有效 */
    bool empty() const 
    {
        return (m_size.empty() || 
               (m_points.empty() && m_lines.empty() && m_polygons.empty()));
    }

public:
    /* 静态进阶函数 */
    static bool polygon2Rect(const Vector_Point &polygon, cv::Rect &rect);
    
    static bool inPolygon(const Vector_Point &polygon, const cv::Point &point);
    static bool inPolygon(const Vector_Point &polygon, const cv::Rect &rect);
    
private:
    /* 帧大小(分辨率) */
    cv::Size       m_size;

    /* 用于存放解析出的点，线，框(多边形区域) */
    Vector_Point   m_points;    
    Vector_Line    m_lines;
    Vector_Polygon m_polygons;
};

#endif 
