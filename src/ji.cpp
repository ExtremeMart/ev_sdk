/*
** 仅用于演示
** ji.h的实现，包括license、模型加密、感兴趣区域的实现
** 如果您了解本演示代码，为快速实现一个新算法，您可以查找'???'，以便定位到需要更改的代码处
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fstream>

#include <opencv2/opencv.hpp>
#include <glog/logging.h>

#include "ji.h"

#include "ji_license.h"
#include "pubKey.hpp"

#include "encrypt_wrapper.hpp"
#include "model_str.hpp"

#include "BoostInterface.h"
#include "cJSON.h"


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


/* 
** 自定义检测器示例 CustomPredictor
** CustomPredictor: 
** 自定义检测器，有算法工程师补充实现；
** 需要补充实现的函数有create()，destroy()，checkAlarm()，runByFrame()共四个
** 根据需要，算法开发者可自行增加其它函数。
**
*/
class CustomPredictor
{
public:
    /* 初始化 */    
    void create(int pbtype)  
    {
        /* 随机种子，用于模拟算法结果输出，实际开发请删除该行代码 ??? */
        srand(time(NULL));
        

        /* 请参考'   检测器类型值定义' */
        m_pbtype = pbtype;
        m_predictor = NULL;


        /* 
        ** 解析算法配置选项
        ** 实际开发配置项可能增加，请注意同步实现 ???
        ** 算法配置文件统一存放在'/usr/local/ev_sdk/model/algo_config.json'文件中
        */
        m_draw_roi_area     = 1;
        m_draw_result       = 1;
        m_show_result       = 0;
        m_gpu_id            = 0;
        m_threshold_value   = 0.5;
        if (!readAlgoConfig("/usr/local/ev_sdk/model/algo_config.json"))
        {
            LOG(WARNING) << "parse readAlgoConfig failed!";
        }
        LOG(INFO) << "algo config:\n"
            << "m_draw_roi_area: "      << m_draw_roi_area    << '\n'
            << "m_draw_result: "        << m_draw_result      << '\n'
            << "m_show_result: "        << m_show_result      << '\n'
            << "m_gpu_id: "             << m_gpu_id           << '\n'
            << "m_threshold_value: "    << m_threshold_value  << '\n'
            << "************************";
        
        /* 
        ** 模型加密 
        ** for example:
            `./3rd/bin/encrypt_model test_model.proto 01234567890123456789012345678988`
            其中'01234567890123456789012345678988'表示密钥，也可以不填，表示自动生成密钥
            运行成功后输出'model_str.hpp', 请手动拷贝该文件到'./src'目录即可
        **
        */

        /* 
        ** 模型解密 
        ** 示例代码，实际开发不同 ??? 
        ** 装载模型请用绝对路径，例如'/usr/local/ev_sdk/model/model.dat'
        ** 提供两种使用方式：
        ** 1.获取解密后的字符串
        ** 2.获取解密后的文件句柄
        */
        #if 1
        void *h = CreateEncryptor(model_str.c_str(), model_str.size(), key.c_str());

        #if 0
        /* 获取解密后的字符串 */
        int fileLen = 0;
        char *fileContent = (char*)FetchBuffer(h, fileLen);
        LOG(INFO) << "FetchBuffer:" << fileContent;

        m_predictor = new Classifier(fileContent, 
            "/usr/local/ev_sdk/model/model.dat", 
            "104,117,123", 
            "female,man,kid");
        #endif

        #if 0
        /* 获取解密后的文件句柄 */
        file *file = (file*)FetchFile(h);
        m_predictor = new Classifier(file, 
            "/usr/local/ev_sdk/model/model.dat", 
            "104,117,123", 
            "female,man,kid");
        #endif

        DestroyEncrtptor(h);
        #endif
    }

    /* 反初化 */
    void destroy() 
    {
        /* 释放检测器实例        ??? */
        if (m_predictor)
        {
            //delete (Classifier*)m_predictor;
        }
    }

    /* 
    ** 分析一帧--静态成员函数 
    ** ev_sdk算法分析（四个函数）的入口函数，内部调用'runByFrame'
    */
    static int calc(void *predictor, const cv::Mat &inMat, const char *args, 
                    cv::Mat &outMat, JI_EVENT *event)
    {

        /* 校验license是否过期等 */
        int iRet = ji_check_expire();
        if (iRet != EV_SUCCESS)
        {
            return (iRet == EV_OVERMAXQPS)?JISDK_RET_OVERMAXQPS:JISDK_RET_UNAUTHORIZED;
        }

        /* 调用算法分析函数'runByFrame' */
        CustomPredictor *pcp = (CustomPredictor*)predictor;
        if (!pcp->runByFrame(inMat,args?args:"",pcp->m_mat,pcp->m_json))
        {
            return JISDK_RET_FAILED;
        }

        /* 处理返回结果 */
        outMat = pcp->m_mat;
        if (!pcp->m_json.empty())
        {
            if (event)
            {
                /* event->code: 非报警类算法，直接返回JISDK_CODE_ALARM */
                event->code = pcp->checkAlarm(pcp->m_json)?JISDK_CODE_ALARM:JISDK_CODE_NORMAL;
                event->json = pcp->m_json.c_str();        
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
    /* 分析一帧     算法分析内部实现 */
    bool runByFrame(const cv::Mat &inMat, const std::string &args, 
                    cv::Mat &outMat, std::string& strJson) 
    {
        /* 注意规避修改源图inMat，算法分析及绘制算法结果请在outMat上 */
        inMat.copyTo(outMat);
    
        /*
        ** args参数的处理      ???
        ** args具体有什么信息有开发者根据算法及项目需求来制定
        ** 大部分算法仅有可选的roi信息，wkt格式表示，例如'POLYGON((0.1 0.1,0.9 0.1,0.9 0.9,0.1 0.9))'
        ** args目前支持两种格式
        ** 1.用'|'分隔各类信息,例如：
            "cid=1000|"
            "POINT(0.38 0.10)|POINT(0.47 0.41)|"
            "LINESTRING(0.07 0.21,0.36 0.245,0.58 0.16,0.97 0.27)|"
            "POLYGON((0.048 0.357,0.166 0.0725,0.393 0.0075,0.392 0.202,0.242 0.375))|"
            "POLYGON((0.513 0.232,0.79 0.1075,0.928 0.102,0.953 0.64,0.759 0.89,0.51 0.245))|"
            "POLYGON((0.115 0.497,0.592 0.82,0.581 0.917,0.14 0.932))"
        ** 分隔请调用'handleArgs'函数，分隔后调用'parseArgs'函数
        ** 2.json格式,例如:
            {
                "cid": "1000",
                "POINT": [
                    "POINT(0.38 0.10)",
                    "POINT(0.47 0.41)"
                ],
                "LINESTRING": "LINESTRING(0.070.21,0.360.245,0.580.16,0.970.27)",
                "POLYGON": [
                    "POLYGON((0.0480.357,0.1660.0725,0.3930.0075,0.3920.202,0.2420.375))",
                    "POLYGON((0.5130.232,0.790.1075,0.9280.102,0.9530.64,0.7590.89,0.510.245))",
                    "POLYGON((0.1150.497,0.5920.82,0.5810.917,0.140.932))"
                ]
            }    
        ** 解析请调用'handleArgsJson'函数
        */
        BoostInterface bi(outMat.size());
        if (!args.empty())
        {   
            #if 1
            /* 模拟args的处理，用'|'分隔 */
            m_cid.clear();
            handleArgs(args, bi);
            #endif
    
            #if 0
            /* 模拟args的处理，json格式 */
            m_cid.clear();
            handleArgs(args, bi);
            #endif
        }
    
    
        /* 算法实现     ??? */
        /* 注意: 算法请使用outMat */
    
        /* 
        ** for example:
            Classifier * pc = (Classifier *)m_predictor;
            std::vector<Prediction> predictions = pc->Classify(outMat);
        **
        */
        
        /* 
        ** 模拟算法返回的结果,实际不同 ???
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
        strJson = (rand() % 2)?json1:json2;
    
        /* 
        ** 过滤出在有效区域内的目标 ??? 
        ** bi.inPolygons,示例详见3rd/boost_interface/main.cpp 
        ** 示例: if (!bi.empty()) { bi.inPolygons ... }       
        */
    
    
        /* 根据算法配置及算法返回结果,绘制输出结果,包括绘制感兴趣多边形区域 ??? */
        
        /*
        ** 绘制感兴趣多边形区域 
        ** for exaple
        ** 根据算法配置draw_roi_area,绘制args,可能包括点，线，框
        */
        if (m_draw_roi_area)
        {
            //绘制多边形区域(可能多个)
            const Vector_Polygon& v_polygons = bi.getPolygons();
            if (v_polygons.size() > 0)
            {
                cv::polylines(outMat, v_polygons, true, cv::Scalar(0, 255, 0), 3, cv::LINE_8, 0);
            }
    
            //绘制线(可能多个)
            const Vector_Line& v_lines = bi.getLines();
            if (v_lines.size() > 0)
            {
                cv::polylines(outMat, v_lines, false, cv::Scalar(0, 255, 0), 3, cv::LINE_8, 0);
            }
    
            //汇制点(可能多个)
            const Vector_Point& v_points = bi.getPoints();
            if (v_points.size() > 0)
            {
                for(auto iter = v_points.cbegin(); iter != v_points.cend(); iter++)
                {
                    cv::circle(outMat, *iter, 3, cv::Scalar(0, 255, 0), cv::FILLED, cv::LINE_8, 0);
                }
            }
        }
    
        /*
        ** 绘制目标区域 ???
        ** 例如:   if (m_draw_result) { to do list ...}
        */
        #if 0
        if (m_draw_result)
        {
            cv::rectangle(outMat,  det_vec[i].rect, cv::Scalar(0,0,255),1,1,0);
            cv::putText(outMat,"object",cv::Point(det_vec[i].rect.x, det_vec[i].rect.y),
                        cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0,255),2,8,0);
        }
        #endif
    
        //实时显示结果
        if (m_show_result)
        {
            cv::imshow("result", outMat);
            cv::waitKey(1);
        }  
    
        return true;
    }
    
    /* 读取算法配置项 */
    bool readAlgoConfig(const std::string& srcFile)
    {
        /* 读取文容到     'buffer' */
        std::ifstream ifs(srcFile.c_str(),std::ifstream::binary);
        if (!ifs.is_open())
        {
            LOG(ERROR) << "open algo config file failed:  " << srcFile;
            return false;
        }

        std::filebuf *fbuf = ifs.rdbuf();
        std::size_t size = fbuf->pubseekoff(0,ifs.end,ifs.in);
        fbuf->pubseekpos(0,ifs.in);

        char* buffer = new char[size+1];
        fbuf->sgetn(buffer,size);
        ifs.close();
        buffer[size] = '\0';

        /* 解析配置选项 json格式 */
        cJSON *jsonRoot = cJSON_Parse(buffer);
        if (jsonRoot == NULL)
        {
            LOG(WARNING) << "parse algo config file failed: "<< buffer;
            if (buffer) delete [] buffer;
            return false;
        }

        cJSON *sub = cJSON_GetObjectItem(jsonRoot, "draw_roi_area");
        if (sub && (sub->type == cJSON_Number))
        {
            m_draw_roi_area = sub->valueint;
        }

        sub = cJSON_GetObjectItem(jsonRoot, "draw_result");
        if (sub && (sub->type == cJSON_Number))
        {
            m_draw_result = sub->valueint;
        }

        sub = cJSON_GetObjectItem(jsonRoot,"show_result");
        if (sub && (sub->type == cJSON_Number))
        {
            m_show_result = sub->valueint;
        }

        sub = cJSON_GetObjectItem(jsonRoot, "gpu_id");
        if (sub && (sub->type == cJSON_Number))
        {
            m_gpu_id = sub->valueint;
        }

        sub = cJSON_GetObjectItem(jsonRoot,"threshold_value");
        if (sub && (sub->type == cJSON_Number))
        {
            m_threshold_value = sub->valuedouble;
        }

        if (jsonRoot) cJSON_Delete(jsonRoot);
        if (buffer) delete [] buffer;

        return true;
    }
        
    /* 判断是否报警       */
    inline bool checkAlarm(const std::string& content) 
    {
        /*
        ** 提供以下四种实方法，实际开发根据需求选择某一个实现即可 ???
        ** 1.非报警类算法，直接返回true，或者在调用处直接修改为:'event->code = JISDK_CODE_ALARM;'
        ** 2.从算法返回的json数据中查找报警时的常量字符串
        ** 3.解析算法返回的json数据
        ** 4.calc函数调用算法时多个参数传出来
        */
        
        #if 0
        /* 非报警类算法，请直接返回true */
        return true;
        #endif

        
        #if 0        
        /* 实现方案1:     直接查找报警常量字符串--较偷赖的方式 */
        return (content.find("\"alertFlag\": 1,") != std::string::npos);
        #endif


        #if 1
        /* 实现方案:     解析json--较安全正确的方式，建议选用 */
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
        /* 实现方案: calc函数调用算法时多个参数传出来--较高效的方式 */
        #endif
    }

    /* 解析args   用'|'分隔 */
    inline bool handleArgs(const std::string &args, BoostInterface &bi)
    {
        #if 1
        /* 仅有可选的roi信息 */
        return bi.parsePolygon(args);
        #endif
            
        #if 0
        /* 用'|'分隔各类信息 */
        size_t idx = 0;
        std::string strItem;
        for (size_t i = 0; i < args.size(); ++i)
        {
            if (args[i] == '|')
            {
                strItem = args.substr(idx, i - idx);
                idx = i + 1;
                parseArgs(strItem,bi);
            }
        }
        strItem = args.substr(idx, args.size() - idx);
        parseArgs(strItem,bi);

        return true;
        #endif
    }

    /* 解析args   json格式 */
    inline bool handleArgsJson(const std::string &args, BoostInterface &bi)
    {
        /*
            {
                "cid": "1000",
                "POINT": [
                    "POINT(0.38 0.10)",
                    "POINT(0.47 0.41)"
                ],
                "LINESTRING": "LINESTRING(0.070.21,0.360.245,0.580.16,0.970.27)",
                "POLYGON": [
                    "POLYGON((0.0480.357,0.1660.0725,0.3930.0075,0.3920.202,0.2420.375))",
                    "POLYGON((0.5130.232,0.790.1075,0.9280.102,0.9530.64,0.7590.89,0.510.245))",
                    "POLYGON((0.1150.497,0.5920.82,0.5810.917,0.140.932))"
                ]
            }

        */  
        cJSON *root, *sub, *item;
        root = cJSON_Parse(args.c_str());
        if (root == NULL)
        {
            LOG(WARNING) << "cJSON_Parse failed, " << args;
            return false;
        }

        sub = cJSON_GetObjectItem(root,"cid");
        if (sub && sub->type == cJSON_String)
        {
            m_cid = sub->valuestring;
        }

        sub = cJSON_GetObjectItem(root,"POINT");
        if (sub)
        {
            if (sub->type == cJSON_String)
            {
                bi.parsePoint(sub->valuestring);
            }
            else if (sub->type == cJSON_Array)
            {
                int n = cJSON_GetArraySize(sub);
                for (int i = 0; i < n; i++)
                {
                    item = cJSON_GetArrayItem(sub,i);
                    if (item && item->type == cJSON_String)
                    {
                         bi.parsePoint(item->valuestring);
                    }
                }
            }
        }
        
        sub = cJSON_GetObjectItem(root,"LINESTRING");
        if (sub)
        {
            if (sub->type == cJSON_String)
            {
                bi.parseLinestring(sub->valuestring);
            }
            else if (sub->type == cJSON_Array)
            {
                int n = cJSON_GetArraySize(sub);
                for (int i = 0; i < n; i++)
                {
                    item = cJSON_GetArrayItem(sub,i);
                    if (item && item->type == cJSON_String)
                    {
                         bi.parseLinestring(item->valuestring);
                    }
                }
            }
        }

        sub = cJSON_GetObjectItem(root,"POLYGON");
        if (sub)
        {
            if (sub->type == cJSON_String)
            {
                bi.parsePolygon(sub->valuestring);
            }
            else if (sub->type == cJSON_Array)
            {
                int n = cJSON_GetArraySize(sub);
                for (int i = 0; i < n; i++)
                {
                    item = cJSON_GetArrayItem(sub,i);
                    if (item && item->type == cJSON_String)
                    {
                         bi.parsePolygon(item->valuestring);
                    }
                }
            }
        }

        cJSON_Delete(root);
        return true;
    }

    /* 用于解析wkt信息       */
    inline bool parseArgs(const std::string &strItem, BoostInterface &bi)
    {
        if (strItem.compare(0,4,"cid=") == 0)
        {
            m_cid = strItem.substr(4);
            return true;
        }

        if (strItem.compare(0,7,"POLYGON") == 0)
        {
            return bi.parsePolygon(strItem);
        }
        else if (strItem.compare(0,10,"LINESTRING") == 0) 
        {
            return bi.parseLinestring(strItem);
        }
        else if (strItem.compare(0,5,"POINT") == 0)
        {
            return bi.parsePoint(strItem);
        }
        else 
        {
            return false;
        }
    }
    
private:
    int m_pbtype;
    void *m_predictor;

    cv::Mat m_mat;
    std::string m_json;
    std::string m_cid;

    /* 算法配置选项 */
    int m_draw_roi_area;
    int m_draw_result;
    int m_show_result;
    int m_gpu_id;
    double m_threshold_value;
};

int ji_init(int argc, char **argv)
{
/* 
** 参数说明
for example:
argc：6
argv[0]: $license
argv[1]: $url
argv[2]: $activation
argv[3]: $timestamp
argv[4]: $qps
argv[5]: $version
...
**
*/
    if (argc < 6 )
    {
        return JISDK_RET_INVALIDPARAMS;
    }

    if (argv[0] == NULL || 
        argv[5] == NULL)
    {
        return JISDK_RET_INVALIDPARAMS;
    }

    int qps = 0;
    if (argv[4]) qps = atoi(argv[4]);

    /* 
    ** 请替换算法的公钥,oneKeySdk.sh会自动生成
    for example:
    cd /usr/local/ev_sdk/bin && ./ev_codec -c pubKey.pem ../src/pubKey.hpp && sed -i "s|key|pubKey|g" ../src/pubKey.hpp
    ** 
    */
    return (ji_check_license(pubKey, argv[0], argv[1], argv[2],
            argv[3], qps>0?&qps:NULL, atoi(argv[5])) == EV_SUCCESS)?
            JISDK_RET_SUCCEED:
            JISDK_RET_UNAUTHORIZED;
}

void ji_reinit()
{
    ji_check_license(NULL,NULL,NULL,NULL,NULL,NULL,0);
}


void* ji_create_predictor(int pdtype)
{
    if (ji_check_expire_only() != EV_SUCCESS)
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
        timestamp = vcapture.get(cv::CAP_PROP_POS_MSEC);
        
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
                                    /*vcapture.get(cv::CAP_PROP_FOURCC)*/cv::VideoWriter::fourcc('X','2','6','4'),
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
