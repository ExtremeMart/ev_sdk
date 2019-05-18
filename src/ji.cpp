/*
**
    仅用于演示
    ji.h的实现,包括license、模型加密、感兴趣区域的实现
**
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>

#include "ji.h"
#include "ji_license.h"
#include "pubKey.hpp"
#include "cJSON.h"
#include "BoostInterface.h"

#include "encrypt_wrapper.hpp"
#include "model_str.hpp"


#define OPEN_PRINT 0
#if OPEN_PRINT 
#define PRINT(format,...) printf("File: "__FILE__", Line: %d: "format"\n", __LINE__, ##__VA_ARGS__)
#else  
#define PRINT(format,...)
#endif


/* 
** 说明
ji_calc_frame
ji_calc_buffer
ji_calc_file
ji_calc_video_file
    -------------->
    CustomPredictor::calc
        -------------->
        CustomPredictor.runByFrame
**
*/


/* 自定义检测器示例 */
/* 
** 说明
CustomPredictor: 
自定义检测器，有算法工程师补充实现；
需要补充实现的函数有create(),destroy(),checkAlarm(),runByFrame,共四个；
根据需要，算法开发者可自行增加其它函数。
**
*/
class CustomPredictor
{
public:
    /* 初始化 */    
    void create(int _pbtype)  
    {
        /* 随机种子，用于模拟算法输出，实际开发时可屏蔽该行代码 ??? */
        srand(time(NULL));


        /* 请参考    检测器类型值定义 */
        pbtype = _pbtype;
        predictor = NULL;


        /* 解析算法配置属性 ./model/algo_config.json ??? */
        

        /* 
        ** 模块加解密 
        for example:
        ./3rd/bin/encrypt_model test_model.proto 01234567890123456789012345678988
        out file: model_str.hpp, copy this file to "./src" directory and add to ev_sdk project.
        **
        */
        void* file = CreateEncryptor(model_str.c_str(), model_str.size(), key.c_str());

        int fileLen = 0;
        char *fileContent = (char*)FetchBuffer(file, fileLen);

        /* 输出解密后模型文件，实际开发时可屏蔽该行代码 ??? */
        std::cout << "FetchBuffer:" << fileContent << std::endl;

        /* allocate predictor ??? */
        /* 
        ** for example:
        predictor = new Classifier(fileContent, 
            "/usr/local/ev_sdk/model/model.dat", 
            "104,117,123", 
            "female,man,kid");
        ** 
        */
        DestroyEncrtptor(file);
    }

    /* 反初化 */
    void destroy() 
    {
        /* destroy predictor */
        if (predictor)
        {
            //delete (Classifier*)predictor;
        }
    }

    /* 分析一帧--静态成员函数 */
    static int calc(void *predictor, const cv::Mat &inMat, const char *args, 
                    cv::Mat &outMat, JI_EVENT *event)
    {
        int iRet = ji_check_expire();
        if (iRet != EV_SUCCESS)
        {
            return (iRet == EV_OVERMAXQPS)?JISDK_RET_OVERMAXQPS:JISDK_RET_UNAUTHORIZED;
        }

        CustomPredictor *pcp = (CustomPredictor*)predictor;
        if (!pcp->runByFrame(inMat,args?args:"",pcp->mat,pcp->json))
        {
            return JISDK_RET_FAILED;
        }

        outMat = pcp->mat;
        if (!pcp->json.empty())
        {
            if (event)
            {
                /* event->code: 非报警类算法，直接返回JISDK_CODE_ALARM */
                event->code = pcp->checkAlarm(pcp->json)?JISDK_CODE_ALARM:JISDK_CODE_NORMAL;
                event->json = pcp->json.c_str();        
            }
        }
        else 
        {
            if (event)
            {
                
                event->code = JISDK_CODE_FAILED;
                event->json = NULL;
            }
        }

        return JISDK_RET_SUCCEED;
    }

private:    
    bool checkAlarm(const std::string& content) 
    {
        /* 判断是否报警 ??? */
        
        #if 0
        /* 非报警类算法，请直接返回true */
        #endif

        
        #if 0        
        /* 实现方案1:     直接查找报警常量字符串--较偷赖的方式 */
        return (content.find("\"alertFlag\": 1,") != std::string::npos);
        #endif


        #if 1
        /* 实现方案2:     解析json--较安全正确的方式，建议选用 */
        cJSON *jsonRoot = cJSON_Parse(content.c_str());
        if (jsonRoot == NULL)
        {
            return false; 
        }

        bool bRet = false;
        cJSON *pSubJson = cJSON_GetObjectItem(jsonRoot, "alertFlag");
        if(pSubJson && (pSubJson->type == cJSON_Number))
        {
            bRet = (1 == pSubJson->valueint);
        }

        cJSON_Delete(jsonRoot);
        return bRet;
        #endif


        #if 0
        /* 实现方案3: calc函数调用算法时多个参数传出来--较高效的方式 */
        #endif
    }
    
    /* 分析一帧 */
    bool runByFrame(const cv::Mat &inMat, const std::string &args, 
                        cv::Mat &outMat, std::string& strJson) 
    {
        /* 
        ** 注意规避修改源图inMat，算法分析请传destMat，绘制算法结果请在outMat上
        ** 其中destMat指向outMat的roi区域
        ** 1.当roi为空或无效时destMat与outMat指向同一张图 
        ** 2.当roi有效时，注意请重新计算偏移位置并绘制算法结果
        */
        inMat.copyTo(outMat);
        cv::Mat destMat = outMat;

        cv::Rect roiRect(0,0,0,0);
        /* 模拟args(roi)的处理 */
        if (!args.empty())
        {   
            std::vector<cv::Point> v_point;
            BoostInterface obj;
            obj.parsePolygon(args.c_str(), outMat.size(), v_point);
            if (!v_point.empty())
            {
                roiRect = obj.polygon2Rect(v_point);
                destMat = outMat(roiRect);
            }
        }

        /* 算法实现     ??? */
        /* 以下代码模拟算法输出(空算法) */
        /* 注意: 算法请使用destMat */
        /* 
        **for example:
        Classifier * pc = (Classifier *)predictor;
        std::vector<Prediction> predictions = pc->Classify(destMat);
        **
        */
        
        /* 
        ** 模拟算法返回的结果
        ** json1,json2是算法返回的结果
        ** 
        */
        const char json1[] = {
            '{','\n',' ',' ',' ',' ','"','a','l','e','r','t','F','l','a','g',
            '"',':',' ','0',',','\n',' ',' ',' ',' ','"','t','o','t','a','l',
            'H','e','a','d','s','"',':',' ','1',',','\n',' ',' ',' ',' ','"',
            'h','e','a','d','I','n','f','o','"',':',' ','[','\n',' ',' ',' ',
            ' ',' ',' ',' ',' ','{','\n',' ',' ',' ',' ',' ',' ',' ',' ',' ',
            ' ',' ',' ','"','x','"',':',' ','1','0',',','\n',' ',' ',' ',' ',
            ' ',' ',' ',' ',' ',' ',' ',' ','"','y','"',':',' ','1','5',',',
            '\n',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','"','w','i',
            'd','t','h','"',':',' ','2','5',',','\n',' ',' ',' ',' ',' ',' ',
            ' ',' ',' ',' ',' ',' ','"','h','e','i','g','h','t','"',':',' ',
            '2','0',',','\n',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
            '"','h','a','v','e','H','e','l','m','e','t','"',':',' ','1','\n',
            ' ',' ',' ',' ',' ',' ',' ',' ','}','\n',' ',' ',' ',' ',']','\n',
            '}','\0'
        };

        const char json2[] = {
            '{','\n',' ',' ',' ',' ','"','a','l','e','r','t','F','l','a','g',
            '"',':',' ','1',',','\n',' ',' ',' ',' ','"','t','o','t','a','l',
            'H','e','a','d','s','"',':',' ','2',',','\n',' ',' ',' ',' ','"',
            'h','e','a','d','I','n','f','o','"',':',' ','[','\n',' ',' ',' ',
            ' ',' ',' ',' ',' ','{','\n',' ',' ',' ',' ',' ',' ',' ',' ',' ',
            ' ',' ',' ','"','x','"',':',' ','1','0',',','\n',' ',' ',' ',' ',
            ' ',' ',' ',' ',' ',' ',' ',' ','"','y','"',':',' ','1','5',',',
            '\n',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','"','w','i',
            'd','t','h','"',':',' ','2','5',',','\n',' ',' ',' ',' ',' ',' ',
            ' ',' ',' ',' ',' ',' ','"','h','e','i','g','h','t','"',':',' ',
            '2','0',',','\n',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
            '"','h','a','v','e','H','e','l','m','e','t','"',':',' ','1','\n',
            ' ',' ',' ',' ',' ',' ',' ',' ','}',',','\n',' ',' ',' ',' ',' ',
            ' ',' ',' ','{','\n',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
            ' ','"','x','"',':',' ','3','0','0',',','\n',' ',' ',' ',' ',' ',
            ' ',' ',' ',' ',' ',' ',' ','"','y','"',':',' ','5','0','0',',',
            '\n',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','"','w','i',
            'd','t','h','"',':',' ','1','5',',','\n',' ',' ',' ',' ',' ',' ',
            ' ',' ',' ',' ',' ',' ','"','h','e','i','g','h','t','"',':',' ',
            '2','0',',','\n',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
            '"','h','a','v','e','H','e','l','m','e','t','"',':',' ','0','\n',
            ' ',' ',' ',' ',' ',' ',' ',' ','}','\n',' ',' ',' ',' ',']','\n',
            '}','\0'
        };

        /* 当roiRect生效时，重新计算偏移位置 +roiRect.x,+roiRect.y ??? */
        /* 根据算法配置及算法返回结果，绘制输出结果 ??? */
        /* outMat draw... */
        
        strJson = (rand() % 2)?json1:json2;
        return true;
    }
    
private:
    int pbtype;
    void *predictor;

    cv::Mat mat;
    std::string json;
};

int ji_init(int argc, char **argv)
{
/* 
** 参数说明
for example:
argc：4
argv[0]: $license
argv[1]: $timestamp
argv[2]: $qps
argv[3]: $version
...
**
*/
    if (argc < 4 )
    {
        return JISDK_RET_INVALIDPARAMS;
    }

    if (argv[0] == NULL || 
        argv[3] == NULL)
    {
        return JISDK_RET_INVALIDPARAMS;
    }

    int qps = 0;
    if (argv[2]) qps = atoi(argv[2]);

    /* 
    ** 请替换算法的公钥
    for example:
    ./3rd/license/v7/ev_codec -c pubKey.txt
    ** 
    */
    return (ji_check_license(pubKey,argv[0], argv[1], qps>0?&qps:NULL, atoi(argv[3])) == EV_SUCCESS)?
            JISDK_RET_SUCCEED:
            JISDK_RET_UNAUTHORIZED;
}

void* ji_create_predictor(int pdtype)
{
    if (ji_check_expire() != EV_SUCCESS)
    {
        return NULL;
    }

    CustomPredictor *pcp = new CustomPredictor();
    pcp->create(pdtype);
    return pcp;
}

void ji_destroy_predictor(void *predictor)
{
    if (predictor == NULL) return ;

    CustomPredictor *pcp = (CustomPredictor*)predictor;
    pcp->destroy();
    delete pcp;
}

int ji_calc_frame(void *predictor, const JI_CV_FRAME *inframe, const char *args,
                  JI_CV_FRAME *outframe, JI_EVENT *event)
{
    if (predictor == NULL || 
        inframe   == NULL )
    {
        return JISDK_RET_INVALIDPARAMS;
    }
    
    cv::Mat inMat(inframe->rows,inframe->cols,inframe->type,inframe->data,inframe->step);
    if (inMat.empty())
    {
        return JISDK_RET_FAILED; 
    }

    cv::Mat outMat;
    int iRet = CustomPredictor::calc(predictor, inMat, args, outMat, event);
    if (iRet == JISDK_RET_SUCCEED)
    {
        if ((event->code != JISDK_CODE_FAILED) &&
            (!outMat.empty()) &&
            (outframe))
        {
            outframe->rows = outMat.rows;
            outframe->cols = outMat.cols;
            outframe->type = outMat.type();
            outframe->data = outMat.data;
            outframe->step = outMat.step;
        }
    }

    return iRet;
}

int ji_calc_buffer(void *predictor, const void *buffer, int length,
                 const char *args, const char *outfile, JI_EVENT *event)
{
    if (predictor == NULL || 
        buffer    == NULL || 
        length    <= 0 )
    {
        return JISDK_RET_INVALIDPARAMS;
    }

    const unsigned char * b = (const unsigned char*)buffer;
    std::vector<unsigned char> vecBuffer(b,b+length);
    cv::Mat inMat = cv::imdecode(vecBuffer, cv::IMREAD_COLOR);
    if (inMat.empty())
    {
        return JISDK_RET_FAILED; 
    }

    cv::Mat outMat;
    int iRet = CustomPredictor::calc(predictor, inMat, args, outMat, event);
    if (iRet == JISDK_RET_SUCCEED)
    {
        if ((event->code != JISDK_CODE_FAILED) &&
            (!outMat.empty()) &&
            (outfile))
        {
            cv::imwrite(outfile,outMat);
        }
    }

    return iRet;
}
                  
int ji_calc_file(void *predictor, const char *infile, const char *args,
                 const char *outfile, JI_EVENT *event)
{
    if (predictor == NULL || 
        infile    == NULL )
    {
        return JISDK_RET_INVALIDPARAMS;
    }

    cv::Mat inMat = cv::imread(infile);
    if (inMat.empty())
    {
        return JISDK_RET_FAILED;
    }

    cv::Mat outMat;
    int iRet = CustomPredictor::calc(predictor, inMat, args, outMat, event);
    if (iRet == JISDK_RET_SUCCEED)
    {
        if ((event->code != JISDK_CODE_FAILED) &&
            (!outMat.empty()) &&
            (outfile))
        {
            cv::imwrite(outfile,outMat);
        }
    }
    
    return iRet;
}

int ji_calc_video_file(void *predictor, const char *infile, const char* args,
                       const char *outfile, const char *jsonfile)
{
/* 根据算法需求，输出格式有所不同，请完善。??? */

/*  
** json 格式(算法不同输出格式也有所不同)：
** for example:
{
    "totalFrames": 2,
    "alertFrames": 1,
    "detail": [
        {
            "timestamp": 1000000,
            "alertFlag": 1,
            "totalHeads": 2,
            "headInfo": [
                {
                    "x": 10,
                    "y": 15,
                    "width": 25,
                    "height": 20,
                    "haveHelmet": 1
                },
                {
                    "x": 300,
                    "y": 500,
                    "width": 15,
                    "height": 20,
                    "haveHelmet": 0
                }
            ]
        },
        {
            "timestamp": 1000001,
            "alertFlag": 0,
            "totalHeads": 1,
            "headInfo": [
                {
                    "x": 10,
                    "y": 15,
                    "width": 25,
                    "height": 20,
                    "haveHelmet": 1
                }
            ]
        }
    ]
}
**
*/
    if (predictor == NULL ||
        infile    == NULL )
    {
        return JISDK_RET_INVALIDPARAMS;
    }

    cv::VideoCapture vcapture(infile);
    if (!vcapture.isOpened())
    {
        return JISDK_RET_FAILED;
    }

    cv::VideoWriter vwriter;
    cv::Mat inMat, outMat;
    JI_EVENT event;
    int iRet = JISDK_RET_FAILED;
    int totalFrames, alertFrames, timestamp;
    totalFrames = alertFrames = timestamp = 0;

    cJSON *jsonRoot, *jsonDetail;
    jsonRoot = jsonDetail = NULL;
    
    while (vcapture.read(inMat))
    {
        timestamp = vcapture.get(CV_CAP_PROP_POS_MSEC);
        
        iRet = CustomPredictor::calc(predictor, inMat, args, outMat, &event);
        if (iRet == JISDK_RET_SUCCEED)
        {
            ++totalFrames;
            
            if (event.code != JISDK_CODE_FAILED)
            {
                if (event.code == JISDK_CODE_ALARM)
                {
                    ++alertFrames;
                }

                if (!outMat.empty() && outfile)
                {
                    if (!vwriter.isOpened())
                    {
                        vwriter.open(outfile,
                                    /*vcapture.get(cv::CAP_PROP_FOURCC)*/cv::VideoWriter::fourcc('x','2','6','4'),
                                    vcapture.get(cv::CAP_PROP_FPS),
                                    outMat.size());
                        if (!vwriter.isOpened())
                        {
                            return JISDK_RET_FAILED;
                        }
                    }
                    vwriter.write(outMat);
                }

                if (event.json && jsonfile)
                {
                    if (jsonDetail == NULL)
                    {
                        jsonDetail = cJSON_CreateArray();
                    }
                    
                    cJSON *jsonFrame = cJSON_Parse(event.json);
                    if (jsonFrame)
                    {
                        cJSON_AddItemToObjectCS(jsonFrame,"timestamp", cJSON_CreateNumber(timestamp)); 
                        cJSON_AddItemToArray(jsonDetail,jsonFrame);
                    }
                }
            }
        }
        else 
        {
            break;
        }
    }

    if (iRet == JISDK_RET_SUCCEED)
    {
        if (jsonfile)
        {
            jsonRoot = cJSON_CreateObject();
            cJSON_AddItemToObjectCS(jsonRoot, "totalFrames", cJSON_CreateNumber(totalFrames));
            cJSON_AddItemToObjectCS(jsonRoot, "alertFrames", cJSON_CreateNumber(alertFrames));

            if (jsonDetail)
            {
                cJSON_AddItemToObjectCS(jsonRoot,"detail", jsonDetail);
            }

            char *buff = cJSON_Print(jsonRoot); 
            std::ofstream fs(jsonfile);
            if (fs.is_open())
            {
                fs << buff;
                fs.close();
            }
            free(buff);
        }
    }

    if (jsonRoot)
    {
        cJSON_Delete(jsonRoot);
    }
    else if (jsonDetail)
    {
        cJSON_Delete(jsonDetail);
    }

    return iRet;
}
