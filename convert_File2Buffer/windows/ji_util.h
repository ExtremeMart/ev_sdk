#pragma warning(disable:4996)
#ifndef JI_UTIL_H
#define JI_UTIL_H

#include <fstream>  
#include <iostream>
#include <iterator>
#include <vector>
#include <sstream>

using namespace std;


	/*
	 * convert image file to buffer.
	 * parameters:
	 * 	inFileName : input file name
	 * 	outBuffer  : output file buffer
	 * return:
	 * 	0:success
	 * 	other:fail
	 */
	int jiFile2buffer(const char* inFileName, vector<unsigned char>* outBuffer);

	int jiFile2buffer(const char* inFileName, vector<unsigned char>* outBuffer){
	
		FILE *fp = fopen(inFileName, "rb");
		if(!fp) return false;
		fseek(fp, 0, SEEK_END);
		unsigned long imgBufferSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		unsigned char *pBufferTemp = new unsigned char[imgBufferSize];
		fread(pBufferTemp, sizeof(unsigned char), imgBufferSize, fp);
		
		vector<unsigned char> buffer;
		for(int i=0;i<imgBufferSize;i++){
			buffer.push_back(pBufferTemp[i]);
		}
		*outBuffer = buffer;
		delete []pBufferTemp;
		return 0;
}


#endif // JI_UTIL_H

