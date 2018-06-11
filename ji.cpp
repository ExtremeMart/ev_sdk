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

using namespace std;

int ji_init(int argc, char** argv)
{
    return 0;
}

void *ji_create_predictor()
{
	return NULL;
}

void ji_destory_predictor(void* predictor)
{
	
}
    
int ji_calc_file(void* predictor, const char* infn, const char* args, const char* outfn, char** json)
{
	return 0;
}

int ji_calc(void* predictor, const unsigned char* buffer, int length, const char* args, const char* outfn, char** json)
{
	//buffer -> vector
	vector<unsigned char> decodebuf(buffer,buffer+length);
	
	//vector --> cv::Mat
//	cv::Mat img =cv::imdecode(cv::Mat(decodebuf), 1);

    //read file ---> cv::Mat
//  cv::Mat img =cv::imread(filename, 1);

    return 0;
}

int ji_calc_buffer(void* predictor, JI_FILE_BUFFER *inBuffer, const char* args, JI_FILE_BUFFER *outBuffer, char** json)
{
	return 0;
}

int ji_calc_video_file(void* predictor, const char* infn, const char* args, const char* outfn, JI_EVENT* event)
{
	return 0;
}

int ji_calc_video_frame(void* predictor, JI_CV_FRAME* inframe, const char* args, JI_CV_FRAME* outframe, JI_EVENT* event)
{
	return 0;
}
