/*****************************************************************************
* Copyright (c) 2018 ABUP.Co.Ltd. All rights reserved.
* File name: abup_hmd5.h
* Description: 
* Author: Ray Shen
* Version: v1.0
* Date: 20181101
*****************************************************************************/
#ifndef __ABUP_HMD5_H__
#define __ABUP_HMD5_H__

#define ABUP_HMD5_LEN 34

#include "abup_typedef.h"
#include "abup_md5.h"

#ifndef MD5_DIGESTSIZE
#define MD5_DIGESTSIZE 16
#endif

#ifndef MD5_BLOCKSIZE
#define MD5_BLOCKSIZE 64
#endif

//#pragma pack(1)
typedef struct{
	ABUP_MD5_CTX ictx;
	ABUP_MD5_CTX octx;
	abup_char imd[MD5_DIGESTSIZE];
	abup_char omd[MD5_DIGESTSIZE];
	abup_char buf[MD5_BLOCKSIZE];
	}ABUP_HMD5_CTX;
//#pragma pack()

extern abup_char * abup_hmd5_pid_psec_mid(abup_char *deviceId, abup_char *deviceSecret,abup_char *mid);
extern abup_char * abup_hmd5_pid_psec_mid(abup_char *deviceId, abup_char *deviceSecret,abup_char *mid);
#endif

