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
        outfn : output file name （For effeciency, if NULL, no need to draw or output anything）
        json : result json string 
      return :
        0:success
        others : fail
    */ 
    int ji_calc(const unsigned char* buffer, int length, const char* outfn, char** json);
    
}
#endif
