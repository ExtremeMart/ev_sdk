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

int ji_init()
{
    return 0;
}

int ji_calc(const unsigned char* buffer, int length, const char* outfn, char** json)
{
    return 0;
}

int ji_calc_video_file(const char* infn, const char* outfn, JI_CB cb)
{
//	cb(1, "json");
	return 0;
}

int ji_calc_video_frame(JI_CV_FRAME* inframe, JI_CV_FRAME** outframe, JI_CB cb)
{
	return 0;
}
