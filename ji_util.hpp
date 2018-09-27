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
	 * 	in_filename : input file name
	 * 	out_buffer  : output file buffer
	 * return:
	 * 	0:success
	 * 	other:fail
	 */

	int ji_file2buffer(std::string inFileName, vector<unsigned char>* outBuffer){
		ifstream fin(inFileName.c_str(), ios::binary);
		if(!fin){
			cout<<"file cannot read."<<endl;
			return -1;
		}

		stringstream sstr;
		sstr << fin.rdbuf();

		fin.close();
		vector<unsigned char> buffer;
		for(int i=0; i < sstr.str().size(); i++){
			buffer.push_back(sstr.str()[i]);
		}

		*outBuffer = buffer;
		return 0;
	}

}

#endif // JI_UTIL_H
