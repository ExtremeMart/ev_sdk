#include <iostream>
#include "src/ji.h"
#include "src/OffDutyPredicator.h"
int main(int argc, char *argv[]) {
    google::InitGoogleLogging("ev");
    FLAGS_stderrthreshold = google::ERROR;
////    const char *videoFile = "/home/dx/Desktop/OffDutyV0.1/data/test02.mp4";
////    const char *videoFileOutput = "/home/dx/Desktop/OffDutyV0.1/dest/out.flv";
    if(argc < 4){
        std::cout<<"Warning : parameter number not right"<<std::endl;
        return -1;
    }

    const char *videoFile = argv[1];
    const char *videoFileOutput = argv[2];
    const char *roiErea = "POLYGON((0.1015625 0.10555556,0.9 0.10555556,0.9 0.9,0.1 0.9,0.1 0.1))";

    //TEST FRAME
    if(std::string(argv[3]) == "1") {
        cv::VideoCapture capture;
        capture.open(videoFile);
        cv::VideoWriter writer;
        bool inited = false;
        void * predictor = ji_create_predictor();
        while(true){
            JI_EVENT event;
            event.json = NULL;
            cv::Mat frame;
            capture>>frame;
            if(frame.empty()) break;
            JI_CV_FRAME inf,outf;
            inf.rows = frame.rows;
            inf.cols = frame.cols;
            inf.step = frame.step;
            inf.data = frame.data;
            inf.type = frame.type();
            ji_calc_video_frame(predictor,&inf,roiErea,&outf,&event);

            cv::Mat input__(inf.rows,inf.cols,inf.type,inf.data,inf.step);
            cv::Mat output__(outf.rows,outf.cols,outf.type,outf.data,outf.step);
            if(!inited){
                inited = true;
                writer.open(videoFileOutput,CV_FOURCC('F', 'L', 'V', '1'),
                            capture.get(cv::CAP_PROP_FPS),
                            cv::Size(output__.cols,
                                     output__.rows));
            }
//            cv::imshow("input__",input__);
//            cv::waitKey(1);
//            cv::imshow("output__",output__);
//            cv::waitKey(1);
            writer<<output__;
            if(event.json !=NULL) {
                std::cout << event.json << std::endl;
                delete [] event.json;
            }
            delete [] (uchar*)outf.data;
        }
        writer.release();
        ji_destory_predictor(predictor);
    }

        //TEST VIDEO
    else if(std::string(argv[3]) == "0") {
        JI_EVENT event;
        event.json = NULL;
        void *predictor = ji_create_predictor();
        ji_calc_video_file(predictor, videoFile, roiErea, videoFileOutput, &event);
        std::cout << event.json << std::endl;
        delete []event.json;
        event.json = NULL;
        ji_destory_predictor(predictor);
    }

    else{
        std::cout<< "Warning : parameter wrong"<<std::endl;
    }


    google::ShutdownGoogleLogging();
}
