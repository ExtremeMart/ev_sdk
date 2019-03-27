#ifndef ENCRYPT_WRAPPER_H_
#define ENCRYPT_WRAPPER_H_

void* CreateEncryptor(const char*,int len,const char*);

void* FetchBuffer(void*,int &);
void* FetchFile(void* );

void DestroyEncrtptor(void *);

#endif