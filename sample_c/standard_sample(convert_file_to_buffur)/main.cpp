#include "ji.h"
#include "ji_util.h"

int main(int argc,char* argv[]){
	/* init model  */
	int ret = ji_init();
	if(ret!=0){
		cout<<"init failed."<<endl;
		return 1;
	}

	/* convert file to buffer for image test */
	vector<unsigned char> buffer;
	ret  = ji_file2buffer(argv[1],&buffer);
	if(ret!=0){
		cout<<"convert file to buffer failed."<<endl;
		return 1;
	}

	/* test core algorithm ji_calc */
	ret = ji_calc(buffer.data(),buffer.size(),NULL,NULL);
	if(ret!=0){
		cout<<"calc failed."<<endl;
		return 1;
	}

	return 0;
}
