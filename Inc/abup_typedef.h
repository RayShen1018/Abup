/*****************************************************************************
* Copyright (c) 2018 ABUP.Co.Ltd. All rights reserved.
* File name: abup_typedef.h
* Description: 
* Author: Ray Shen
* Version: v1.0
* Date: 20181101
*****************************************************************************/
#ifndef _ABUP_TYPEDEF_H_
#define _ABUP_TYPEDEF_H_

#include "abup_config.h"
#include <stddef.h>

#ifndef abup_extern
#ifdef __cplusplus
#define abup_extern extern "C"
#else
#define abup_extern extern
#endif
#endif//abup_extern

#if (ABUP_DEFAULT_SEGMENT_SIZE_INDEX == ABUP_SEGMENT_SIZE_16_INDEX)
#define ABUP_DATA_SEQ_MAX_LEN 16
#elif (ABUP_DEFAULT_SEGMENT_SIZE_INDEX == ABUP_SEGMENT_SIZE_32_INDEX)
#define ABUP_DATA_SEQ_MAX_LEN 32
#elif (ABUP_DEFAULT_SEGMENT_SIZE_INDEX == ABUP_SEGMENT_SIZE_64_INDEX)
#define ABUP_DATA_SEQ_MAX_LEN 64
#elif (ABUP_DEFAULT_SEGMENT_SIZE_INDEX == ABUP_SEGMENT_SIZE_128_INDEX)
#define ABUP_DATA_SEQ_MAX_LEN 128
#elif (ABUP_DEFAULT_SEGMENT_SIZE_INDEX == ABUP_SEGMENT_SIZE_256_INDEX)
#define ABUP_DATA_SEQ_MAX_LEN 256
#elif (ABUP_DEFAULT_SEGMENT_SIZE_INDEX == ABUP_SEGMENT_SIZE_512_INDEX)
#define ABUP_DATA_SEQ_MAX_LEN 512
#endif

#define ABUP_HTTP_MAX_LEN                   (ABUP_DATA_SEQ_MAX_LEN + 650)
#define ABUP_COAP_MAX                   (ABUP_DATA_SEQ_MAX_LEN + 32)
#define ABUP_COAP_MIN                   256
#if (ABUP_COAP_MAX < ABUP_COAP_MIN)
#define ABUP_COAP_MAX_LEN                   ABUP_COAP_MIN
#else
#define ABUP_COAP_MAX_LEN                   ABUP_COAP_MAX
#endif

#if defined(__GNUC__)
#define ABUP_ASM            __asm                                      /*!< asm keyword for GNU Compiler          */
#define ABUP_INLINE         inline                                     /*!< inline keyword for GNU Compiler       */
#define ABUP_STATIC_INLINE  static inline
#define ABUP_ALIGN(x) __attribute__ ((aligned (x)))
#elif defined(__ICCARM__)||defined(__ICCRX__)||defined(__ICCSTM8__)
#define ABUP_ASM            __asm                                      /*!< asm keyword for IAR Compiler          */
#define ABUP_INLINE         inline                                     /*!< inline keyword for IAR Compiler. Only available in High optimization mode! */
#define ABUP_STATIC_INLINE  static inline
#define ABUP_ALIGN(x) __attribute__((aligned(x)))
#elif defined(__CC_ARM)
#define ABUP_ASM            __asm                                      /*!< asm keyword for ARM Compiler          */
#define ABUP_INLINE         __inline                                   /*!< inline keyword for ARM Compiler       */
#define ABUP_STATIC_INLINE  static __inline
#define ABUP_ALIGN(x) __align(x)
#else
  #error "Alignment not supported for this compiler."
#endif


typedef enum {
	ABUP_FILETYPE_APP,
	ABUP_FILETYPE_PATCH,
	ABUP_FILETYPE_BACKUP,
	ABUP_FILETYPE_PATCH_INFO,
	ABUP_FILETYPE_END
}ABUP_PEER_FILETYPE;

#define ABUPSW32(x) ((((abup_uint32)(x)&(abup_uint32)0x000000ff) << 24 )\
										|(((abup_uint32)(x)&(abup_uint32)0x0000ff00 ) << 8 )\
										|(((abup_uint32)(x)&(abup_uint32)0x00ff0000 ) >> 8 )\
										|(((abup_uint32)(x)&(abup_uint32)0xff000000 ) >> 24 ))


#define ABUP_UPDATE_SUCCESS              1
#define ABUP_UPDATE_AUTH_FAIL              98
#define ABUP_UPDATE_FAIL              99

#define ABUP_MID_MAXLEN		32
#define ABUP_DEVICEID_MAX_LEN 32
#define ABUP_DEVICESECRET_MAX_LEN 64
#define ABUP_SN_VER_MAXLEN   4
#if defined(ABUP_SLIM_RES)&&((ABUP_KEY_VERSION == 2)||(ABUP_KEY_VERSION == 3))
#define ABUP_HASH_MAXLEN    (4+1)
#else
#define ABUP_HASH_MAXLEN    32
#endif
#define ABUP_CRC_MAXLEN     4
#define ABUP_VERSION_MAXLEN     4
#define ABUP_SN_DATETIME_MAXLEN    22
#if defined(ABUP_WIFI_SSID_PWD)
#define ABUP_WIFI_SSID_MAXLEN		32
#define ABUP_WIFI_PWD_MAXLEN		32
#endif

typedef char                    abup_char;

typedef unsigned short          abup_wchar;

typedef unsigned char           abup_uint8;

typedef signed char             abup_int8;

typedef unsigned short int      abup_uint16;

typedef signed short int        abup_int16;

typedef unsigned int            abup_uint;

typedef signed int              abup_int;

typedef unsigned int            abup_uint32;

typedef signed int              abup_int32;

typedef unsigned long long   abup_uint64;

typedef signed long long     abup_int64;

typedef unsigned int abup_size_t;

typedef   signed           int abup_intptr_t;

typedef unsigned           int abup_uintptr_t;

typedef enum {
	STATE_INIT = 1,    
	STATE_CV,//检测版本
	STATE_DL,//下载
	STATE_RD,//上报下载结果
	STATE_UG,//
	STATE_RU,//上报升级结果
	STATE_KY, //KEY
	STATE_RG, //注册
	STATE_END
}FotaState;

typedef abup_uint8   abup_bool;
#define abup_true  ((abup_bool)1)
#define abup_false ((abup_bool)0)


typedef struct
{
    abup_uint8 algorithm_ver[ABUP_SN_VER_MAXLEN];
    abup_uint32 timestamp;
    abup_uint32 crc;
}ABUP_ALIGN(4) abup_activation_hdr_t;

typedef struct
{
    abup_activation_hdr_t hdr;
    abup_uint8 activation_code[ABUP_HASH_MAXLEN];
}ABUP_ALIGN(4) abup_activation_code_t;
//#pragma pack(1)

typedef struct  
{  
	abup_uint32 index;
	abup_uint32 index_max;
	abup_uint32 delta_id;
	abup_uint16 update_result;
	abup_uint8 deviceId[ABUP_DEVICEID_MAX_LEN + 1];
	abup_uint8 deviceSecret[ABUP_DEVICESECRET_MAX_LEN + 1];
	abup_uint8 inited:4;
	abup_uint8 serverNO:4;
	abup_uint8 mid[ABUP_MID_MAXLEN + 1];
#if (ABUP_KEY_VERSION != 0xFF)
	abup_activation_code_t activation_code;
	abup_activation_code_t activation_code_bk;
#endif
	abup_uint default_module;
#if defined(ABUP_WIFI_SSID_PWD)
	abup_char wifi_ssid[ABUP_WIFI_SSID_MAXLEN + 1];
	abup_char wifi_pwd[ABUP_WIFI_PWD_MAXLEN + 1];
#endif
}ABUP_ALIGN(1) abup_update_struct; 
//#pragma pack()

abup_char* abup_get_manufacturer(void);
abup_char* abup_get_model_number(void);
abup_char* abup_get_product_id(void);
abup_char* abup_get_product_sec(void);
abup_char* abup_get_device_type(void);
abup_char* abup_get_platform(void);
abup_char* abup_get_sdk_version(void);
abup_char* abup_get_apk_version(void);
abup_char* abup_get_firmware_version(void);
abup_char* abup_get_hw_version(void);
abup_char* abup_get_sw_version(void);
abup_char* abup_get_network_type(void);
abup_uint abup_get_try_timer(void);
abup_int8 abup_get_try_count(void);
abup_int32 abup_get_utc_time(void);
abup_uint abup_get_default_module(void);
abup_uint32 abup_get_default_baudrate(void);
abup_uint8 abup_get_default_protocol(void);
abup_uint8 abup_get_default_segment_size(void);
abup_uint abup_get_data_max_len(void);
abup_char* abup_get_buf(void);
void abup_reset_buf(void);
abup_uint abup_get_buf_len(void);
extern abup_update_struct* abup_hal_get_update(void);
extern abup_bool abup_hal_set_update(abup_update_struct *abup_update);
#endif

