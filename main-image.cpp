//#include "ji.hpp"
#include "./include/ji_util.hpp"
#include "ji.h"
#include <string.h>
#include <sstream>
#include <glog/logging.h>
using namespace std;
int main(int argc, char* argv[]) {
	/* init model  */
	//image buffer analysis
	if (argc < 4) {
		std::cout << "Parameter number not right" <<std::endl;
		return -1;
	}
	google::InitGoogleLogging("ev");
	void * predictor = ji_create_predictor();

	char *json = NULL;
	if (std::string(argv[3]) == "1") {
		vector<unsigned char> buffer;
		ji_file2buffer1(argv[1], &buffer);
		while(1){
		ji_calc(predictor, (const unsigned char*) &buffer[0], buffer.size(), "",
				argv[2], &json);
		cout << json << endl;
		delete[] json;
		}
		//image file analysis
	} else if (std::string(argv[3]) == "0") {
		while(1){
		ji_calc_file(predictor, argv[1], "", argv[2], &json);
		cout << json << endl;
		delete[] json;
		}
	} else {
		std::cout << "Error: Choose Interface Wrong" << std::endl;
	}
	ji_destory_predictor(predictor);
	google::ShutdownGoogleLogging();
	return 0;
}
