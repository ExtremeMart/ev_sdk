#include "src/ji.h"
//#include "ji_util.h"//这个为可选项 图片转buffer 但是这个函数已经在下面实现 就无需这个头文件
#include <string.h>
#include <sstream>
#include <glog/logging.h>
#include <vector>
#include <iostream>
#include <sys/time.h>
using namespace std;
/*
*
*请注意，Demo.cpp文件内，请不要添加 开发者自写的 头文件
*用demo测试的时候 请注意是否有感兴区域绘制，如有请询问开发者感兴区域 是怎么写的,（推荐使用极市 模板 BoostInterface）
*如有不同的图片或者视频路径 请自定义
*
*/

double get_current_time()
{
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}

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
    
int main(int argc, char *argv[])
{   
    
    if(argc < 4){
        std::cout<<"parameter number not right"<<std::endl;
        return -1;
    }
        
    const char *videoFile = argv[1];
    const char *videoFileOutput = argv[2];
    const char *roiErea = "POLYGON((0.25 0, 1 0, 1 1, 0.25 1))";

    if(std::string(argv[3]) == "0"){
        //int iRet = ji_init(argc,&argv[argc - 1]);
        //if(iRet == -1){
        //    std::cout<<"error"<<std::endl;
        //    return -1;
        //}
        void *predictor = ji_create_predictor();

        if (predictor == NULL) {
            std::cout<<"ji_create_predictor-------NULL"<<std::endl;
            return -1;
        }

        const char* inFileName = argv[1];
        const char* outFileName = argv[2];
        vector<unsigned char> buffer;
        char *json = NULL;
        jiFile2buffer(inFileName, &buffer);

        int ret = ji_calc(predictor, (const unsigned char *)&buffer[0], buffer.size(),roiErea, outFileName, &json);

        printf("%s", json);
            delete[] json;
            json = NULL;

        ji_destory_predictor(predictor);

    }else if(std::string(argv[3]) == "1"){
        //int iRet = ji_init(argc,&argv[argc - 1]);
        //if(iRet == -1){
        //    std::cout<<"error"<<std::endl;
        //    return -1;
        //}
        void *predictor = ji_create_predictor();
        if (predictor == NULL) {
            std::cout<<"ji_create_predictor-------NULL"<<std::endl;
            return -1;
        }
        const char* inFileName = argv[1];
        const char* outFileName = argv[2];
        char *json = NULL;

        int ret = ji_calc_file(predictor, inFileName, roiErea, outFileName, &json);

        printf("%s\n", json);
        delete[] json;
        json = NULL;

        ji_destory_predictor(predictor);
    }else if(std::string(argv[3]) == "2"){
        JI_EVENT event;
        event.json = NULL;
        //int iRet = ji_init(argc,&argv[argc - 1]);
        //    return -1;
        //}
        void * predictor = ji_create_predictor();
        if (predictor == NULL) {
            std::cout<<"ji_create_predictor-------NULL"<<std::endl;
            return -1;
        }

        int ret = ji_calc_video_file(predictor, videoFile, roiErea, videoFileOutput, &event);

        if(event.json !=NULL) {
            std::cout << event.json << std::endl;
            delete[] event.json;
            event.json = NULL;

                ji_destory_predictor(predictor);
        }
    }else if(std::string(argv[3]) == "3"){
        //int iRet = ji_init(argc,&argv[argc - 1]);
        //if(iRet == -1){
        //    std::cout<<"error"<<std::endl;
        //    return -1;
        //}
        cv::VideoWriter writer;
        void * predictor = ji_create_predictor();
        if (predictor == NULL) {
            std::cout<<"ji_create_predictor-------NULL"<<std::endl;
            return -1;
        }
        cv::VideoCapture capture;
        capture.open(videoFile);
        bool inited = false;
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

                        int ret = ji_calc_video_frame(predictor,&inf,roiErea,&outf,&event);

                        cv::Mat input__(inf.rows,inf.cols,inf.type,inf.data,inf.step);
                        cv::Mat output__(outf.rows,outf.cols,outf.type,outf.data,outf.step);
                        if(!inited){
                                inited = true;
                                writer.open(videoFileOutput,CV_FOURCC('F', 'L', 'V', '1'),
                                                        capture.get(cv::CAP_PROP_FPS),
                                                        cv::Size(output__.cols,
                                                                         output__.rows));
                        }
                        writer<<output__;
                        if(event.json !=NULL) {
                                std::cout << event.json << std::endl;
                                delete[] event.json;
                        }
                        delete [] (uchar*)outf.data;
                }
                writer.release();
                ji_destory_predictor(predictor);
    }else if(std::string(argv[3]) == "4"){

                const char* inFileName = argv[1];

        cv::VideoWriter writer;
        void * predictor = ji_create_predictor();
        if (predictor == NULL) {
            std::cout<<"ji_create_predictor-------NULL"<<std::endl;
            return -1;
        }

        bool inited = false;

                JI_EVENT event;
                event.json = NULL;
                cv::Mat frame = cv::imread(inFileName);
                JI_CV_FRAME inf,outf;
                inf.rows = frame.rows;
                inf.cols = frame.cols;
                inf.step = frame.step;
                inf.data = frame.data;
                inf.type = frame.type();
                std::cout << inf.rows << std::endl;

                int ret = ji_calc_video_frame(predictor,&inf,roiErea,&outf,&event);

                cv::Mat input__(inf.rows,inf.cols,inf.type,inf.data,inf.step);
                cv::Mat output__(outf.rows,outf.cols,outf.type,outf.data,outf.step);

                writer<<output__;
                if(event.json !=NULL) {
                        std::cout << event.json << std::endl;
                        delete[] event.json;
                }
                delete [] (uchar*)outf.data;

                writer.release();
                ji_destory_predictor(predictor);
    }else {
        std::cout<<"Wrong parameter"<<std::endl;
        }
    return 0;
}
