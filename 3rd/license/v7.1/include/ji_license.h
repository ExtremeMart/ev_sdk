#ifndef __ev_license_h__
#define __ev_license_h__

#ifdef __cplusplus
extern "C" {
#endif

#define DLL_DEFAULT __attribute__((visibility("default")))

#define EV_MESSAGE_MAXLEN (4096)
#define EV_ID_MAXLEN      (1024)

#define EV_SUCCESS        (0)
#define EV_FAIL           (-1)
#define EV_EXPIRE         (-2)
#define EV_OVERMAXQPS     (-3)

/*
生成rsa钥匙:
    openssl genrsa -out privateKey.pem 1024
    openssl rsa -in privateKey.pem -pubout -out pubKey.pem
*/

/*
函 数 名	: ji_generate_reference
功能描述	: 在既定运行环境生成参考信息,包括参考码和版本
函数参数	: 
    info:       [可选参数] 输入输出参数,参考信息缓冲,json格式,
                包括参考码和版本(建议分配(EV_MESSAGE_MAXLEN)
    reference:  [可选参数] 输入输出参数,参考码缓冲(建议分配EV_ID_MAXLEN)
    version:    [可选参数] 输入输出参数,参考码版本
返回参数	: 成功返回EV_SUCCESS,其它表示失败  
注     意: info一组,reference与version一组，两组至少传一组 
*****************************************************************************/
DLL_DEFAULT
int ji_generate_reference(char *info, char *reference, int *version);

/*
函 数 名	: ji_generate_license
功能描述	: 根据摘要生成授权码，同时还可以指定有效期
函数参数	: 
    privateKey: [必选参数] 输入参数,rsa私钥
    reference:  [必选参数] 输入参数,参考码
    timestamp:  [可选参数] 输入参数,指定有效期
    qps:        [可选参数] 输入参数,指定请求量上限
    license:    [必选参数] 输入输出参数,授权码缓冲(建议分配EV_ID_MAXLEN)
返回参数	: 成功返回EV_SUCCESS,其它表示失败 
*****************************************************************************/
DLL_DEFAULT
int ji_generate_license(const char *privateKey, const char *reference, 
                        const char *timestamp, const int *qps, char *license);


/*
函 数 名	: ji_check_license
功能描述	: 校验授权码
函数参数	: 
    pubKey:     [必选参数] 输入参数,rsa公钥
    license:    [必选参数] 输入参数,授权码
    timestamp:  [可选参数] 输入参数,指定有效期
    qps:        [可选参数] 输入参数,指定请求量上限
    version:    [必选参数] 输入参数,参考码版本
返回参数	: 成功返回EV_SUCCESS,其它表示失败 
*****************************************************************************/
DLL_DEFAULT
int ji_check_license(const char *pubKey, const char *license, 
                     const char *timestamp, const int *qps, int version);


/*
函 数 名	: ji_check_expire
功能描述	: 检验授权,有效期及请求量上限
函数参数	: 无参数
返回参数	: 已授权且在有效期内且未超过请求量上限时返回EV_SUCCESS,其它表示失败或过期或超过请求量上限
备注      ：在授权码校验成功才真正检验
          1.没有授权码时,返回EV_FAIL;
          2.有timestamp时,超过有效期,返回EV_EXPIRE;
          3.有qps时,超过请求量上限,返回EV_OVERMAXQPS;
          4.其它返回EV_SUCCESS.
*****************************************************************************/
DLL_DEFAULT
int ji_check_expire();

#ifdef __cplusplus
}
#endif

#endif
