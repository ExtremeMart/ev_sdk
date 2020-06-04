#ifndef __ev_license_h__
#define __ev_license_h__

#ifdef __cplusplus
extern "C" {
#endif

#define DLL_DEFAULT __attribute__((visibility("default")))

/* 
** 版本说明
非联网与联网合二为一
当版本:v10
非联网最新版本:v7
联网最新版本:v8
** 
*/
#define EV_LICENSE_VERSION  "version:v20.1.3"

#define EV_MESSAGE_MAXLEN   (4096)
#define EV_ID_MAXLEN        (1024)

/* 函数返回值 */
#define EV_SUCCESS          (0)
#define EV_FAIL             (-1)
#define EV_EXPIRE           (-2)
#define EV_OVERMAXQPS       (-3)
#define EV_INVALID          (-4)        //联网校验，通信协议无效
#define EV_OFFLINE          (-5)        //联网校验，服务器离线状态

/*
生成rsa钥匙:
    openssl genrsa -out privateKey.pem 1024
    openssl rsa -in privateKey.pem -pubout -out pubKey.pem
*/

/*
函 数 名	: ji_generate_reference_impl
功能描述	: 非联网，在既定运行环境生成参考信息,包括参考码和版本
函数参数	:
    info:       [可选参数] 输入输出参数,参考信息缓冲,json格式,
                包括参考码和版本(建议分配(EV_MESSAGE_MAXLEN)
    reference:  [可选参数] 输入输出参数,参考码缓冲(建议分配EV_ID_MAXLEN)
    version:    [可选参数] 输入输出参数,参考码版本
返回参数	: 成功返回EV_SUCCESS,其它表示失败
注     意: info一组,reference与version一组，两组至少传一组
*****************************************************************************/
DLL_DEFAULT
int ji_generate_reference_impl(char *info, char *reference, int *version);


/*
函 数 名 : ji_generate_reference_networking_impl
功能描述    : 联网,在既定运行环境生成参考信息,包括参考码和版本
函数参数    :
    info:       [可选参数] 输入输出参数,参考信息缓冲,json格式,
                包括参考码和版本(建议分配(EV_MESSAGE_MAXLEN)
    reference:  [可选参数] 输入输出参数,参考码缓冲(建议分配EV_ID_MAXLEN)
    version:    [可选参数] 输入输出参数,参考码版本
返回参数    : 成功返回EV_SUCCESS,其它表示失败
注     意: info一组,reference与version一组，两组至少传一组
*****************************************************************************/
DLL_DEFAULT
int ji_generate_reference_networking_impl(char *info, char *reference, int *version);


/*
函 数 名	: ji_generate_license_impl
功能描述	: 根据摘要生成授权码，同时还可以指定有效期
函数参数	:
    privateKey: [必选参数] 输入参数,rsa私钥
    reference:  [必选参数] 输入参数,参考码
    activation: [可选参数] 输入参数,指定激活码
    timestamp:  [可选参数] 输入参数,指定有效期
    qps:        [可选参数] 输入参数,指定请求量上限
    version:    [必选参数] 输入参数,参考码版本
    license:    [必选参数] 输入输出参数,授权码缓冲(建议分配EV_ID_MAXLEN)
返回参数	: 成功返回EV_SUCCESS,其它表示失败
*****************************************************************************/
DLL_DEFAULT
int ji_generate_license_impl(const char *privateKey, const char *reference,
                             const char *activation, const char *timestamp,
                             const int  *qps, int version, char *license);


/*
函 数 名 : ji_check_license_impl
功能描述    : 校验授权码
函数参数    :
    pubKey:     [必选参数] 输入参数,rsa公钥
    license:    [必选参数] 输入参数,授权码
    url:        [可选参数] 输入参数,校验授权码http地址
    activation: [可选参数] 输入参数,指定激活码
    timestamp:  [可选参数] 输入参数,指定有效期
    qps:        [可选参数] 输入参数,指定请求量上限
    version:    [必选参数] 输入参数,参考码版本
返回参数    : 成功返回EV_SUCCESS,其它表示失败
备注      : 只有v8(含)后才会真正生效url及activation参数.
*****************************************************************************/
DLL_DEFAULT
int ji_check_license_impl(const char *pubKey, const char *license, const char *url,
                          const char *activation, const char *timestamp, const int *qps, int version);


/*
函 数 名	: ji_check_expire_impl
功能描述	: 检验授权,有效期及请求量上限
函数参数	: 无参数
返回参数	: 已授权且在有效期内且未超过请求量上限时返回EV_SUCCESS,其它表示失败或过期或超过请求量上限
备注      ：在授权码校验成功才真正检验
          1.没有授权码时,返回EV_FAIL;
          2.有timestamp时,超过有效期,返回EV_EXPIRE;
          3.有qps时,超过请求量上限,返回EV_OVERMAXQPS;
          4.其它返回EV_SUCCESS.
          5.v8(含)后增加url校验
*****************************************************************************/
DLL_DEFAULT
int ji_check_expire_impl();


/*
函 数 名	: ji_check_expire_only_impl
功能描述	: 仅检验授权及有效期,不对qps进行处理
函数参数	: 无参数
返回参数	: 已授权且在有效期内返回EV_SUCCESS,否则返回EV_FAIL
备注      ：与ji_check_expire区别：当qps启用时,调用ji_check_expire函数qps计数会加1
*****************************************************************************/
DLL_DEFAULT
int ji_check_expire_only_impl();

/**
 * 获取license版本信息，
 * @param version[out], 返回的版本信息, 需要外部释放内存空间
 * @return 成功返回EV_SUCCESS, 其他值表示失败
 */
DLL_DEFAULT
int ji_get_license_version_impl(char **version);

#ifdef __cplusplus
}
#endif

#endif