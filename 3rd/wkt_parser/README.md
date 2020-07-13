# WKT解析器
`WKTParser`是一个专门用于解析WKT格式的字符串的工具。

## 定义
1. 点表示法：`POINT (30 10)`
   
   ![POINT](https://upload.wikimedia.org/wikipedia/commons/thumb/c/c2/SFA_Point.svg/102px-SFA_Point.svg.png)
2. 线表示法：`LINESTRING (30 10, 10 30, 40 40)`

   ![LINE](https://upload.wikimedia.org/wikipedia/commons/thumb/b/b9/SFA_LineString.svg/102px-SFA_LineString.svg.png)
3. 多边形表示法：`POLYGON ((30 10, 40 40, 20 40, 10 20, 30 10)`

   ![POLYGON](https://upload.wikimedia.org/wikipedia/commons/thumb/3/3f/SFA_Polygon.svg/102px-SFA_Polygon.svg.png)

WKT格式的具体定义请参考[WKT格式](./WKT_format.pdf)、[WKT 格式定义](https://en.wikipedia.org/wiki/Well-known_text_representation_of_geometry)、[WKT示例](https://www.mysqlzh.com/doc/175/141.html)。

## 使用方法
`WKTParser`初始化时需要提供一个图像尺寸`(width, height)`，用于反归一化坐标，使得`args`中的ROI支持多种不同尺寸的图片。
1. 初始化`WKTParser`
   ```c++
   WKTParser wktParser(100, 200)
   ``` 
2. 解析点
   ```c++
   const char * cst_point_1 = "POINT(0.38 0.10)";
   cv::Point point;
   wktParser.parsePoint(cst_point_1, &point);
   ```
   解析线
   ```c++
   const char *cst_line = "LINESTRING(0.07 0.21,0.36 0.245,0.58 0.16,0.97 0.27)";
   VectorPoint line;
   wktParser.parseLinestring(cst_line, &line);
   ```
   解析多边形(框)
   ```c++
   const char *cst_polygon = "POLYGON((0.048 0.357,0.166 0.0725,0.393 0.0075,0.392 0.202,0.242 0.375))";
   VectorPoint polygon;
   wktParser.parsePolygon(cst_polygon, &polygon);
   ```
其他更多功能请参考代码`WKTParser.h`