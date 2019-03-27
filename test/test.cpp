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
int repeats = 1;
void *predictor = NULL;


bool read_license(const string& srcFile, string& license, string& timestamp, int& qps, int& version)
{
    std::ifstream ifs(srcFile.c_str(),std::ifstream::binary);
    if (!ifs.is_open())
    {
        cerr << "[ERROR] open license file failed." << endl;
        return false;
    }
    
    std::filebuf *fbuf = ifs.rdbuf();

    //get file size
    std::size_t size = fbuf->pubseekoff (0,ifs.end,ifs.in);
    fbuf->pubseekpos(0,ifs.in);

    //get file data
    char* buffer = new char[size+1];
    fbuf->sgetn (buffer,size);
    ifs.close();
    buffer[size] = '\0';

    cJSON *json = NULL, *sub;
    bool bRet = false;
    do 
    {
        //parse json 
        cJSON *json = cJSON_Parse(buffer);
        if (json == NULL)
        {
            cerr << "[ERROR] parse license file failed." << endl;
            break;
        }

        sub = cJSON_GetObjectItem(json, "license");
        if (sub == NULL || sub->type != cJSON_String)
        {
            cerr << "[ERRROR] invalid license, json: %s" << buffer << endl;
            break;
        }
        license = sub->valuestring;

        sub = cJSON_GetObjectItem(json, "timestamp");
        if (sub && sub->type == cJSON_String)
        {
            timestamp = sub->valuestring;
        }

        sub = cJSON_GetObjectItem(json,"qps");
        if (sub && (sub->type == cJSON_Number))
        {
            qps = sub->valueint;
        }

        sub = cJSON_GetObjectItem(json, "version");
        if (sub == NULL || sub->type != cJSON_Number)
        {
            cerr << "[ERRROR] invalid version, json: " << buffer << endl;
            break;
        }
        version = sub->valueint;

        bRet = true;
    } while (0);

    if (json) cJSON_Delete(json);
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
    cout << "---------------------------------" << endl;
    cout << "usage:" << endl;
    cout << "  -h  --help        show help information" << endl;
    cout << "  -f  --function    test function for \n";
    cout << "                    [1.ji_calc_frame,2.ji_calc_buffer,3.ji_calc_file,4.ji_calc_video_file]" << endl;
    cout << "  -l  --license     license file. default: license.txt"  << endl;
    cout << "  -i  --infile      source file"     << endl;
    cout << "  -a  --args        for example roi" << endl;
    cout << "  -o  --outfile     result file"     << endl;
    cout << "  -r  --repeat      number of repetitions. default: 1"              << endl;
    cout << "                    <= 0 represents an unlimited number of times"   << endl;
    cout << "                    for example: -r 100" << endl;
    cout << "---------------------------------"   << endl;
}

void test_for_ji_calc_frame()
{
    JI_CV_FRAME inframe,outframe;
    JI_EVENT event;

    cv::Mat inMat = cv::imread(strIn);
    if (inMat.empty())
    {
        cerr << "[ERROR] cv::imread source file failed, " << strIn << endl;
        return;
    }
    inframe.rows = inMat.rows;
    inframe.cols = inMat.cols;
    inframe.step = inMat.step;
    inframe.data = inMat.data;
    inframe.type = inMat.type();

    int iRet;
    int count = 0;
    while ((repeats == -1) || (count++ < repeats))
    {
        if (repeats == -1 || repeats > 1)
            cout << "repeat: " << count << endl;

        iRet = ji_calc_frame(predictor, &inframe, EMPTY_EQ_NULL(strArgs), &outframe, &event);
        cout << "call ji_calc_frame, return " << iRet << endl;

        if (iRet == JISDK_RET_SUCCEED)
        {
            cout << "event info:"
                 << "\n\tcode: " << event.code
                 << "\n\tjson: " << event.json << endl;

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
}

void test_for_ji_calc_buffer()
{
    std::ifstream ifs(strIn.c_str(),std::ifstream::binary);
    if (!ifs.is_open())
    {
        cerr << "[ERROR] open source file failed." << endl;
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
    while ((repeats == -1) || (count++ < repeats))
    {
        if (repeats == -1 || repeats > 1)
            cout << "repeat: " << count << endl;
    
        iRet = ji_calc_buffer(predictor, buffer, size, EMPTY_EQ_NULL(strArgs), 
                              EMPTY_EQ_NULL(strOut), &event);
        cout << "call ji_calc_buffer, return " << iRet << endl;

        if (iRet == JISDK_RET_SUCCEED)
        {
            cout << "event info:"
                 << "\n\tcode: " << event.code
                 << "\n\tjson: " << event.json << endl;
        }
    } //end_while
    
    delete [] buffer;
}

void test_for_ji_calc_file()
{
    JI_EVENT event;
    int iRet;
    int count = 0;
    while ((repeats == -1) || (count++ < repeats))
    {
        if (repeats == -1 || repeats > 1)
            cout << "repeat: " << count << endl;

        iRet = ji_calc_file(predictor,strIn.c_str(), EMPTY_EQ_NULL(strArgs), 
                            EMPTY_EQ_NULL(strOut), &event);
        cout << "call ji_calc_file, return " << iRet << endl;
        
        if (iRet == JISDK_RET_SUCCEED)
        {
            cout << "event info:"
                 << "\n\tcode: " << event.code
                 << "\n\tjson: " << event.json << endl;
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
    while ((repeats == -1) || (count++ < repeats))
    {
        if (repeats == -1 || repeats > 1)
            cout << "repeat: " << count << endl;

        iRet = ji_calc_video_file(predictor,strIn.c_str(), EMPTY_EQ_NULL(strArgs), 
                                  EMPTY_EQ_NULL(strOut), EMPTY_EQ_NULL(strJson));
        cout << "call ji_calc_video_file, return " << iRet << endl;

        if (iRet == JISDK_RET_SUCCEED)
        {
            cout << "result info:"
                 << "\n\tout_video_file: " << strOut
                 << "\n\tout_json_file: "  << strJson << endl;
        }
    } //end_while
}

int main(int argc, char *argv[])
{
    //capture exception
    google::InstallFailureSignalHandler();
    google::InstallFailureWriter(&signal_handle);

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
                strIn = optarg;
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
        cerr << "[ERROR] invalid function." << endl;
        show_help();
        return -1;
    }

    if (strLicense.empty())
    {
        strLicense = "license.txt";
    }
    
    if (strIn.empty())
    {
        cerr << "[ERROR] no infile." << endl;
        show_help();
        return -1;
    }

    static const string cs_function[4] = {
                              "1.ji_calc_frame",
                              "2.ji_calc_buffer",
                              "3.ji_calc_file",
                              "4.ji_calc_video_file"
    };

    if (repeats <= 0) repeats = -1;
    
    //print params
    cout << "run params info:" 
         << "\n\tfuction: "   << cs_function[c]
         << "\n\tlicense: "   << strLicense
         << "\n\tinfile: "    << strIn 
         << "\n\targs: "      << strArgs 
         << "\n\toutfile: "   << strOut      
         << "\n\trepeat:"     << repeats   << endl;

    //read license & check license
    string l,ts;
    int v,qps;
    v = qps = 0;
    if (!read_license(strLicense,l,ts,qps,v))
    {
        cerr << "[ERROR] read_license faild." << endl;
        return -1;
    }

    char strQps[64] = {0};
    sprintf(strQps,"%d",qps);
    
    char strVersion[64] = {0};
    sprintf(strVersion,"%d",v);
    
    cout << "license info:"
         << "\n\tlicense: "     << l
         << "\n\ttimestamp: "   << ts
         << "\n\tqps: "         << (qps>0?strQps:"")
         << "\n\tversion:"      << v    << endl;

    int ac = 0;
    char *av[4];
    av[ac++] = const_cast<char*>(l.c_str());
    av[ac++] = ts.empty()?NULL:const_cast<char*>(ts.c_str());    
    av[ac++] = qps>0?strQps:NULL;
    av[ac++] = strVersion;
    int iRet = ji_init(ac,av);
    if (iRet != JISDK_RET_SUCCEED) 
    {
        cerr <<  "[ERROR] ji_init faild, return " << iRet << endl;
        return -1;
    }

    //create predictor
    predictor = ji_create_predictor(JISDK_PREDICTOR_DEFAULT);
    if (predictor == NULL)
    {
        cerr <<  "[ERROR] ji_create_predictor faild, return NULL" << endl;
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
