/*****************************************************************************
* Copyright (c) 2018 ABUP.Co.Ltd. All rights reserved.
* File name: abup_hal.h
* Description: abup_hal.c头文件
* Author: Ray Shen
* Version: v1.0
* Date: 20181101
*****************************************************************************/
#ifndef __ABUP_HAL_H__
#define __ABUP_HAL_H__
#include "abup_stdlib.h"
#if !defined(ABUP_BOOTLOADER)
#include "abup_http.h"
#include "abup_coap.h"
#include "abup_md5.h"
#endif

#define ABUP_MAX_DELAY      (abup_uint32)0xFFFFFFFFU

#define ABUP_TOKEN_MAXLEN		16

#if (ABUP_DEFAULT_NETWORK_PROTOCOL == ABUP_PROTOCOL_HTTP)
#define ABUP_HAL_PROTOCOL_MAX_LEN (300 + ABUP_DATA_SEQ_MAX_LEN)
#elif (ABUP_DEFAULT_NETWORK_PROTOCOL == ABUP_PROTOCOL_COAP_HTTP)
#if (ABUP_COAP_MAX_LEN > (300 + ABUP_DATA_SEQ_MAX_LEN))
#define ABUP_HAL_PROTOCOL_MAX_LEN (ABUP_COAP_MAX_LEN)
#else
#define ABUP_HAL_PROTOCOL_MAX_LEN (300 + ABUP_DATA_SEQ_MAX_LEN)
#endif
#else
#define ABUP_HAL_PROTOCOL_MAX_LEN (ABUP_COAP_MAX_LEN)
#endif
#if (ABUP_HAL_PROTOCOL_MAX_LEN < 256)
#define ABUP_HAL_DATA_MAX_LEN 256
#else
#define ABUP_HAL_DATA_MAX_LEN ABUP_HAL_PROTOCOL_MAX_LEN
#endif
#define ABUP_DOWNLOAD_MD5_LEN		16
#if defined(ABUP_SLIM_RES)
#define ABUP_DOWNLOAD_HOST_MAX_LEN		32
#define ABUP_DOWNLOAD_URI_MAX_LEN		80
#else
#define ABUP_DOWNLOAD_HOST_MAX_LEN		64
#define ABUP_DOWNLOAD_URI_MAX_LEN		128
#endif
#define ABUP_AT_IP_MAXLEN		4

#if (ABUP_DOWNLOAD_HOST_MAX_LEN < 32)
#define ABUP_BUF_MAX_LEN		32
#else
#define ABUP_BUF_MAX_LEN		ABUP_DOWNLOAD_HOST_MAX_LEN
#endif
//#pragma pack(1)
typedef struct
{
    abup_char download_host[ABUP_DOWNLOAD_HOST_MAX_LEN + 1];
#if !defined(ABUP_SLIM_RES)
    abup_char bkup_host[ABUP_DOWNLOAD_HOST_MAX_LEN + 1];
#endif
    abup_uint8 download_host_ip[ABUP_AT_IP_MAXLEN + 1];
    abup_char download_url[ABUP_DOWNLOAD_URI_MAX_LEN + 1];
    abup_char download_md5[ABUP_DOWNLOAD_MD5_LEN + 1];
    abup_uint32 download_delta_id;
    abup_uint32 download_delta_size;
    abup_uint32 download_port;
} download_uri_struct;
typedef enum
{
    ABUP_RESULT_FAIL = 0,
    ABUP_RESULT_CV_SUCC,
    ABUP_RESULT_CV_FAIL,
    ABUP_RESULT_RPT_SUCC,
    ABUP_RESULT_RPT_FAIL,
    ABUP_RESULT_PROGRESS,
    ABUP_RESULT_END,
} ABUP_RESULT;
typedef enum
{
    ABUP_HAL_FLAG_START,
    ABUP_HAL_FLAG_1 = 1,
    ABUP_HAL_FLAG_2 = 2,
    ABUP_HAL_FLAG_4 = 4,
    ABUP_HAL_FLAG_8 = 8,
    ABUP_HAL_FLAG_ALL = ABUP_HAL_FLAG_1|ABUP_HAL_FLAG_2|ABUP_HAL_FLAG_4|ABUP_HAL_FLAG_8,
    // do not add message id at here
} ABUP_HAL_FLAG;
typedef void (*abup_msg_cb)(void* ptr);
typedef void (*abup_state_result)(abup_uint state);
extern abup_uint32 abup_action_result;

extern abup_msg_cb abup_app_msg_cb;
extern abup_msg_cb abup_atp_msg_cb;

extern void Abup_Delay(abup_uint32 Delay);
#if defined(USE_FULL_LL_DRIVER)||defined(USE_HAL_DRIVER)
extern void AbupGetChipUniqueID(abup_uint32 *UID);
#endif
extern void Abup_IncTick(void);
#ifndef ABUP_BOOTLOADER
extern void Abup_AT_Uart_Recv(abup_uint8 ch);
#ifdef ABUP_USART_DEBUG_AT
extern void Abup_Debug_Uart_Recv(abup_uint8 ch);
#endif
#endif
extern abup_uint32 Abup_GetTick(void);
extern void abup_hal_parse_dns(abup_char *host,abup_uint8* ip);

extern abup_bool ABUPCheckKey(void);
extern abup_bool ABUPSaveKey(abup_uint8* pbuf, abup_int len);
extern download_uri_struct* abup_get_download_atp_uri(void);
extern void abup_hal_update_device(abup_uint8* mid,abup_int16 mid_len,abup_uint8* deviceId,abup_int16 deviceIdLen,abup_uint8* deviceSecret,abup_int16 deviceSecretLen);
extern abup_int abup_hal_flash_read(abup_uint8 type,abup_uint32 addr, abup_uint8 *buf, abup_uint size);
extern abup_int abup_hal_flash_write(abup_uint8 type,abup_uint32 addr, abup_uint8 *buf, abup_uint size);
extern abup_int abup_hal_flash_erase(abup_uint8 type,abup_uint32 addr, abup_uint size);
abup_uint32 abup_hal_get_blocksize(void);
abup_uint32 abup_hal_get_info_addr(void);
abup_uint32 abup_hal_get_backup_addr(void);
abup_uint32 abup_hal_get_app_addr(void);
abup_uint32 abup_hal_get_delta_addr(void);
abup_uint32 abup_hal_get_delta_size(void);
extern abup_uint8 *abup_get_hal_data(void);
extern void abup_reset_hal_data(void);
extern abup_uint abup_get_hal_data_len(void);
extern abup_bool abup_hal_write_delta(abup_uint16 index,abup_uint8* data,abup_uint16 len);

extern void Abup_UartRecved(void);
extern void abup_hal_app_msg(void* ptr);
extern void abup_hal_start(abup_msg_cb app_msg_cb,abup_msg_cb atp_msg_cb,abup_state_result state_result);
extern abup_bool abup_hal_started(void);
extern void abup_hal_stop(void);
void abup_display_update(abup_bool enable);
extern abup_uint32 abup_get_download_index(void);
extern abup_uint32 abup_get_download_index_max(void);
extern abup_uint abup_md5_calc_result;
extern abup_uint32 download_index;
extern abup_uint32 download_index_max;
extern abup_uint8 abup_current_token[ABUP_TOKEN_MAXLEN];
extern abup_uint8 abup_current_token_len;
extern abup_int8 abup_get_conn_try_count(void);
extern void abup_set_conn_try_count(abup_int8);
extern abup_uint8* abup_get_str_ip(abup_uint8* ip);
#if defined(ABUP_BOOTLOADER)
#else
#if defined(ABUP_WIFI_SSID_PWD)
abup_bool abup_hal_check_ssid_pwd(abup_char* ssid,abup_uint16 ssid_len,abup_char* pwd,abup_uint16 pwd_len);
#endif
abup_bool abup_hal_get_state(void);
abup_bool abup_hal_legal_moudule(abup_uint moudule);
abup_char* abup_get_server_host(void);
abup_bool abup_hal_para_url(abup_char *url,abup_uint16 url_len);
abup_uint32* abup_get_host_port(void);
abup_bool abup_hal_para_http(abup_uint8 *data,abup_http_parameter* tmp,abup_uint8 tmplen);
abup_uint abup_MD5Calc_result(void);
abup_int abup_MD5Calc(abup_uint buflen, abup_char *md5out);
abup_int8 abup_hal_get_current_state(void);
#endif
abup_int abup_hal_get_key_version(void);
abup_bool abup_hal_check_key_version(void);
abup_bool abup_hal_server_support_key(void);
abup_uint8 abup_hal_get_serverNO(void);
abup_uint8* abup_hal_get_mid(void);
abup_uint32 abup_hal_get_delta_id(void);
abup_uint8* abup_hal_get_deviceId(void);
abup_uint8* abup_hal_get_deviceSecret(void);
abup_uint16 abup_hal_get_update_result(void);
void abup_hal_set_update_result(abup_uint16 result);
abup_activation_code_t* abup_hal_get_activation_code(void);
abup_activation_code_t* abup_hal_get_activation_code_bk(void);
void abup_hal_update_activation_code(void);
void abup_hal_reset_activation_code(void);
abup_uint8* abup_hal_get_download_host_ip(void);
abup_uint32 abup_hal_get_download_delta_id(void);
abup_uint32 abup_hal_get_download_delta_size(void);
abup_char* abup_hal_get_download_url(void);
abup_char* abup_hal_get_download_host(void);
#endif
