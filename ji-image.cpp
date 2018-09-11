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
#include "ClothesPredicator.h"

int ji_init(int argc, char** argv) {
	return -2;
}

void *ji_create_predictor() {
	//detector init
	char bufferPath[100];
	getcwd(bufferPath, 100);
	const string basePath = bufferPath;
	const string dmodel_file = basePath + "/model/deploy.prototxt";
	const string dweights_file = basePath + "/model/_iter_120000.caffemodel";
	const string dmean_file = "";

	ModelFile detectorModelFile(dmodel_file, dweights_file, dmean_file, "", "");

	//classifier init
	const string model_file0 = basePath
			+ "/model/ResNet_50_train_val20.prototxt";
	const string trained_file0 = basePath
			+ "/model/ResNet_50_train_val23_iter_500000.caffemodel";
	const string mean_file0 = basePath + "/model/imagenet_mean.binaryproto";
	const string label_file0 = basePath + "/model/cloth_shape.txt";
	const string label_file2 = basePath + "/model/cloth_type.txt";

	ModelFile classifierModelFile(model_file0, trained_file0, mean_file0,
			label_file0, label_file2);
	std::vector<ModelFile> modelFile;
	modelFile.push_back(detectorModelFile);
	modelFile.push_back(classifierModelFile);
	//predictor init
	ClothesPredicator *clothesPredicator = new ClothesPredicator(modelFile);
	return clothesPredicator;

}

void ji_destory_predictor(void* predictor) {
	ClothesPredicator *predictor_ = (ClothesPredicator *) predictor;
	delete predictor_;
}

int ji_calc(void* predictor, const unsigned char* buffer, int length,
		const char* args, const char* outfn, char** json) {

	ClothesPredicator *clothesPredicator = (ClothesPredicator *) predictor;
	std::vector<uchar> bufferSet;
	for (int i = 0; i < length; i++) {
		bufferSet.push_back(buffer[i]);
	}
	Json::Value temp;
	clothesPredicator->analysisImgBuffer(bufferSet, length, args, outfn, temp);
	if (!temp.empty()) {
		char *jsonTemp = new char[temp.toStyledString().size()+1];
		strcpy(jsonTemp, temp.toStyledString().c_str());
		*json = jsonTemp;
	} else {
		return -1;
	}

	return 0;
}

int ji_calc_file(void* predictor, const char* infn, const char* args,
		const char* outfn, char** json) {
	ClothesPredicator *clothesPredicator = (ClothesPredicator *) predictor;
	Json::Value temp;
	clothesPredicator->analysisImgfile(infn, args, outfn, temp);
	if (!temp.empty()) {
		char *jsonTemp = new char[temp.toStyledString().size()+1];
		strcpy(jsonTemp, temp.toStyledString().c_str());
		*json = jsonTemp;
	} else {
		return -1;
	}

	return 0;

}

int ji_calc_video_file(void* predictor, const char* infn, const char* args,
		const char* outfn, JI_EVENT* event) {
	return -2;
}

int ji_calc_video_frame(void* predictor, JI_CV_FRAME* inframe, const char* args,
		JI_CV_FRAME* outframe, JI_EVENT* event) {
	return -2;
}
