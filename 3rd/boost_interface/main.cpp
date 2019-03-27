/*
 * main.cpp
 *
 *  Created on: Nov 12, 2018
 *      Author: dx
 */


#include <iostream>
#include "BoostInterface.h"


/*
**
请注意，roi感兴趣区域 分为3种，点，线，面，其中面为多边形（例如，三角形，或矩形，或者其他） 
下列有对应的写法char * roiPoint = "POINT(0.9 0.5)|POINT(0.9 0.5)";
这是多点写法（多线段和多个多边形类似）里面的参数 为比例分割 例如： 0.1 = 1/10
极市小工具网址  http://dev.polygonlinepointtool.extremevision.com.cn/   

请注意（重点！！！！！！）：
感兴趣区域是什么就要画出什么。例如 是五边形在输出的结果上就需要画出五边形.
**
*/

using namespace std;

int main(int argc, char* argv[])
{
    BoostInterface boostInterface;
    cv::Size curSize(1920, 1080);
    std::vector<cv::Point> point;
    std::vector<cv::Point> pointLine;
    std::vector<cv::Point> pointPolygon;

    //解析点 
    const char * roiPoint = "POINT(0.9 0.5)";
    boostInterface.parsePoint(roiPoint, curSize, point);
    cout << "test parsePoint, input: " << roiPoint << ", output: size=" << point.size() << endl;
    for(int i = 0; i < point.size(); i++)
    {
        cout << "point " << i+1 << " info:" << endl;
        cout << "\tx="   << point[i].x << endl;
        cout << "\ty="   << point[i].y << endl;
    }
    cout << endl;

    //解析线段
    const char * roiLine = "LINESTRING(0.1 0.4, 0.9 0.5)"; 
    boostInterface.parseLinestring(roiLine, curSize, pointLine);
    cout << "test parseLinestring, input: " << roiLine << ", output: size=" << pointLine.size() << endl;
    for(int i = 0; i < pointLine.size(); i++)
    {
        cout << "point " << i+1 << " info:" << endl;
        cout << "\tx="   << pointLine[i].x << endl;
        cout << "\ty="   << pointLine[i].y << endl;
    }
    cout << endl;

    //解析多边形(框)
    const char * roiPolygon = "POLYGON((0.1 0.4, 0.9 0.5,0.35 0.4,0.66 0.66, 0.78 0.78))";
    boostInterface.parsePolygon(roiPolygon, curSize, pointPolygon);
    cout << "test parsePolygon, input: " << roiPolygon << ", output: size=" << pointPolygon.size() << endl;
    for(int i = 0; i < pointPolygon.size(); i++)
    {
        cout << "point " << i+1 << " info:" << endl;
        cout << "\tx="   << pointPolygon[i].x << endl;
        cout << "\ty="   << pointPolygon[i].y << endl;
    }
    cout << endl;

    return 0;
}
