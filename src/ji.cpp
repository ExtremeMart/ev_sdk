/*
**
    仅用于演示
    ji.h的实现,包括license的实现.
**
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>

#include "ji.h"
#include "ji_license.h"
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

/* 自定义检测器示例 */
class CustomPredictor
{
public:
    void create(int _pbtype)  
    {
        srand(time(NULL));

        pbtype = _pbtype;

        /* allocate predictor */
        predictor = NULL;

        /* 
        ** 模块加解密 
        for example:
        ./3rd/bin/encrypt_model test_model.proto 01234567890123456789012345678988
        out file: model_str.hpp, copy this file to "./include" directory and add to ev_sdk project.
        **
        */
        void* file = CreateEncryptor(model_str.c_str(), model_str.size(), key.c_str());

        int fileLen = 0;
        char *fileContent = (char*)FetchBuffer(file, fileLen);
        std::cout << "FetchBuffer:" << fileContent << std::endl;

        /* 
        ** for example:
        predictor = new Classifier(fileContent, 
            "/usr/local/ev_sdk/model/model.dat", 
            "104,117,123", 
            "female,man,kid");
        ** 
        */

        DestroyEncrtptor(file);

        json = NULL;
        jsonMaxLen = 0;
    }

    void destroy() 
    {
        /* destroy predictor */
        if (predictor)
        {
            //delete (Classifier*)predictor;
        }

        if (json)
        {
            delete [] json; json = NULL;
        }
        
        jsonMaxLen = 0;
    }
    
    bool runByFrame(const cv::Mat &inMat, const char *args, 
                        cv::Mat &outMat, std::string& strJson) 
    {
        /* 模拟args(roi)的处理 （开发者可自行编写添加）*/
        cv::Mat srcMat = inMat;
        if (args && (strlen(args) > 0))
        {   
            std::vector<cv::Point> v_point;
            BoostInterface obj;
            obj.parsePolygon(args, inMat.size(), v_point);
            if (!v_point.empty())
            {
                srcMat = inMat(obj.polygon2Rect(v_point));
            }
        }

        /* in:srcMat, out:outMat */
        /* 
        **for example:
        Classifier * pc = (Classifier *)predictor;
        std::vector<Prediction> predictions = pc->Classify(srcMat);
        **
        */
        
        /* 
        ** 模拟算法返回的结果
        ** json1,json2是算法返回的结果
        ** json1
        {
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

        ** json2
        {
        	"timestamp":1000000,
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
        }
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
        strJson = (rand() % 2)?json1:json2;

        /* 根据算法类型及算法返回结果，绘制输出mat */
        /* 开发者自行编写实现 */
        // srcMat draw...
        outMat = srcMat;
        return true;
    }
    
    static bool checkAlarm(const std::string& strJson) 
    {
        return (strJson.find("\"alertFlag\": 1,") != std::string::npos);
    }

    static int calc(void *predictor, const cv::Mat &inMat, const char *args, 
                    cv::Mat &outMat, JI_EVENT *event)
    {
        int iRet = ji_check_expire();
        if (iRet != EV_SUCCESS)
        {
            if (iRet == EV_OVERMAXQPS)
            {
                return JISDK_RET_OVERMAXQPS;
            }

            return JISDK_RET_UNAUTHORIZED;
        }

        CustomPredictor *pcp = (CustomPredictor*)predictor;
        std::string strJson;

        if (!pcp->runByFrame(inMat,args,pcp->mat,strJson))
        {
            return JISDK_RET_FAILED;
        }

        outMat = pcp->mat;
        if (!strJson.empty())
        {
            int s = strJson.size() + 1;
            if (s > pcp->jsonMaxLen)
            {   
                if (pcp->json)
                {
                    delete [] pcp->json; 
                    pcp->json = NULL;
                }

                pcp->jsonMaxLen = ((s - 1) / 8192 + 1) * 8192;
                pcp->json = new char[pcp->jsonMaxLen];
            }
            strcpy(pcp->json,strJson.c_str());

            if (event)
            {
                event->code = CustomPredictor::checkAlarm(strJson)?JISDK_CODE_ALARM:JISDK_CODE_NORMAL;
                event->json = pcp->json;        
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
    int pbtype;
    void *predictor;

    cv::Mat mat;
    char *json;
    int jsonMaxLen;
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
    ** 请自行替换算法的公钥
    for example:
    ./3rd/license/v5/ev_license -c pubKey.txt
    ** 
    */
    const char pubKey[] = {
        '-','-','-','-','-','B','E','G','I','N',' ','P','U','B','L','I',
        'C',' ','K','E','Y','-','-','-','-','-','\n','M','I','G','f','M',
        'A','0','G','C','S','q','G','S','I','b','3','D','Q','E','B','A',
        'Q','U','A','A','4','G','N','A','D','C','B','i','Q','K','B','g',
        'Q','D','B','+','1','d','X','X','A','0','y','D','7','r','c','w',
        'W','j','P','l','h','P','R','I','6','D','m','\n','o','E','C','A',
        'T','a','N','B','z','X','G','f','U','j','o','L','v','G','S','b',
        'b','J','t','O','0','u','j','L','m','P','v','e','4','0','c','3',
        'b','l','4','Q','k','t','n','Q','H','f','H','1','O','W','G','Z',
        'W','h','4','v','9','6','H','o','f','t','H','f','\n','l','G','W',
        'X','X','b','u','8','4','H','E','F','S','W','M','B','s','6','x',
        '/','x','p','E','q','s','h','M','N','1','D','6','H','m','c','w',
        'X','k','4','g','E','3','g','I','d','r','/','h','W','q','g','t',
        'F','m','+','v','H','M','W','l','I','+','t','Q','J','\n','0','o',
        'e','W','U','e','b','G','F','I','p','+','m','E','F','x','W','Q',
        'I','D','A','Q','A','B','\n','-','-','-','-','-','E','N','D',' ',
        'P','U','B','L','I','C',' ','K','E','Y','-','-','-','-','-','\0'
    };

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
/*  
** json 格式(算法不同输出格也有所不同)：
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
                                    vcapture.get(cv::CAP_PROP_FOURCC),
                                    vcapture.get(cv::CAP_PROP_FPS),
                                    cv::Size(outMat.cols,outMat.rows));
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
                        cJSON_AddItemToObject(jsonFrame,"timestamp", cJSON_CreateNumber(timestamp)); 
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
            cJSON_AddItemToObject(jsonRoot, "totalFrames", cJSON_CreateNumber(totalFrames));
            cJSON_AddItemToObject(jsonRoot, "alertFrames", cJSON_CreateNumber(alertFrames));

            if (jsonDetail)
            {
                cJSON_AddItemToObject(jsonRoot,"detail", jsonDetail);
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
