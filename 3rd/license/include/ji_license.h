//
// Created by hrh on 2019/11/7.
//

#ifndef JI_LICENSE_JI_LICENSE_H
#define JI_LICENSE_JI_LICENSE_H

#ifdef __cplusplus
extern "C" {
#endif

#define DLL_DEFAULT __attribute__((visibility("default")))

#include <stdbool.h>

#include "ji_license_impl.h"

__attribute__((constructor)) bool license_init();
__attribute__((destructor)) bool license_finit();

typedef int generate_reference_type(char *info, char *reference, int *version);

typedef int generate_reference_networking_type(char *info, char *reference, int *version);

typedef int generate_license_type(const char *privateKey, const char *reference, const char *activation, const char *timestamp,
                                  const int *qps, int version, char *license);

typedef int check_license_type(const char *pubKey, const char *license, const char *url, const char *activation,
                               const char *timestamp, const int *qps, int version);

typedef int check_expire_type();

typedef int check_expire_only_type();

typedef int get_license_version_type(char **version);

DLL_DEFAULT
int ji_generate_reference(char *info, char *reference, int *version);

DLL_DEFAULT
int ji_generate_reference_networking(char *info, char *reference, int *version);

DLL_DEFAULT
int ji_generate_license(const char *privateKey, const char *reference,
                             const char *activation, const char *timestamp,
                             const int  *qps, int version, char *license);

DLL_DEFAULT
int ji_check_license(const char *pubKey, const char *license, const char *url,
                          const char *activation, const char *timestamp, const int *qps, int version);

DLL_DEFAULT
int ji_check_expire();

DLL_DEFAULT
int ji_check_expire_only();

DLL_DEFAULT
int ji_get_license_version(char **version);

#ifdef __cplusplus
}
#endif

#endif //JI_LICENSE_JI_LICENSE_H
