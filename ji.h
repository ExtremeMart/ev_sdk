#ifndef JI_H
#define JI_H

/*
 * Notes:
 * 1. If use init funciton, no need to release library or model.
 * 2. Please release image inside the functions(ji_calc_*).
 *
 */

extern "C" {
	
	/*
	  refer to cv::Mat definition
	*/
	typedef struct
	{
		int 	rows; //Number of rows in a 2D array.
		int 	cols; //Number of columns in a 2D array.
		int 	type;  //Array type. Use CV_8UC1, ..., CV_64FC4 to create 1-4 channel matrices, or CV_8UC(n), ..., CV_64FC(n) to create multi-channel (up to CV_CN_MAX channels) matrices.
		void * 	data; //Pointer to the user data.
		int 	step; //Number of bytes each matrix row occupies.
	}JI_CV_FRAME;
	
   /*
      define call back function 
      parameters:
        code : event code
        json : event (JSON format)
      return :
        0:success
        others : fail
    */ 	
	typedef int (*JI_CB)(int code, const char* json);

    /* 
        init library/model
        return:
          0:success
          other : fail 
    */
    int ji_init();

   /*
      analysis image
      parameters:
        buffer : input file buffer
        length : input file buffer length
        args  : if any, input parameters (JSON format)
        outfn : output file name
        json : result json string (JSON format)
      return :
        0:success
        others : fail
    */ 
    int ji_calc(const unsigned char* buffer, int length, const char* args, const char* outfn, char** json);

   /*
      analysis image
      parameters:
        infn : input file name
        outfn : output file name
        args  : if any, input parameters (JSON format)
        json : result json string (JSON format)
      return :
        0:success
        others : fail
    */ 
    int ji_calc_file(const char* infn, const char* args, const char* outfn, char** json);
    
   /*
      analysis video file
      parameters:
        infn : input file name
        args  : if any, input parameters(JSON format)
        outfn : output file name
        cb : call back function point 
      return :
        0:success
        others : fail
    */ 
    int ji_calc_video_file(const char* infn, const char* args, const char* outfn, JI_CB cb);

   /*
      analysis video frame
      parameters:
        inframe : input frame
        args  : if any,input parameters (JSON format)
        outframe : output frame
        cb : call back function point 
      return :
        0:success
        others : fail
    */     
    int ji_calc_video_frame(JI_CV_FRAME* inframe, const char* args, JI_CV_FRAME** outframe, JI_CB cb);
    
}
#endif
