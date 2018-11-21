/*
 * main.cpp
 *
 *  Created on: Nov 12, 2018
 *      Author: dx
 */


#include <iostream>
#include "BoostInterface.h"
//char * roiPoint = "POINT(0.9 0.5)|POINT(0.9 0.5)";这是多点写法（多线段和多个多边形类似）
// 极市小工具网址  http://dev.polygonlinepointtool.extremevision.com.cn/   

int main(int argc, char* argv[]){
	BoostInterface boostInterface;
	cv::Size curSize(1920, 1080);
	vector<cv::Point> pointLine;
	vector<cv::Point> point;
	vector<cv::Point> pointPolygon;
	//解析线段
	std::cout<<"test"<<std::endl;
	char * roiLine = "LINESTRING(0.1 0.4, 0.9 0.5)";
	boostInterface.parseLinestring(roiLine, curSize, pointLine);
	for(int i = 0; i < pointLine.size(); i++){
		cout<<"line"<<pointLine[i].x<<endl;
		cout<<"line"<<pointLine[i].y<<endl;
	}
	//解析点
	char * roiPoint = "POINT(0.9 0.5)";
	boostInterface.parsePoint(roiPoint, curSize, point);
	for(int i = 0; i < point.size(); i++){
		cout<<"point"<<point[i].x<<endl;
		cout<<"point"<<point[i].y<<endl;
	}
	//解析多边形
	char * roiPolygon = "POLYGON((0.1 0.4, 0.9 0.5,0.35 0.4,0.66 0.66, 0.78 0.78))";
	boostInterface.parsePolygon(roiPolygon, curSize, pointPolygon);
	for(int i = 0; i < pointPolygon.size(); i++){
		cout<<"polygon"<<pointPolygon[i].x<<endl;
		cout<<"polygon"<<pointPolygon[i].y<<endl;
	}
	return 0;
}
