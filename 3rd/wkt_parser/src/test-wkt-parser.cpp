#include <iostream>
#include "WKTParser.h"

using namespace std;

/**
 * 测试WKTParser的功能
 */

int main(int argc, char* argv[])
{

    WKTParser wktParser(cv::Size(640, 480));
    size_t i,j;
    
    // 解析点
    const char * cst_point_1 = "POINT(0.38 0.10)";
    const char * cst_point_2 = "POINT(0.47 0.41)";
    cv::Point point_1, point_2;
    wktParser.parsePoint(cst_point_1, &point_1);
    wktParser.parsePoint(cst_point_2, &point_2);
    cout << "test parsePoint ..." << 
    "\n\tcst_point_1: " << cst_point_1 << ", point_1(x,y): " << point_1.x << "," << point_1.y <<
    "\n\tcst_point_2: " << cst_point_2 << ", point_2(x,y): " << point_2.x << "," << point_2.y << endl;


    // 解析线段
    const char *cst_line = "LINESTRING(0.07 0.21,0.36 0.245,0.58 0.16,0.97 0.27)";
    VectorPoint line;
    wktParser.parseLinestring(cst_line, &line);
    cout << "test parseLinestring ..." << 
    "\n\tcst_line: " << cst_line << ", size: " << line.size() << endl;
    cout << "v_line info, size: " << line.size();
    for(i = 0; i < line.size(); i++)
    {
        cout << "\n\tpoint " << i+1 << "(x,y): " << line[i].x << "," << line[i].y;
    }
    cout << endl;


    // 解析多边形(框)
    const char *cst_polygon_1 = "POLYGON((0.048 0.357,0.166 0.0725,0.393 0.0075,0.392 0.202,0.242 0.375))";
    const char *cst_polygon_2 = "POLYGON((0.513 0.232,0.79 0.1075,0.928 0.102,0.953 0.64,0.759 0.89,0.51 0.245))";
    const char *cst_polygon_3 = "POLYGON((0.115 0.497,0.592 0.82,0.581 0.917,0.14 0.932))";
    VectorPoint polygon_1,polygon_2,polygon_3;
    wktParser.parsePolygon(cst_polygon_1, &polygon_1);
    wktParser.parsePolygon(cst_polygon_2, &polygon_2);
    wktParser.parsePolygon(cst_polygon_3, &polygon_3);
    cout << "test parsePolygon ..." <<
    "\n\tcst_polygon_1: " << cst_polygon_1 << ",size: " << polygon_1.size() <<
    "\n\tcst_polygon_2: " << cst_polygon_2 << ",size: " << polygon_2.size() <<
    "\n\tcst_polygon_3: " << cst_polygon_3 << ",size: " << polygon_3.size() << endl;

    cout << "polygon_1 info, size: " << polygon_1.size();
    for(i = 0; i < polygon_1.size(); i++)
    {
        cout << "\n\tpoint " << i+1 << "(x,y): " << polygon_1[i].x << "," << polygon_1[i].y;
    }
    cout << endl;

    cout << "polygon_2 info, size: " << polygon_2.size();
    for(i = 0; i < polygon_2.size(); i++)
    {
        cout << "\n\tpoint " << i+1 << "(x,y): " << polygon_2[i].x << "," << polygon_2[i].y;
    }
    cout << endl;

    cout << "v_polygon_3 info, size: " << polygon_3.size();
    for(i = 0; i < polygon_3.size(); i++)
    {
        cout << "\n\tpoint " << i+1 << "(x,y): " << polygon_3[i].x << "," << polygon_3[i].y;
    }
    cout << endl;


    // 判断点是否在多边形区域内
    if (wktParser.inPolygons(cv::Point(152, 96)))
    {
        cout << "inPolygons(cv::Point(152,96), return true." << endl;
    }
    else
    {
        cout << "inPolygons(cv::Point(152,96), return false." << endl;
    }


    // 判断矩形是否在多边形区域内
    if (wktParser.inPolygons(cv::Rect(140, 80, 20, 20)))
    {
        cout << "inPolygons(cv::Rect(150,90,20,20)), return true." << endl;
    }
    else 
    {
        cout << "inPolygons(cv::Rect(150,90,20,20)), return false." << endl;
    }

    // 判断矩形是否在多边形区域内
    if (wktParser.inPolygons(cv::Rect(150, 90, 200, 20)))
    {
        cout << "inPolygons(cv::Rect(150,90,200,20)), return true." << endl;
    }
    else 
    {
        cout << "inPolygons(cv::Rect(150,90,200,20)), return false." << endl;
    }

    // 输出wktParser内所有的点线框
    const VectorPoint &cf_points = wktParser.getPoints();
    const VectorLine  &cf_lines  = wktParser.getLines();
    const VectorPolygon &cf_polygons = wktParser.getPolygons();
    cout << "wktParser.m_points info, size: " << cf_points.size();
    i = 0;
    for (auto iter = cf_points.cbegin(); iter != cf_points.cend(); iter++)
    {
        cout << "\n\tpoint" << ++i << "(x,y): " << iter->x << "," << iter->y;
    }
    cout << endl;

    cout << "wktParser.m_lines info, size: " << cf_lines.size();
    i = 0;
    for (auto iter = cf_lines.cbegin(); iter != cf_lines.cend(); iter++)
    {
        cout << "\n\tline" << ++i <<", size: " << iter->size();
        j = 0;
        for (auto iter2 = iter->cbegin(); iter2 != iter->cend(); iter2++)
        {
            cout << "\n\t\tpoint" << ++j << "(x,y): " << iter2->x << "," << iter2->y;
        }
    }
    cout << endl;
    
    cout << "wktParser.m_polygons info, size: " << cf_polygons.size();
    i = 0;
    for (auto iter = cf_polygons.cbegin(); iter != cf_polygons.cend(); iter++)
    {
        cout << "\n\tpolygon" << ++i <<", size: " << iter->size();
        j = 0;
        for (auto iter2 = iter->cbegin(); iter2 != iter->cend(); iter2++)
        {
            cout << "\n\t\tpoint" << ++j << "(x,y): " << iter2->x << "," << iter2->y;
        }
    }
    cout << endl;


    cout << "finished. good luck!" << endl;
    return 0;
}
