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
#include "OffDutyPredicator.h"
using namespace std;
int ji_init()
{

    return -2;
}

void *ji_create_predictor(){
    std::string evhead_yolo_model_ = "/home/dx/Desktop/OffDutyV0.2/model/evhead_final.weights";
    std::string evhead_yolo_config_ = "/home/dx/Desktop/OffDutyV0.2/model/evhead_test.cfg";
    std::string meanPath = "";
    OffDutyPredicator *predictor = new OffDutyPredicator(evhead_yolo_model_, evhead_yolo_config_, meanPath);
    return predictor;

}

void ji_destory_predictor(void* predictor){
    OffDutyPredicator *predictor_ = (OffDutyPredicator *)predictor;
    delete predictor_;

}

int ji_calc(void* predictor, const unsigned char* buffer, int length,
		const char* args, const char* outfn, char** json) {

	return -2;
}

int ji_calc_file(void* predictor, const char* infn, const char* args,
		const char* outfn, char** json) {

	return -2;
}


int ji_calc_video_file(void* predictor, const char* infn, const char* args, const char* outfn, JI_EVENT* event){
    OffDutyPredicator *ooffduty = (OffDutyPredicator *)predictor;
    Json::Value temp;
    //Stepover->runByVideo(infn, args, outfn,temp);
    ooffduty->runByVideo(infn, args, outfn,temp);

    if(!temp.empty()){
        event->json = new char[(temp.toStyledString()).size()+1];
        strcpy( event->json,temp.toStyledString().c_str());
        event->json[temp.toStyledString().size()] = '\0';
        event->code = 0;
    }
    else{
        event->code = -1;
    }

//    std::ofstream outfile;
//    outfile.open("/home/fan/config");
//    std::ostringstream messages;
//    messages<<temp.toStyledString()<<std::endl;
//    outfile<<messages.str()<<std::endl;
//    outfile.close();
    return 0;

}

int ji_calc_video_frame(void* predictor, JI_CV_FRAME* inframe, const char* args, JI_CV_FRAME* outframe, JI_EVENT* event){
    OffDutyPredicator *ooffduty = (OffDutyPredicator *)predictor;

    cv::Mat inmat(inframe->rows,inframe->cols,inframe->type,inframe->data,inframe->step);
    cv::Mat outmat ;
    Json::Value temp;
    ooffduty->runByVideoFrame(inmat,args,outmat,temp);

    if(!temp.empty()){
        event->json = new char[temp.toStyledString().size()+1];
        strcpy(event->json,temp.toStyledString().c_str());
        event->json[temp.toStyledString().size()] = '\0';
        event->code = 0;
        outframe->cols = outmat.cols;
        outframe->rows = outmat.rows;
        outframe->type = outmat.type();
        outframe->data = new char[outmat.total() * outmat.elemSize()];
        memcpy(outframe->data,outmat.data,outmat.total() * outmat.elemSize());
        //outframe->data = outmat.data;
        outframe->step = outmat.step;
    } else{
        event->code = -1;
    }
    return 0;
}

