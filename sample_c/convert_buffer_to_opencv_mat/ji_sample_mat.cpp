#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

#include "ji.h"

#include <opencv2/highgui.hpp>

using namespace std;

int ji_init()
{
    return 0;
}

int ji_calc(const unsigned char* buffer, int length, const char* outfn, char** json)
{
	/* convert file buffer to opencv Mat  */
	vector<unsigned char> decodebuf(buffer,buffer+length);
	cv::Mat img =cv::imdecode(cv::Mat(decodebuf), -1);
	cv::imshow("test image",img);
	cv::waitKey(-1);
    return 0;
}
