/*****************************************************************************
* Copyright (c) 2018 ABUP.Co.Ltd. All rights reserved.
* File name: abup_config.h
* Description:
* Author: Ray Shen
* Version: v1.0
* Date: 20181101
*****************************************************************************/
#ifndef _ABUP_CONFIG_H_
#define _ABUP_CONFIG_H_
#include "abup_os.h"

#define ABUP_SLIM_RES

//0是lusun算法，1是wosun算法
#ifndef ABUP_FOTA_ALGORITHM
#define ABUP_FOTA_ALGORITHM 0
#endif

//lusun算法还原分配的RAM，注：不大于一个SECTOR
#ifndef ABUP_RESTORE_RAM
#define ABUP_RESTORE_RAM                   0x00000020
#endif
//wosun算法还原分配的RAM，可用RAM全部用于还原
#ifndef ABUP_WORKING_BUFFER_LEN
#define ABUP_WORKING_BUFFER_LEN                   0x1E000
#endif

//0低配莴笋算法，可用RAM小于512K开启 1高配莴笋算法，可用RAM大于512K开启
#define ABUP_WOSUN_STREAM_TYPE 0
//驱动宏，用户可以自行配置
#define STM32L4_NUCLEO
//开启debug，否则user 
#if (ABUP_BOOTLOADER_DEBUG == 1)||(ABUP_APP_DEBUG == 1)
#define ABUP_DEBUG_MODE
#endif
#define ABUP_MODULE_BC28_ENABLE
//#define ABUP_MODULE_M5310_ENABLE
#define ABUP_MODULE_ESP8266_ENABLE
//#define ABUP_MODULE_ESP07S_ENABLE
//#define ABUP_MODULE_SIM7020C_ENABLE
//#define ABUP_MODULE_SIM7600_ENABLE
//#define ABUP_MODULE_ME3630_ENABLE
//#define ABUP_MODULE_SLM152_ENABLE
//每次下载差分包大小、RAM小可以适当减小
#ifndef PKG_USING_ABUP_FOTA
#define ABUP_DEFAULT_SEGMENT_SIZE_INDEX		ABUP_SEGMENT_SIZE_128_INDEX
//device
//SECTOR/BLOCK 总数
#define ABUP_SECTOR_MAX_NUM 0x00000100
//SECTOR/BLOCK 大小
#define ABUP_DEFAULT_SECTOR_SIZE 0x00000800
//Flash基础地址
#define ABUP_FLASH_BASE_ADDR 0x08000000
//bootloader大小
#define ABUP_BL_SIZE                   0x00010000
//app大小
#define ABUP_APP_SIZE                   0x40000
//差分包信息存放地址
#define ABUP_UPDATE_INFO_ADDR                   0x08050000
//差分包存放地址
#define ABUP_UPDATE_DELTA_ADDR                   0x08050800 
//差分包大小，lusun算法包含差分包大小+断点续升所需参数保存空间
#define ABUP_UPDATE_DELTA_SIZE                   0x2F800


//OEM
#define ADUPS_FOTA_SERVICE_OEM			"L452RE"
//设备型号
#define ADUPS_FOTA_SERVICE_MODEL "L452RE"
//Product ID
#define ADUPS_FOTA_SERVICE_PRODUCT_ID "1551332713"
//Product Secret
#define ADUPS_FOTA_SERVICE_PRODUCT_SEC "43d1d10afb934ec997f8a3ac2c2dde77"
//设备类型
#define ADUPS_FOTA_SERVICE_DEVICE_TYPE "box"
//平台
#define ADUPS_FOTA_SERVICE_PLATFORM "stm32l4"
//版本号
#define ABUP_FIRMWARE_VERSION  		"1.0"
#endif
//网络类型
#define ABUP_NETWORK_TYPE			"NB"
#if defined(PKG_USING_ABUP_FOTA)
#define ADUPS_FOTA_RTT_VERSION	"RTT_V1.0"
#define ABUP_KEY_VERSION			3
#else
#define ABUP_KEY_VERSION			2
#endif

//"IOT5.0_LUSUN11_R50426"是lusun算法
//否则是wosun算法,wosun算法默认"IOT4.0_R42641"
#define ADUPS_FOTA_LUSUN_VERSION "IOT5.0_LUSUN11_R50426"
#define ADUPS_FOTA_WOSUN_VERSION "IOT4.0_R42641"
#if (ABUP_FOTA_ALGORITHM == 0)
#define ADUPS_FOTA_SDK_VER ADUPS_FOTA_LUSUN_VERSION
#else
#define ADUPS_FOTA_SDK_VER ADUPS_FOTA_WOSUN_VERSION
#endif
#define ADUPS_FOTA_APP_VERSION	"ADUPS_V4.0"


#define ABUP_HW_VERSION      		"HW01"
#define ABUP_SW_VERSION    	 		"SW01"


#define ABUP_SEGMENT_SIZE_16_INDEX		0
#define ABUP_SEGMENT_SIZE_32_INDEX		1
#define ABUP_SEGMENT_SIZE_64_INDEX		2
#define ABUP_SEGMENT_SIZE_128_INDEX		3
#define ABUP_SEGMENT_SIZE_256_INDEX		4
#define ABUP_SEGMENT_SIZE_512_INDEX		5

typedef enum
{
//移远系列
    ABUP_MODULE_BC28 = 0,
//中移系列
    ABUP_MODULE_M5310 = 20,
//乐鑫系列
    ABUP_MODULE_ESP8266 = 40,
    ABUP_MODULE_ESP07S = 41,
//芯讯通系列
    ABUP_MODULE_SIM7020C = 60,
    ABUP_MODULE_SIM7600 = 61,
//高新兴系列
    ABUP_MODULE_ME3630 = 80,
//美格
    ABUP_MODULE_SLM152 = 100,
	
    ABUP_MODULE_MAX = 0xFFFFF
} ABUP_MODULE;
//ABUP_DEFAULT_NETWORK_PROTOCOL为1是CoAP协议(UDP),为2是HTTP协议(TCP),3同时支持
#define ABUP_PROTOCOL_NONE 0
#define ABUP_PROTOCOL_COAP 1
#define ABUP_PROTOCOL_HTTP 2
#define ABUP_PROTOCOL_COAP_HTTP (ABUP_PROTOCOL_COAP|ABUP_PROTOCOL_HTTP)
#if defined(PKG_USING_ABUP_FOTA)
#if !defined(ABUP_DEFAULT_NETWORK_PROTOCOL)
#define ABUP_DEFAULT_NETWORK_PROTOCOL ABUP_PROTOCOL_HTTP
#endif
#else
#if defined(ABUP_MODULE_ESP8266_ENABLE)||defined(ABUP_MODULE_ESP07S_ENABLE)||defined(ABUP_MODULE_ME3630_ENABLE)||defined(ABUP_MODULE_SIM7600_ENABLE)
#define ABUP_DEFAULT_HTTP_PROTOCOL ABUP_PROTOCOL_HTTP
#else
#define ABUP_DEFAULT_HTTP_PROTOCOL ABUP_PROTOCOL_NONE
#endif
#if defined(ABUP_MODULE_BC28_ENABLE)||defined(ABUP_MODULE_M5310_ENABLE)||defined(ABUP_MODULE_SIM7020C_ENABLE)||defined(ABUP_MODULE_SLM152_ENABLE)
#define ABUP_DEFAULT_COAP_PROTOCOL ABUP_PROTOCOL_COAP
#else
#define ABUP_DEFAULT_COAP_PROTOCOL ABUP_PROTOCOL_NONE
#endif
#define ABUP_DEFAULT_NETWORK_PROTOCOL (ABUP_DEFAULT_HTTP_PROTOCOL + ABUP_DEFAULT_COAP_PROTOCOL)
#endif
//重试次数
#define ABUP_TRY_COUNT 6
#if defined(ABUP_MODULE_ESP8266_ENABLE)||defined(ABUP_MODULE_ESP07S_ENABLE)
#define ABUP_WIFI_SSID_PWD
#endif
#endif

