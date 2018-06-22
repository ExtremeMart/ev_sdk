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

int ji_init(int argc, char** argv) {
	return -2;
}

void *ji_create_predictor() {

}

void ji_destory_predictor(void* predictor) {

}

int ji_calc(void* predictor, const unsigned char* buffer, int length,
		const char* args, const char* outfn, char** json) {
	return -2;
}

int ji_calc_file(void* predictor, const char* infn, const char* args,
		const char* outfn, char** json) {
	return -2;
}

int ji_calc_video_file(void* predictor, const char* infn, const char* args,
		const char* outfn, JI_EVENT* event) {
	return -2;
}

int ji_calc_video_frame(void* predictor, JI_CV_FRAME* inframe, const char* args,
		JI_CV_FRAME* outframe, JI_EVENT* event) {
	return -2;
}
