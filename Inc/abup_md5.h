/*****************************************************************************
* Copyright (c) 2018 ABUP.Co.Ltd. All rights reserved.
* File name: abup_md5.h
* Description:
* Author: Ray Shen
* Version: v1.0
* Date: 20181101
*****************************************************************************/
#ifndef ABUP_MD5_H
#define ABUP_MD5_H
#include "abup_stdlib.h"
#ifndef MD5_BLOCKSIZE
#define MD5_BLOCKSIZE 64
#endif
//#pragma pack(1)
typedef struct {
    abup_uint32 count[2];
    abup_uint32 state[4];
    abup_uint8 buffer[MD5_BLOCKSIZE];
} ABUP_MD5_CTX;
typedef struct {
    abup_uint8 content1;
    abup_uint8 content2;
    abup_uint32 content3;
}ABUP_MD5_DATA;
//#pragma pack()
#define F(x,y,z) (((x) & (y)) | (~(x) & (z)))
#define G(x,y,z) (((x) & (z)) | ((y) & ~(z)))
#define H(x,y,z) ((x)^(y)^(z))
#define I(x,y,z) ((y) ^ ((x) | ~(z)))
#define ROTATE_LEFT(x,n) (((x) << (n)) | ((x) >> (32-(n))))



#define FF(a,b,c,d,x,s,ac) \
				{ \
          (a) += F((b),(c),(d)) + (x) + (ac); \
          (a) = ROTATE_LEFT((a),(s)); \
          (a) += (b); \
          }

#define GG(a,b,c,d,x,s,ac) \
          { \
          (a) += G((b),(c),(d)) + (x) + (ac); \
          (a) = ROTATE_LEFT((a),(s)); \
          (a) += (b); \
          }

#define HH(a,b,c,d,x,s,ac) \
          { \
          (a) += H((b),(c),(d)) + (x) + (ac); \
          (a) = ROTATE_LEFT((a),(s)); \
          (a) += (b); \
          }

#define II(a,b,c,d,x,s,ac) \
          { \
          (a) += I((b),(c),(d)) + (x) + (ac); \
          (a) = ROTATE_LEFT((a),(s)); \
          (a) += (b); \
          }

extern void AbupMD5Init(ABUP_MD5_CTX *context);
extern void AbupMD5Update(ABUP_MD5_CTX *context,abup_uint8 *input,abup_uint32 inputlen);
extern void AbupMD5Final(ABUP_MD5_CTX *context,abup_uint8 digest[16]);
extern void AbupMD5Transform(abup_uint32 state[4],abup_uint8 block[64]);
extern void AbupMD5Encode(abup_uint8 *output,abup_uint32 *input,abup_uint32 len);
extern void AbupMD5Decode(abup_uint32 *output,abup_uint8 *input,abup_uint32 len);
#endif

