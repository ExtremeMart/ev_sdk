#ifndef JI_UTIL_HPP
#define JI_UTIL_HPP

#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>
#include <sstream>

using namespace std;
extern "C"{

        /*
         * convert image file to buffer.
         * parameters:
         *      in_filename : input file name
         *      out_buffer  : output file buffer
         * return:
         *      0:success
         *      other:fail
         */
        int ji_file2buffer(const char* in_filname, vector<unsigned char>* out_buffer);
        
        int ji_file2buffer(const char* in_filename, vector<unsigned char>* out_buffer){
                ifstream in(in_filename, ios::in|ios::binary);
                if(!in){ 
                        cout<<"file cannot read."<<endl;
                        return 1;
                }
        //      stringstream sstr;
        //      while(in>>sstr.rdbuf());
                istreambuf_iterator<char> start(in), end;
                vector<unsigned char> buffer(start, end);
                in.close();
                
   //           vector<unsigned char> buffer(sstr.str().begin(), sstr.str().end());
                *out_buffer = buffer;
                return 0;
        }       
        
}

#endif // JI_UTIL_H
