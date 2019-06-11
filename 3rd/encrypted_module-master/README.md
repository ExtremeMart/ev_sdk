# encrypted module
This project is a deencrypt module for deep learning model encrypt. 
Together with project encrypt_suite,this project composes a complete encrypt-deencrypt suite.
This project provides the following Api to decouple encrypt-dencrypt add various framework.
In current version,only frameworks which take buffer as input can be totally decoupled, 
and frameworks that only take file name as input is still slightly coupled.
so we have to rewrite framework to completes a api that take FILE*  as inputs, which is a easy work,compared to completing dencrypt inner framework:
## The provided apis are as follows:

```
void* CreateEncryptor(const char* str,int str_len,const char* key);
//create an instance：
//params：
//  str->a str corresponding to an encrypted model
//  str_len->the length of the encrypted str
//	Key->the key to deencrypt model,
//output：
//  pointer to the instance


void* FetchBuffer(void* ptr,int &buffer_len);
//get buffer of the deencrypted model：
//params:
	//ptr->pointer to the instance
	//buffer->the length of dencrypted buffer
//output：
    dencrypted buffer

void* FetchFile(void* ptr);
//get FILE * of the deencrypted model：
//params：
//	ptr->pointer to the instance	
//output：
//  FILE* corresponding to dencrypted file//free this in frame  work


void DestroyEncrtptor(void *ptr);
//destroy all things about this module
//  p[arams：
//	ptr->pointer to the instance
```

## for example

 `./3rd/encrypted_module-master/encrypt_model ./3rd/encrypted_module-master/test_model.proto 01234567890123456789012345678988`
 
 out file: ./model_str.hpp, copy this file to "./src" directory and add to ev_sdk project.
 
 
 ---
