#ifndef JI_H
#define JI_H

/*
 * Notes:
 * 1. If use init funciton, no need to release library or model.
 * 2. Please release image inside the functions(ji_calc).
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
      event defination
    */
	typedef struct
	{
		int code; //event code
		char* json; //event
	}JI_EVENT;
	
	typedef struct
	{
		const char* filename;
		const unsigned char* buffer;
		int length;
	}JI_FILE_BUFFER;

    /* 
        init library/model
        return:
          0:success
          other : fail 
    */
    int ji_init(int argc, char** argv);

    /*
    creat instance and initiallized that can be used to analysis
	*/
    void *ji_create_predictor();

	/*
	destory instance
	*/
    void ji_destory_predictor(void* predictor);
   /*
      analysis image
      parameters:
        buffer : input file buffer
        length : input file buffer length
        outfn : output file name
        json : result json string 
      return :
        0:success
        others : fail
    */ 
    int ji_calc(void* predictor, const unsigned char* buffer, int length, const char* args, const char* outfn, char** json);
    
	/*
      analysis image
      parameters:
        predictor : predictor created by ji_create_predictor()
        inBuffer : input file buffer
        args : input args
        outBuffer : output file buffer
        json : result json string 
      return :
        0:success
        others : fail
    */ 
    int ji_calc_buffer(void* predictor, JI_FILE_BUFFER *inBuffer, const char* args, JI_FILE_BUFFER *outBuffer, char** json);

   /*
      analysis image
      parameters:
        infn : input file name
        outfn : output file name
        json : result json string 
      return :
        0:success
        others : fail
    */ 
    int ji_calc_file(void* predictor, const char* infn, const char* args, const char* outfn, char** json);
    
   /*
      analysis video file
      parameters:
        infn : input file name
        outfn : output file name
        cb : call back function point 
      return :
        0:success
        others : fail
    */ 
    int ji_calc_video_file(void* predictor, const char* infn, const char* args, const char* outfn, JI_EVENT* event);

   /*
      analysis video frame
      parameters:
        inframe : input frame
        outframe : output frame
        cb : call back function point 
      return :
        0:success
        others : fail
    */     
    int ji_calc_video_frame(void* predictor, JI_CV_FRAME* inframe, const char* args, JI_CV_FRAME* outframe, JI_EVENT* event);
    
}

/* 
如果是图片分析，请实现以下方法
*/

    /* 
        init library/model
        return:
          0:success
          other : fail 
    */
    int ji_init(int argc, char** argv);

    /*
    creat instance and initiallized that can be used to analysis
	*/
    void *ji_create_predictor();

	/*
	destory instance
	*/
    void ji_destory_predictor(void* predictor);

   /*
      analysis image
      parameters:
        buffer : input file buffer
        length : input file buffer length
        outfn : output file name
        json : result json string 
      return :
        0:success
        others : fail
    */ 
    int ji_calc(void* predictor, const unsigned char* buffer, int length, const char* args, const char* outfn, char** json);

   /*
      analysis image
      parameters:
        infn : input file name
        outfn : output file name
        json : result json string 
      return :
        0:success
        others : fail
    */ 
    int ji_calc_file(void* predictor, const char* infn, const char* args, const char* outfn, char** json);

#endif
