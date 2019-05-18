#include <fstream>
#include <getopt.h>
#include <string>

#include <opencv2/opencv.hpp> 
#include <glog/logging.h>

#include "ji.h"
#include "cJSON.h"

using namespace std;

#define EMPTY_EQ_NULL(a)  ((a).empty())?NULL:(a).c_str()

string strFunction,strLicense,strIn,strArgs,strOut;
int repeats  = 1;
void *predictor = NULL;

int check_filetype(const string &fielname)
{
    int filetype = 0; //0:image; 1:video
    
    std::size_t found = fielname.rfind('.');
    if (found!=std::string::npos)
    {
        string strExt = fielname.substr(found);
        if (strExt.compare(".mp4") == 0 ||
            strExt.compare(".avi") == 0 ||
            strExt.compare(".flv") == 0 || 
            strExt.compare(".h264") == 0)
        {
            filetype = 1;
        }
    }

    return filetype;
}

bool read_license(const string& srcFile, string& license, string& timestamp, int& qps, int& version)
{
    std::ifstream ifs(srcFile.c_str(),std::ifstream::binary);
    if (!ifs.is_open())
    {
        LOG(ERROR) << "[ERROR] open license file failed.";
        return false;
    }
    
    std::filebuf *fbuf = ifs.rdbuf();

    //get file size
    std::size_t size = fbuf->pubseekoff(0,ifs.end,ifs.in);
    fbuf->pubseekpos(0,ifs.in);

    //get file data
    char* buffer = new char[size+1];
    fbuf->sgetn(buffer,size);
    ifs.close();
    buffer[size] = '\0';

    cJSON *jsonRoot = NULL, *sub;
    bool bRet = false;
    do 
    {
        //parse json 
        jsonRoot = cJSON_Parse(buffer);
        if (jsonRoot == NULL)
        {
            LOG(ERROR) << "[ERROR] parse license file failed.";
            break;
        }

        sub = cJSON_GetObjectItem(jsonRoot, "license");
        if (sub == NULL || sub->type != cJSON_String)
        {
            LOG(ERROR) << "[ERRROR] invalid license, json: %s" << buffer;
            break;
        }
        license = sub->valuestring;

        sub = cJSON_GetObjectItem(jsonRoot, "timestamp");
        if (sub && sub->type == cJSON_String)
        {
            timestamp = sub->valuestring;
        }

        sub = cJSON_GetObjectItem(jsonRoot,"qps");
        if (sub && (sub->type == cJSON_Number))
        {
            qps = sub->valueint;
        }

        sub = cJSON_GetObjectItem(jsonRoot, "version");
        if (sub == NULL || sub->type != cJSON_Number)
        {
            LOG(ERROR) << "[ERRROR] invalid version, json: " << buffer;
            break;
        }
        version = sub->valueint;

        bRet = true;
    } while (0);

    if (jsonRoot) cJSON_Delete(jsonRoot);
    if (buffer) delete [] buffer;
    return bRet;
}

void signal_handle(const char* data, int size)
{
    std::ofstream fs("core_dump.log",std::ios::app);
    std::string str = std::string(data,size);

    fs<<str;
    LOG(ERROR)<<str;

    fs.close();
}

void show_help()
{
    LOG(INFO) << "\n"
    << "---------------------------------\n"
    << "usage:\n"
    << "  -h  --help        show help information\n"
    << "  -f  --function    test function for \n"
    << "                    [1.ji_calc_frame,2.ji_calc_buffer,3.ji_calc_file,4.ji_calc_video_file]\n"
    << "  -l  --license     license file. default: license.txt\n"
    << "  -i  --infile      source file\n"
    << "  -a  --args        for example roi\n"
    << "  -o  --outfile     result file\n"
    << "  -r  --repeat      number of repetitions. default: 1\n"
    << "                    <= 0 represents an unlimited number of times\n"
    << "                    for example: -r 100\n"
    << "---------------------------------\n";
}

void test_for_ji_calc_frame()
{
    int filetype = check_filetype(strIn);

    /* image-file */
    if (filetype == 0)    
    {
        JI_CV_FRAME inframe,outframe;
        JI_EVENT event;
        cv::Mat inMat = cv::imread(strIn);
        if (inMat.empty())
        {
            LOG(ERROR) << "[ERROR] cv::imread source file failed, " << strIn;
            return;
        }
        inframe.rows = inMat.rows;
        inframe.cols = inMat.cols;
        inframe.step = inMat.step;
        inframe.data = inMat.data;
        inframe.type = inMat.type();

        int iRet;
        int count = 0;
        while ((repeats == -1) || (count < repeats))
        {
            ++count;
            if (repeats == -1 || repeats > 1)
                LOG(INFO) << "repeat: " << count;

            iRet = ji_calc_frame(predictor, &inframe, EMPTY_EQ_NULL(strArgs), &outframe, &event);
            LOG(INFO) << "call ji_calc_frame, return " << iRet;

            if (iRet == JISDK_RET_SUCCEED)
            {
                LOG(INFO) << "event info:"
                     << "\n\tcode: " << event.code
                     << "\n\tjson: " << event.json;

                if (event.code != JISDK_CODE_FAILED)
                {
                    if (!strOut.empty())
                    {
                        cv::Mat outMat(outframe.rows,outframe.cols,outframe.type,outframe.data,outframe.step);
                        cv::imwrite(strOut,outMat);
                    }
                }
            }
       } //end_while

       return ;
    }


    /* video-file */
    int count = 0;
    while ((repeats == -1) || (count < repeats))
    {
        ++count;
        if (repeats == -1 || repeats > 1)
            LOG(INFO) << "repeat: " << count;

        cv::VideoCapture vcapture(strIn);
        if (!vcapture.isOpened())
        {
            LOG(ERROR) << "[ERROR] cv::VideoCapture,open video file failed, " << strIn;
            break;;
        }
        
        cv::VideoWriter vwriter;

        cv::Mat inMat;
        int iRet;
        JI_CV_FRAME inframe,outframe;
        JI_EVENT event;
        while (vcapture.read(inMat))
        {
            inframe.rows = inMat.rows;
            inframe.cols = inMat.cols;
            inframe.step = inMat.step;
            inframe.data = inMat.data;
            inframe.type = inMat.type();
        
            iRet = ji_calc_frame(predictor, &inframe, EMPTY_EQ_NULL(strArgs), &outframe, &event);
            LOG(INFO) << "call ji_calc_frame, return " << iRet;
            
            if (iRet == JISDK_RET_SUCCEED)
            {
                LOG(INFO) << "event info:"
                     << "\n\tcode: " << event.code
                     << "\n\tjson: " << event.json;

                 if (event.code != JISDK_CODE_FAILED)
                 {
                     cv::Mat outMat(outframe.rows,outframe.cols,outframe.type,outframe.data,outframe.step);
                     if (!strOut.empty())
                     {
                         if (!vwriter.isOpened())
                         {
                             vwriter.open(strOut,
                                         /*vcapture.get(cv::CAP_PROP_FOURCC)*/cv::VideoWriter::fourcc('x','2','6','4'),
                                         vcapture.get(cv::CAP_PROP_FPS),
                                         outMat.size());
                             if (!vwriter.isOpened())
                             {
                                 LOG(ERROR) << "[ERROR] cv::VideoWriter,open video file failed, " << strOut;
                                 break;;
                             }
                         }
                         vwriter.write(outMat);
                     }
                 }
            }
        } // end_while_vcapture.read(inMat)

        vwriter.release();
        vcapture.release();
    } //end_while
}

void test_for_ji_calc_buffer()
{
    std::ifstream ifs(strIn.c_str(),std::ifstream::binary);
    if (!ifs.is_open())
    {
        LOG(ERROR) << "[ERROR] open source file failed.";
        return;
    }

    std::filebuf *fbuff = ifs.rdbuf();
    std::size_t size = fbuff->pubseekoff(0,ifs.end,ifs.in);
    fbuff->pubseekpos(0,ifs.in);
    
    char* buffer = new char[size+1];
    fbuff->sgetn(buffer,size);
    ifs.close();

    JI_EVENT event;
    int iRet;
    int count = 0;
    while ((repeats == -1) || (count < repeats))
    {
        ++count;
        if (repeats == -1 || repeats > 1)
            LOG(INFO) << "repeat: " << count;
    
        iRet = ji_calc_buffer(predictor, buffer, size, EMPTY_EQ_NULL(strArgs), 
                              EMPTY_EQ_NULL(strOut), &event);
        LOG(INFO) << "call ji_calc_buffer, return " << iRet;

        if (iRet == JISDK_RET_SUCCEED)
        {
            LOG(INFO) << "event info:"
                      << "\n\tcode: " << event.code
                      << "\n\tjson: " << event.json;
        }
    } //end_while
    
    delete [] buffer;
}

void test_for_ji_calc_file()
{
    JI_EVENT event;
    int iRet;
    int count = 0;
    while ((repeats == -1) || (count < repeats))
    {
        ++count;
        if (repeats == -1 || repeats > 1)
            LOG(INFO) << "repeat: " << count;

        iRet = ji_calc_file(predictor,strIn.c_str(), EMPTY_EQ_NULL(strArgs), 
                            EMPTY_EQ_NULL(strOut), &event);
        LOG(INFO) << "call ji_calc_file, return " << iRet;
        
        if (iRet == JISDK_RET_SUCCEED)
        {
            LOG(INFO) << "event info:"
                      << "\n\tcode: " << event.code
                      << "\n\tjson: " << event.json;
        }
    } //end_while    
}

void test_for_ji_calc_video_file()
{
    string strJson;
    if (!strOut.empty())
    {
        strJson = strOut + ".txt";
    }

    int iRet;
    int count = 0;
    while ((repeats == -1) || (count < repeats))
    {
        ++count;
        if (repeats == -1 || repeats > 1)
            LOG(INFO) << "repeat: " << count;

        iRet = ji_calc_video_file(predictor,strIn.c_str(), EMPTY_EQ_NULL(strArgs), 
                                  EMPTY_EQ_NULL(strOut), EMPTY_EQ_NULL(strJson));
        LOG(INFO) << "call ji_calc_video_file, return " << iRet;

        if (iRet == JISDK_RET_SUCCEED)
        {
            LOG(INFO) << "result info:"
                      << "\n\tout_video_file: " << strOut
                      << "\n\tout_json_file: "  << strJson;
        }
    } //end_while
}

#if 0
void cJSON_sample()
{
/*
{
    "remark":   "sample",
    "data": [{
            "NO":   1,
            "gender":   "male",
            "age":  35,
            "child":    [{
                    "SUBNO":    1,
                    "gender":   "male",
                    "age":  5
                }, {
                    "SUBNO":    2,
                    "gender":   "female",
                    "age":  10
                }]
        }, {
            "NO":   2,
            "gender":   "male",
            "age":  40
        }, {
            "NO":   3,
            "gender":   "male",
            "age":  80,
            "child":    [{
                    "SUBNO":    1,
                    "gender":   "male",
                    "age":  50
                }, {
                    "SUBNO":    2,
                    "gender":   "female",
                    "age":  55
                }]
        }]
}
*/
    /* add sample */
    cJSON *jsonRoot = cJSON_CreateObject();

    // add "remark"    
    cJSON_AddItemToObjectCS(jsonRoot, "remark", cJSON_CreateString("sample"));

    /* begin add "data" array */
    cJSON *jsonData = cJSON_CreateArray();


    /* begin add "data.object1" */
    cJSON *jsonDataObject1 = cJSON_CreateObject();
    cJSON_AddItemToObjectCS(jsonDataObject1,"NO",cJSON_CreateNumber(1));
    cJSON_AddItemToObjectCS(jsonDataObject1,"gender",cJSON_CreateString("male"));
    cJSON_AddItemToObjectCS(jsonDataObject1,"age",cJSON_CreateNumber(35));

    // object1_child1
    cJSON *jsonData_object1_child1 = cJSON_CreateObject();
    cJSON_AddItemToObjectCS(jsonData_object1_child1,"SUBNO",cJSON_CreateNumber(1));
    cJSON_AddItemToObjectCS(jsonData_object1_child1,"gender",cJSON_CreateString("male"));
    cJSON_AddItemToObjectCS(jsonData_object1_child1,"age",cJSON_CreateNumber(5));

    // object1_child2
    cJSON *jsonData_object1_child2 = cJSON_CreateObject();
    cJSON_AddItemToObjectCS(jsonData_object1_child2,"SUBNO",cJSON_CreateNumber(2));
    cJSON_AddItemToObjectCS(jsonData_object1_child2,"gender",cJSON_CreateString("female"));
    cJSON_AddItemToObjectCS(jsonData_object1_child2,"age",cJSON_CreateNumber(10));

    cJSON *jsonData_object1_child = cJSON_CreateArray();
    cJSON_AddItemToArray(jsonData_object1_child,jsonData_object1_child1);
    cJSON_AddItemToArray(jsonData_object1_child,jsonData_object1_child2);

    cJSON_AddItemToObjectCS(jsonDataObject1,"child", jsonData_object1_child); 
    cJSON_AddItemToArray(jsonData,jsonDataObject1);
    /* end add "data.object1" */


    /* begin add "data.object2" */
    cJSON *jsonDataObject2 = cJSON_CreateObject();
    cJSON_AddItemToObjectCS(jsonDataObject2,"NO",cJSON_CreateNumber(2));
    cJSON_AddItemToObjectCS(jsonDataObject2,"gender",cJSON_CreateString("male"));
    cJSON_AddItemToObjectCS(jsonDataObject2,"age",cJSON_CreateNumber(40));
    cJSON_AddItemToArray(jsonData,jsonDataObject2);
    /* end add "data.object2" */

    
    /* begin add "data.object3" */
    cJSON *jsonDataObject3 = cJSON_CreateObject();
    cJSON_AddItemToObjectCS(jsonDataObject3,"NO",cJSON_CreateNumber(3));
    cJSON_AddItemToObjectCS(jsonDataObject3,"gender",cJSON_CreateString("male"));
    cJSON_AddItemToObjectCS(jsonDataObject3,"age",cJSON_CreateNumber(80));

    // object1_child1
    cJSON *jsonData_object3_child1 = cJSON_CreateObject();
    cJSON_AddItemToObjectCS(jsonData_object3_child1,"SUBNO",cJSON_CreateNumber(1));
    cJSON_AddItemToObjectCS(jsonData_object3_child1,"gender",cJSON_CreateString("male"));
    cJSON_AddItemToObjectCS(jsonData_object3_child1,"age",cJSON_CreateNumber(50));

    // object1_child2
    cJSON *jsonData_object3_child2 = cJSON_CreateObject();
    cJSON_AddItemToObjectCS(jsonData_object3_child2,"SUBNO",cJSON_CreateNumber(2));
    cJSON_AddItemToObjectCS(jsonData_object3_child2,"gender",cJSON_CreateString("female"));
    cJSON_AddItemToObjectCS(jsonData_object3_child2,"age",cJSON_CreateNumber(55));

    cJSON *jsonData_object3_child = cJSON_CreateArray();
    cJSON_AddItemToArray(jsonData_object3_child,jsonData_object3_child1);
    cJSON_AddItemToArray(jsonData_object3_child,jsonData_object3_child2);

    cJSON_AddItemToObjectCS(jsonDataObject3,"child", jsonData_object3_child);  
    cJSON_AddItemToArray(jsonData,jsonDataObject3);
    /* end add "data.object3" */

    cJSON_AddItemToObjectCS(jsonRoot,"data",jsonData);

    /* print sample */
    char *buff = cJSON_Print(jsonRoot); 
    LOG(INFO) << buff;
    free(buff);

    /* free cJSON sampee*/
    cJSON_Delete(jsonRoot);
}

void jsoncpp_sample()
{
/*
{
  "remark":   "sample",
  "data": [{
          "NO":   1,
          "gender":   "male",
          "age":  35,
          "child":    [{
                  "SUBNO":    1,
                  "gender":   "male",
                  "age":  5
              }, {
                  "SUBNO":    2,
                  "gender":   "female",
                  "age":  10
              }]
      }, {
          "NO":   2,
          "gender":   "male",
          "age":  40
      }, {
          "NO":   3,
          "gender":   "male",
          "age":  80,
          "child":    [{
                  "SUBNO":    1,
                  "gender":   "male",
                  "age":  50
              }, {
                  "SUBNO":    2,
                  "gender":   "female",
                  "age":  55
              }]
  		}]
}
*/

    /* 定义 */
    Json::Value value;
    Json::Value data;
    Json::Value data_array;
    Json::Value child;
    Json::Value child_array;

    data_array["NO"] = Json::Value(1); //添加内容
    data_array["gender"] = Json::Value("male");
    data_array["age"] = Json::Value(35);

    child_array["SUBNO"] = Json::Value(1);
    child_array["gender"] = Json::Value("male");
    child_array["age"] = Json::Value(5);
    child.append(child_array);     //添加数组

    child_array.clear();//清空内容
    child_array["SUBNO"] = Json::Value(2);
    child_array["gender"] = Json::Value("female");
    child_array["age"] = Json::Value(10);
    child.append(child_array);
    data_array["child"] = child;//添加数组内容

    data.append(data_array);
    child.clear();
    data_array.clear();
    data_array["NO"] = Json::Value(2);
    data_array["gender"] = Json::Value("male");
    data_array["age"] = Json::Value(40);
    data.append(data_array);

    data_array.clear();
    data_array["NO"] = Json::Value(3);
    data_array["gender"] = Json::Value("male");
    data_array["age"] = Json::Value(80);

    child_array["SUBNO"] = Json::Value(1);
    child_array["gender"] = Json::Value("male");
    child_array["age"] = Json::Value(50);
    child.append(child_array);

    child_array.clear();
    child_array["SUBNO"] = Json::Value(2);
    child_array["gender"] = Json::Value("female");
    child_array["age"] = Json::Value(55);
    child.append(child_array);
    data_array["child"] = child;
    data.append(data_array);
    value["data"] = data;
    value["remark"] = Json::Value("sample");

    std::string out = value.toStyledString();
    LOG(INFO) << out;
}
#endif

int main(int argc, char *argv[])
{
    //glog
    google::InitGoogleLogging(argv[0]);
    FLAGS_minloglevel = google::INFO;
    FLAGS_stderrthreshold=google::INFO;
    FLAGS_colorlogtostderr=true;

    /* 如需要保存日志到文件test.INFO,请启用下一行代码 */
    //google::SetLogDestination(google::GLOG_INFO, "info");

    //capture exception
    google::InstallFailureSignalHandler();
    google::InstallFailureWriter(&signal_handle);

    LOG(INFO) << "EV_SDK_VERSION: " << EV_SDK_VERSION;

    //parse params
    const char *short_options = "hf:l:i:a:o:r:";  
    const struct option long_options[] = {  
         { "help",     0,   NULL,    'h'},
         { "function", 1,   NULL,    'f'},  
         { "license",  1,   NULL,    'l'},
         { "infile",   1,   NULL,    'i'},  
         { "args",     1,   NULL,    'a'},  
         { "outfile",  1,   NULL,    'o'}, 
         { "repeat",   1,   NULL,    'r'}, 
         {      0,     0,      0,      0}
    }; 

    bool bShowHelp = false;
    int c;
    int option_index = 0;    
    do
    {    
        c = getopt_long(argc, argv, short_options,long_options,&option_index);
        if (c == -1) break;

        switch (c)
        {
            case 'h':
                bShowHelp = true;
                break;

            case 'f':
                strFunction = optarg;
                break;

            case 'l':
                strLicense = optarg;
                break;

            case 'i':
                strIn = optarg;
                break;

            case 'a':
                strArgs = optarg;
                break;

            case 'o':
                strOut = optarg;
                break;

            case 'r':
                repeats = atoi(optarg);
                break;

            default:
                break;
        }
    } while (1);

    if (bShowHelp)
    {
        show_help();
        return 0;
    }

    //check params
    c = -1;
    if (strFunction.compare("ji_calc_frame") == 0 || strFunction.compare("1") == 0)
        c = 0;
    else if (strFunction.compare("ji_calc_buffer") == 0   || strFunction.compare("2") == 0)
        c = 1;
    else if (strFunction.compare("ji_calc_file")   == 0   || strFunction.compare("3") == 0)
        c = 2;
    else if (strFunction.compare("ji_calc_vido_file") == 0 || strFunction.compare("4") == 0)
        c = 3;
        
    if (c == -1)
    {
        LOG(ERROR) << "[ERROR] invalid function.";
        show_help();
        return -1;
    }

    if (strLicense.empty())
    {
        strLicense = "license.txt";
    }
    
    if (strIn.empty())
    {
        LOG(ERROR) << "[ERROR] no infile.";
        show_help();
        return -1;
    }

    if (repeats <= 0) repeats = -1;
    
    //print params
    static const string cs_function[4] = {
        "1.ji_calc_frame",
        "2.ji_calc_buffer",
        "3.ji_calc_file",
        "4.ji_calc_video_file"
    };
    LOG(INFO) << "run params info:" 
              << "\n\tfuction: "   << cs_function[c]
              << "\n\tlicense: "   << strLicense
              << "\n\tinfile: "    << strIn 
              << "\n\targs: "      << strArgs 
              << "\n\toutfile: "   << strOut      
              << "\n\trepeat:"     << repeats;

    //read license & check license
    string l,ts;
    int v,qps;
    v = qps = 0;
    if (!read_license(strLicense,l,ts,qps,v))
    {
        LOG(ERROR) << "[ERROR] read_license faild.";
        return -1;
    }

    char strQps[64] = {0};
    sprintf(strQps,"%d",qps);
    
    char strVersion[64] = {0};
    sprintf(strVersion,"%d",v);
    
    LOG(INFO) << "license info:"
              << "\n\tlicense: "     << l
              << "\n\ttimestamp: "   << ts
              << "\n\tqps: "         << (qps>0?strQps:"")
              << "\n\tversion:"      << v;

    int ac = 0;
    char *av[4];
    av[ac++] = const_cast<char*>(l.c_str());
    av[ac++] = ts.empty()?NULL:const_cast<char*>(ts.c_str());    
    av[ac++] = qps>0?strQps:NULL;
    av[ac++] = strVersion;
    int iRet = ji_init(ac,av);
    if (iRet != JISDK_RET_SUCCEED) 
    {
        LOG(ERROR) << "[ERROR] ji_init faild, return " << iRet;
        return -1;
    }

    //create predictor
    predictor = ji_create_predictor(JISDK_PREDICTOR_DEFAULT);
    if (predictor == NULL)
    {
        LOG(ERROR) << "[ERROR] ji_create_predictor faild, return NULL";
        return -1;
    }

    //test 
    switch (c)
    {
        //ji_calc_frame
        case 0:
            test_for_ji_calc_frame();
            break;

        //ji_calc_buffer
        case 1:
            test_for_ji_calc_buffer();
            break;

        //ji_calc_file
        case 2:
            test_for_ji_calc_file();
            break;
        
        //ji_calc_video_file
        case 3:
            test_for_ji_calc_video_file();
            break;
        
        default: break;
    }

    ji_destroy_predictor(predictor);
    return iRet; 
}
