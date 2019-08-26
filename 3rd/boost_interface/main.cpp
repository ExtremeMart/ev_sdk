#include <iostream>
#include "BoostInterface.h"

using namespace std;

int main(int argc, char* argv[])
{
    /*
    **
    ** ji_calc_frame ->args,args具体有什么信息有开发者根据算法及项目需求来制定
    ** 目前支持两种格式：一种建议用|分隔各类信息；另外一种是json格式表示
    ** |分隔示例：
    "cid=1000|POINT(0.38 0.10)|POINT(0.47 0.41)|LINESTRING(0.07 0.21,0.36 0.245,0.58 0.16,0.97 0.27)|"
    "POLYGON((0.048 0.357,0.166 0.0725,0.393 0.0075,0.392 0.202,0.242 0.375))|"
    "POLYGON((0.513 0.232,0.79 0.1075,0.928 0.102,0.953 0.64,0.759 0.89,0.51 0.245))|"
    "POLYGON((0.115 0.497,0.592 0.82,0.581 0.917,0.14 0.932))"
    ** json格式示例:
    {
        "cid": "1000",
        "POINT": [
            "POINT(0.38 0.10)",
            "POINT(0.47 0.41)"
        ],
        "LINESTRING": "LINESTRING(0.070.21,0.360.245,0.580.16,0.970.27)",
        "POLYGON": [
            "POLYGON((0.0480.357,0.1660.0725,0.3930.0075,0.3920.202,0.2420.375))",
            "POLYGON((0.5130.232,0.790.1075,0.9280.102,0.9530.64,0.7590.89,0.510.245))",
            "POLYGON((0.1150.497,0.5920.82,0.5810.917,0.140.932))"
        ]
    }    
    ** 最终对args的解析有开发者完成，开发者分隔出wkt内容后，调用本库对应接口，以便完成后续的功能
    */
    BoostInterface bi(cv::Size(640,480));
    size_t i,j;
    
    /* 解析点 */
    const char * cst_point_1 = "POINT(0.38 0.10)";
    const char * cst_point_2 = "POINT(0.47 0.41)";
    cv::Point point_1, point_2;
    bi.parsePoint(cst_point_1, &point_1);
    bi.parsePoint(cst_point_2, &point_2);
    cout << "test parsePoint ..." << 
    "\n\tcst_point_1: " << cst_point_1 << ", point_1(x,y): " << point_1.x << "," << point_1.y <<
    "\n\tcst_point_2: " << cst_point_2 << ", point_2(x,y): " << point_2.x << "," << point_2.y << endl;


    /* 解析线段 */
    const char *cst_line = "LINESTRING(0.07 0.21,0.36 0.245,0.58 0.16,0.97 0.27)";
    Vector_Point line;
    bi.parseLinestring(cst_line, &line);
    cout << "test parseLinestring ..." << 
    "\n\tcst_line: " << cst_line << ", size: " << line.size() << endl;
    cout << "v_line info, size: " << line.size();
    for(i = 0; i < line.size(); i++)
    {
        cout << "\n\tpoint " << i+1 << "(x,y): " << line[i].x << "," << line[i].y;
    }
    cout << endl;


    /* 解析多边形(框) */
    const char *cst_polygon_1 = "POLYGON((0.048 0.357,0.166 0.0725,0.393 0.0075,0.392 0.202,0.242 0.375))";
    const char *cst_polygon_2 = "POLYGON((0.513 0.232,0.79 0.1075,0.928 0.102,0.953 0.64,0.759 0.89,0.51 0.245))";
    const char *cst_polygon_3 = "POLYGON((0.115 0.497,0.592 0.82,0.581 0.917,0.14 0.932))";
    Vector_Point polygon_1,polygon_2,polygon_3;
    bi.parsePolygon(cst_polygon_1, &polygon_1);
    bi.parsePolygon(cst_polygon_2, &polygon_2);
    bi.parsePolygon(cst_polygon_3, &polygon_3);
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


    /* 判断点是否在多边形区域内 */
    if (bi.inPolygons(cv::Point(152,96)))
    {
        cout << "inPolygons(cv::Point(152,96), return true." << endl;
    }
    else
    {
        cout << "inPolygons(cv::Point(152,96), return false." << endl;
    }


    /* 判断矩形是否在多边形区域内 */
    if (bi.inPolygons(cv::Rect(140,80,20,20)))
    {
        cout << "inPolygons(cv::Rect(150,90,20,20)), return true." << endl;
    }
    else 
    {
        cout << "inPolygons(cv::Rect(150,90,20,20)), return false." << endl;
    }

    /* 判断点是否在多边形区域内2 */
    if (bi.inPolygons(cv::Point(280,192)))
    {
        cout << "inPolygons(cv::Point(280,192), return true." << endl;
    }
    else
    {
        cout << "inPolygons(cv::Point(280,192), return false." << endl;
    }


    /* 判断矩形是否在多边形区域内2 */
    if (bi.inPolygons(cv::Rect(150,90,200,20)))
    {
        cout << "inPolygons(cv::Rect(150,90,200,20)), return true." << endl;
    }
    else 
    {
        cout << "inPolygons(cv::Rect(150,90,200,20)), return false." << endl;
    }


    /* 输出bi内所有的点线框 */
    const Vector_Point &cf_points = bi.getPoints();
    const Vector_Line  &cf_lines  = bi.getLines();
    const Vector_Polygon &cf_polygons = bi.getPolygons();
    cout << "bi.m_points info, size: " << cf_points.size();
    i = 0;
    for (auto iter = cf_points.cbegin(); iter != cf_points.cend(); iter++)
    {
        cout << "\n\tpoint" << ++i << "(x,y): " << iter->x << "," << iter->y;
    }
    cout << endl;

    cout << "bi.m_lines info, size: " << cf_lines.size();
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
    
    cout << "bi.m_polygons info, size: " << cf_polygons.size();
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
