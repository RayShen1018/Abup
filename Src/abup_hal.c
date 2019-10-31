/*****************************************************************************
* Copyright (c) 2018 ABUP.Co.Ltd. All rights reserved.
* File name: abup_hal.c
* Description: 逻辑、AT处理
* Author: Ray Shen
* Version: v1.0
* Date: 20181101
*****************************************************************************/
#include "abup_hal.h"
#if defined(PKG_USING_ABUP_FOTA)
#include "fal.h"
#else
#if !defined(ABUP_BOOTLOADER)
#include "abup_custom.h"
#endif
#include "abup_hal_flash.h"
#include "abup_hal_uart.h"
#endif
abup_char abup_hal_buf[ABUP_BUF_MAX_LEN];
#if !defined(ABUP_BOOTLOADER)
abup_uint32 abup_action_result = ABUP_RESULT_FAIL;
abup_msg_cb abup_app_msg_cb = NULL;
abup_msg_cb abup_atp_msg_cb = NULL;
abup_state_result abup_state_result_cb = NULL;
download_uri_struct download_atp_uri_data = {0};
download_uri_struct *download_atp_uri = &download_atp_uri_data;
ABUP_ALIGN(1) abup_uint8 abup_hal_data[ABUP_HAL_DATA_MAX_LEN] = {0};
abup_uint32 download_index = 0;
abup_uint32 download_index_max = 0;
abup_uint abup_md5_calc_result = 0;
abup_uint16 abup_current_msgid = 0;
abup_uint8 abup_current_token[ABUP_TOKEN_MAXLEN] = {0};
abup_uint8 abup_current_token_len = 0;
abup_uint8 abup_str_ip[16] = {0};
abup_int8 abup_conn_try_count = 0;
abup_uint32 abup_port = 0;
#endif
volatile abup_uint32 abup_tick = 0;
#ifdef ABUP_BOOTLOADER
abup_uint8 abup_hal_flash_tmp[ABUP_DEFAULT_SECTOR_SIZE];
abup_update_struct* abup_hal_update = (abup_update_struct*)abup_hal_flash_tmp;
#else
abup_update_struct abup_hal_flash_tmp;
abup_update_struct* abup_hal_update = (abup_update_struct*)&abup_hal_flash_tmp;
#endif
abup_uint32 Abup_GetTick(void)
{
    return abup_tick;
}
abup_uint abup_timer_dec_offset(void)
{
#ifdef STM8L05X_HD_VL
    return 50;
#elif defined(ABUP_FREERTOS)
    return 100;
#else
    return 1;
#endif
}
void Abup_IncTick(void)
{
#if defined(STM32L0_DISCO)
    static abup_uint16 count = 0;
    if(count < 7)
    {
        count++;
        return;
    }
    else
    {
        count = 0;
    }
#endif
    abup_tick += abup_timer_dec_offset();
#ifdef ABUP_TIMERS
    AbupRtcTimer();
#endif
#if !defined(ABUP_BOOTLOADER)&&!defined(PKG_USING_ABUP_FOTA)&&!defined(ABUP_FREERTOS)
    Abup_UartRecved();
#endif
}
void Abup_Delay(abup_uint32 Delay)
{
    abup_uint32 tickstart = Abup_GetTick();
    abup_uint32 wait = Delay;

    /* Add a period to guaranty minimum wait */
    if (wait < ABUP_MAX_DELAY)
    {
        wait += abup_timer_dec_offset();
    }

    while((Abup_GetTick() - tickstart) < wait)
    {
    }
}
#if defined(USE_FULL_LL_DRIVER)||defined(USE_HAL_DRIVER)
void AbupGetChipUniqueID(abup_uint32 *UID)
{
#if defined(USE_FULL_LL_DRIVER)
    *UID = LL_GetUID_Word0();
    *(UID + 1) = LL_GetUID_Word1();
    *(UID + 2) = LL_GetUID_Word2();
#else
    *UID = HAL_GetUIDw0();
    *(UID + 1) = HAL_GetUIDw1();
    *(UID + 2) = HAL_GetUIDw2();
#endif
}
#endif
abup_char* abup_get_buf(void)
{
    return abup_hal_buf;
}
abup_uint abup_get_buf_len(void)
{
    return sizeof(abup_hal_buf);
}
void abup_reset_buf(void)
{
    abup_memset(abup_hal_buf,0,ABUP_BUF_MAX_LEN);
}
abup_uint32 abup_get_blocksize(void)
{
    return ABUP_DEFAULT_SECTOR_SIZE;
}
abup_uint32 abup_flash_get_info_addr(void)
{
    return ABUP_UPDATE_INFO_ADDR;
}
abup_uint32 abup_flash_get_delta_addr(void)
{
    return ABUP_UPDATE_DELTA_ADDR;
}
abup_uint32 abup_flash_get_delta_size(void)
{
    return ABUP_UPDATE_DELTA_SIZE - ABUP_DEFAULT_SECTOR_SIZE;
}
abup_bool abup_hal_erase_sector(abup_uint32 base_addr)
{
    abup_int abup_result = 0;
    if((base_addr%ABUP_DEFAULT_SECTOR_SIZE) == 0)
    {
        abup_result = abup_hal_flash_erase(base_addr,ABUP_DEFAULT_SECTOR_SIZE);

    }
    return (abup_result < 0)?abup_false:abup_true;
}
abup_bool abup_hal_legal_moudule(abup_uint moudule)
{
    abup_bool legal = abup_true;
    switch(moudule)
    {
#ifdef ABUP_MODULE_BC28_ENABLE
    case ABUP_MODULE_BC28:
        break;
#endif
#ifdef ABUP_MODULE_M5310_ENABLE
    case ABUP_MODULE_M5310:
        break;
#endif
#ifdef ABUP_MODULE_ESP8266_ENABLE
    case ABUP_MODULE_ESP8266:
        break;
#endif
#ifdef ABUP_MODULE_ESP07S_ENABLE
    case ABUP_MODULE_ESP07S:
        break;
#endif
#ifdef ABUP_MODULE_SIM7020C_ENABLE
    case ABUP_MODULE_SIM7020C:
        break;
#endif
#ifdef ABUP_MODULE_SIM7600_ENABLE
    case ABUP_MODULE_SIM7600:
        break;
#endif
#ifdef ABUP_MODULE_ME3630_ENABLE
    case ABUP_MODULE_ME3630:
        break;
#endif
#ifdef ABUP_MODULE_SLM152_ENABLE
    case ABUP_MODULE_SLM152:
        break;
#endif
    default:
    {
        legal = abup_false;
    }
    break;
    }
    return legal;
}
#if defined(ABUP_WIFI_SSID_PWD)
abup_bool abup_hal_check_ssid_pwd(abup_char* ssid,abup_uint16 ssid_len,abup_char* pwd,abup_uint16 pwd_len)
{
    abup_update_struct* abup_update = abup_hal_get_update();
    if((ssid_len > 2)
            &&(ssid_len < sizeof(abup_update->wifi_ssid))
            &&(pwd_len > 4)
            &&(pwd_len < sizeof(abup_update->wifi_pwd))
            &&(!abup_str_is_same_char(ssid,ssid_len,0))
            &&(!abup_str_is_same_char(ssid,ssid_len,0xFF))
            &&(!abup_str_is_same_char(pwd,pwd_len,0))
            &&(!abup_str_is_same_char(pwd,pwd_len,0xFF)))
    {
        return abup_true;
    }
    return abup_false;
}
#endif
abup_int abup_hal_get_key_version(void)
{
    return ABUP_KEY_VERSION;
}
abup_bool abup_hal_check_key_version(void)
{
    if(abup_hal_get_key_version() == 0xFF)
        return abup_false;
    else
        return abup_true;
}
abup_bool abup_hal_server_support_key(void)
{
    if(abup_hal_get_serverNO() == 1)
        return abup_false;
    else
        return abup_true;
}
abup_uint abup_get_default_module(void)
{
    abup_update_struct* abup_update = abup_hal_get_update();
    return (abup_uint)abup_update->default_module;
}
abup_uint8 abup_hal_get_serverNO(void)
{
    abup_update_struct* abup_update = abup_hal_get_update();
    return abup_update->serverNO;
}
abup_uint8* abup_hal_get_mid(void)
{
    abup_update_struct* abup_update = abup_hal_get_update();
    return abup_update->mid;
}
abup_uint32 abup_hal_get_delta_id(void)
{
    abup_update_struct* abup_update = abup_hal_get_update();
    return abup_update->delta_id;
}
abup_uint8* abup_hal_get_deviceId(void)
{
    abup_update_struct* abup_update = abup_hal_get_update();
    return abup_update->deviceId;
}
abup_uint8* abup_hal_get_deviceSecret(void)
{
    abup_update_struct* abup_update = abup_hal_get_update();
    return abup_update->deviceSecret;
}
abup_uint16 abup_hal_get_update_result(void)
{
    abup_update_struct* abup_update = abup_hal_get_update();
    return abup_update->update_result;
}
void abup_hal_set_update_result(abup_uint16 result)
{
    abup_update_struct* abup_update = abup_hal_get_update();
    abup_update->update_result = result;
    abup_hal_set_update(abup_update);
}
abup_activation_code_t* abup_hal_get_activation_code(void)
{
    abup_update_struct* abup_update = abup_hal_get_update();
    return &abup_update->activation_code;
}
abup_activation_code_t* abup_hal_get_activation_code_bk(void)
{
    abup_update_struct* abup_update = abup_hal_get_update();
    return &abup_update->activation_code_bk;
}
void abup_hal_update_activation_code(void)
{
    abup_update_struct* abup_update = abup_hal_get_update();
    abup_hal_set_update(abup_update);
}
void abup_hal_reset_activation_code(void)
{
    if(abup_hal_server_support_key())
    {
        abup_update_struct* abup_update = abup_hal_get_update();
        abup_memset(&abup_update->activation_code_bk,0,sizeof(abup_activation_code_t));
        abup_memcpy(&abup_update->activation_code_bk,&abup_update->activation_code,sizeof(abup_activation_code_t));
        abup_memset(&abup_update->activation_code,0,sizeof(abup_update->activation_code));
        abup_hal_set_update(abup_update);
    }
}
abup_update_struct* abup_hal_get_update(void)
{
    static abup_bool inited = abup_false;
    if(inited == abup_false)
    {
        abup_uint32 addr = abup_flash_get_info_addr();
        abup_uint abup_hal_update_size = sizeof(abup_hal_flash_tmp);
        abup_memset(abup_hal_update,0,abup_hal_update_size);
        abup_hal_flash_read(addr,(abup_uint8*)abup_hal_update,abup_hal_update_size);
        if(abup_hal_update->inited != 0x8)
        {
            abup_hal_flash_erase(addr,abup_hal_update_size);
            abup_memset(abup_hal_update,0,abup_hal_update_size);
            abup_hal_update->inited = 0x8;
//            abup_hal_update->serverNO = 0x2;//测试服务器
            abup_hal_update->default_module = (abup_uint)-1;
            abup_hal_flash_write(addr,(abup_uint8*)abup_hal_update,abup_hal_update_size);
            abup_memset(abup_hal_update,0,abup_hal_update_size);
            abup_hal_flash_read(addr,(abup_uint8*)abup_hal_update,abup_hal_update_size);
        }
#ifndef ABUP_BOOTLOADER
        if(!abup_hal_legal_moudule(abup_hal_update->default_module))
        {
            for(abup_uint i = 0; i < ABUP_MODULE_MAX; i++)
            {
                if(abup_hal_legal_moudule(i))
                {
                    abup_hal_flash_erase(addr,abup_hal_update_size);
                    abup_hal_update->default_module = i;
                    abup_hal_flash_write(addr,(abup_uint8*)abup_hal_update,abup_hal_update_size);
                    abup_memset(abup_hal_update,0,abup_hal_update_size);
                    abup_hal_flash_read(addr,(abup_uint8*)abup_hal_update,abup_hal_update_size);
                    break;
                }
            }
        }
#endif
        inited = abup_true;
    }
    return (abup_update_struct*)abup_hal_update;
}
abup_bool abup_hal_set_update(abup_update_struct *abup_update)
{
    abup_uint32 addr = abup_flash_get_info_addr();
    abup_uint abup_hal_update_size = sizeof(abup_hal_flash_tmp);
    if(abup_hal_erase_sector(addr))
    {
        if(abup_hal_flash_write(addr, (abup_uint8*)abup_update, abup_hal_update_size) == abup_hal_update_size)
        {
            return abup_true;
        }
    }
    return abup_false;
}
abup_bool abup_hal_write_delta(abup_uint32 base_addr,abup_uint16 index,abup_uint8* data,abup_uint16 len)
{
    abup_bool result = abup_false;
    if(abup_hal_erase_sector(base_addr + index *ABUP_DATA_SEQ_MAX_LEN))
    {
        if(abup_hal_flash_write(base_addr + index *ABUP_DATA_SEQ_MAX_LEN, data, len) == len)
        {
            result = abup_true;
        }
    }
    return result;
}
abup_int abup_hal_flash_read(abup_uint32 addr, abup_uint8 *buf, abup_uint size)
{
#if defined(PKG_USING_ABUP_FOTA)
    const struct fal_partition  *device;
    device = fal_partition_find(ABUP_RTTHREAD_FLASH);
    return fal_partition_read(device, addr - ABUP_UPDATE_INFO_ADDR, buf, size);
#else
    return abup_flash_read(addr,buf,size);
#endif
}

abup_int abup_hal_flash_write(abup_uint32 addr, abup_uint8 *buf, abup_uint size)
{
#if defined(PKG_USING_ABUP_FOTA)
    const struct fal_partition  *device;
    device = fal_partition_find(ABUP_RTTHREAD_FLASH);
    return fal_partition_write(device, addr - ABUP_UPDATE_INFO_ADDR, buf, size);
#else
    return abup_flash_write(addr,buf,size);
#endif
}
abup_int abup_hal_flash_erase(abup_uint32 addr, abup_uint size)
{
#if defined(PKG_USING_ABUP_FOTA)
    const struct fal_partition  *device;
    device = fal_partition_find(ABUP_RTTHREAD_FLASH);
    return fal_partition_erase(device, addr - ABUP_UPDATE_INFO_ADDR, size);
#else
    return abup_erase_sector(addr,size);
#endif
}
#ifdef ABUP_QUEUE_MSG
void abup_hal_app_msg(void* ptr)
{
    if(abup_app_msg_cb != NULL)
    {
        if(abup_exist_msgid(ABUP_MSG_ATP_SEND_ABUP_MSG_2_APP) == NULL)
        {
            abup_obtain_msg(ABUP_MSG_ATP_SEND_ABUP_MSG_2_APP, abup_app_msg_cb, ptr);
        }
    }
}
void abup_hal_start(abup_msg_cb app_msg_cb,abup_msg_cb atp_msg_cb,abup_state_result state_result)
{
    abup_action_result = ABUP_RESULT_FAIL;
    abup_app_msg_cb = app_msg_cb;
    abup_atp_msg_cb = atp_msg_cb;
    abup_state_result_cb = state_result;
    abup_atp_init();
    abup_display_update(abup_false);
}
abup_bool abup_hal_started(void)
{
    if((abup_app_msg_cb != NULL)
            ||(abup_atp_msg_cb != NULL)
            ||(abup_state_result_cb != NULL))
    {
        abup_info_printf("[Abup] try later!\r\n");
        return abup_true;
    }
    return abup_false;
}
void abup_hal_stop(void)
{
    abup_app_msg_cb = NULL;
    abup_atp_msg_cb = NULL;
    abup_state_result_cb = NULL;
    abup_atp_at_stop_timer();
    abup_display_update(abup_true);
    abup_msg_from_app(abup_action_result);
}
abup_bool abup_hal_get_state(void)
{
    if(abup_app_msg_cb == NULL)
        return abup_false;
    else
        return abup_true;
}
#endif
#if defined(ABUP_BOOTLOADER)
#else
abup_uint8 *abup_get_hal_data(void)
{
    return (abup_uint8 *)abup_hal_data;
}
abup_uint abup_get_hal_data_len(void)
{
    return ABUP_HAL_DATA_MAX_LEN;
}
void abup_reset_hal_data(void)
{
    abup_memset(abup_hal_data,0,ABUP_HAL_DATA_MAX_LEN);
}
download_uri_struct* abup_get_download_atp_uri(void)
{
    return (download_uri_struct *)download_atp_uri;
}
abup_uint8* abup_hal_get_download_host_ip(void)
{
    return (abup_uint8 *)download_atp_uri->download_host_ip;
}
abup_uint32 abup_hal_get_download_delta_id(void)
{
    return (abup_uint32)download_atp_uri->download_delta_id;
}
abup_uint32 abup_hal_get_download_delta_size(void)
{
    return (abup_uint32)download_atp_uri->download_delta_size;
}
abup_char* abup_hal_get_download_url(void)
{
    return (abup_char*)download_atp_uri->download_url;
}
abup_char* abup_hal_get_download_host(void)
{
    return (abup_char*)download_atp_uri->download_host;
}
abup_uint32 abup_get_download_index(void)
{
    return download_index;
}
abup_uint32 abup_get_download_index_max(void)
{
    return download_index_max;
}
abup_int8 abup_get_conn_try_count(void)
{
    return abup_conn_try_count;
}
void abup_set_conn_try_count(abup_int8 count)
{
    abup_conn_try_count = count;
}
abup_bool abup_legal_ID(abup_int32 mid,abup_int32 msgid)
{
    if((mid > (msgid - abup_get_conn_try_count()))&&(mid < (msgid + 1)))
    {
        return abup_true;
    }
    abup_info_printf("mid = %d msgid=%d\r\n",mid,msgid);
    return abup_false;
}
abup_int abup_isipv4str(const abup_char *str)
{
    abup_int i, a[4];
    if(abup_sscanf(str,"%d.%d.%d.%d", &a[0], &a[1], &a[2], &a[3]) != 4 )
    {
        return 0;
    }
    for(i=0; i<4; i++)
    {
        if (a[i] < 0 || a[i] > 255)
        {
            return 0;
        }
        else
        {
            continue;
        }
    }
    if((a[0] == 0)&&(a[1] == 0)&&(a[2] == 0)&&(a[3] == 0))
    {
        return 0;
    }
    return 1;
}
abup_uint8* abup_get_str_ip(abup_uint8* ip)
{
    abup_memset(abup_str_ip,0,16);
    if(ip != NULL)
    {
        abup_snprintf((abup_char *)abup_str_ip,sizeof(abup_str_ip),"%d.%d.%d.%d",ip[0],ip[1],ip[2],ip[3]);
    }
    return abup_str_ip;
}
abup_bool abup_get_domain(abup_char *server,abup_char* domain,abup_int16 domain_len,abup_int* port)
{
    if(server)
    {
        abup_char* c = abup_strstr(server,":");
        if(c&&domain)
        {
            if(port != NULL)
            {
                abup_memset(domain,0,domain_len);
                abup_strcpy(domain,c + 1);
                *port = abup_atoi(domain);
            }
            abup_memset(domain,0,domain_len);
            abup_memcpy(domain,server,c - server);
            return abup_true;
        }
    }
    return abup_false;
}
abup_char* abup_get_server_host(void)
{
#if ((ABUP_DEFAULT_NETWORK_PROTOCOL == ABUP_PROTOCOL_COAP)||(ABUP_DEFAULT_NETWORK_PROTOCOL == ABUP_PROTOCOL_COAP_HTTP))
    if(abup_get_default_protocol() == ABUP_PROTOCOL_COAP)
        return abup_get_coap_server_host();
#endif
#if ((ABUP_DEFAULT_NETWORK_PROTOCOL == ABUP_PROTOCOL_HTTP)||(ABUP_DEFAULT_NETWORK_PROTOCOL == ABUP_PROTOCOL_COAP_HTTP))
    if(abup_get_default_protocol() == ABUP_PROTOCOL_HTTP)
        return abup_get_http_server_host();
#endif
    return NULL;
}
abup_bool abup_hal_para_url(abup_char *url,abup_uint16 url_len)
{
    abup_uint8 *abup_p = NULL;
    abup_uint8 flag = 0;
    abup_uint8 *p1 = NULL;
    abup_uint8 *p2 = NULL;
    abup_uint8 length = 0;
    abup_uint8 id = 0;
    abup_char *tmp = (abup_char *)abup_get_buf();
    abup_uint8 len = 0;
    download_uri_struct* download_uri = abup_get_download_atp_uri();

    abup_p = (abup_uint8*)url;
    flag = 0;
    abup_memset(download_uri,0,sizeof(download_uri_struct));
    while((abup_p != NULL)&&(abup_p < (abup_uint8 *)(url + url_len)))
    {
        abup_bool max = abup_false;
        if(*abup_p&0xC0)
        {
            abup_reset_buf();
            if(*abup_p&0x08)
            {
                length = *(abup_p + 2);
                max = abup_true;
            }
            else
            {
                length = *abup_p - 0xC0;
            }
            id = *(abup_p + 1);
            if(id == 0x01)
            {
                p1 = (abup_uint8*)abup_strstr((const abup_char*)url,"coap://");
                if(p1 == NULL)
                    break;
                len = abup_strlen("coap://");
                p2 = (abup_uint8*)abup_strstr((const abup_char*)p1 + len,":");
                if(p2 == NULL)
                    break;
                p1 += len;
                if(p1 == NULL)
                    break;
                if((p2 - p1) < sizeof(download_uri->download_host))
                    abup_memcpy(download_uri->download_host,p1,p2 - p1);
                else
                    break;
//                ABUP_hal_parse_dns(download_uri->download_host,download_uri->download_host_ip);
                p1 = p2 + 1;
                if(p1 == NULL)
                    break;
                p2 = (abup_uint8*)abup_strstr((const abup_char*)p1,"/");
                if(p2 == NULL)
                    break;
                abup_memcpy(tmp,p1,p2 - p1);
                abup_sscanf(tmp,"%d",&download_uri->download_port);
                abup_memset(tmp,0,8);
                if((abup_p + 2 + length + (max?1:0) - p2) < sizeof(download_uri->download_url))
                    abup_memcpy(download_uri->download_url,p2,abup_p + 2 + length + (max?1:0) - p2);
                else
                    break;
                flag |= ABUP_HAL_FLAG_1;
            }
            else if(id == 0x64)
            {
                p1 = abup_p + 2 + (max?1:0);
                if(p1 == NULL)
                    break;
                abup_memcpy(tmp,p1,length);
                abup_sscanf(tmp,"%d",&download_uri->download_delta_size);
                abup_memset(tmp,0,8);
                flag |= ABUP_HAL_FLAG_2;
            }
            else if(id == 0x65)
            {
                p1 = abup_p + 2 + (max?1:0);
                if(p1 == NULL)
                    break;
                abup_memcpy(tmp,p1,length);
                abup_sscanf(tmp,"%d",&download_uri->download_delta_id);
                abup_memset(tmp,0,8);
                flag |= ABUP_HAL_FLAG_4;
            }
            else if(id == 0x66)
            {
                p1 = abup_p + 2 + (max?1:0);
                if(p1 == NULL)
                    break;
                abup_hexstr2byte((abup_char*)download_uri->download_md5,(const abup_uint8 *)p1,(sizeof(download_uri->download_md5)*2) < length?(sizeof(download_uri->download_md5)*2):length);
                flag |= ABUP_HAL_FLAG_8;
            }
            if(flag == ABUP_HAL_FLAG_ALL)
                break;
            abup_p = abup_p + 2 + length + (max?1:0);
        }
        else
        {
            abup_p++;
        }
    }

    if(flag == ABUP_HAL_FLAG_ALL)
    {
        return abup_true;
    }
    else
        return abup_false;
}
abup_uint32* abup_get_host_port(void)
{
    return &abup_port;
}
abup_bool abup_hal_para_http(abup_uint8 *data,abup_http_parameter* tmp,abup_uint8 tmplen)
{
    abup_uint8 i = 0;
    abup_bool result = abup_false;
    abup_memset(tmp,0,tmplen);
    abup_memcpy(tmp[i++].type,"\"deltaUrl\":",abup_strlen("\"deltaUrl\":"));
    abup_memcpy(tmp[i++].type,"\"deltaID\":",abup_strlen("\"deltaID\":"));
    abup_memcpy(tmp[i++].type,"\"fileSize\":",abup_strlen("\"fileSize\":"));
    abup_memcpy(tmp[i++].type,"\"md5sum\":",abup_strlen("\"md5sum\":"));
#ifdef ABUP_SLIM_RES
#else
    abup_memcpy(tmp[i++].type,"\"bakUrl\":",abup_strlen("\"bakUrl\":"));
#endif
    if(abup_parse_http_data((abup_char*)data,tmp,i))
    {
        download_uri_struct* uri = abup_get_download_atp_uri();
        abup_char * id = abup_get_product_id();
        abup_memset(uri->download_md5,0,sizeof(uri->download_md5));
        if((tmp[1].content_len - 2) < sizeof(uri->download_md5))
        {
            abup_memcpy(uri->download_md5,tmp[1].content + 1,tmp[1].content_len - 2);
            uri->download_delta_id = abup_atoi(uri->download_md5);
        }
        uri->download_port = 80;
        abup_memset(uri->download_md5,0,sizeof(uri->download_md5));
        if(tmp[2].content_len < sizeof(uri->download_md5))
        {
            abup_memcpy(uri->download_md5,tmp[2].content,tmp[2].content_len);
        }
        uri->download_delta_size = abup_atoi(uri->download_md5);
        abup_memset(uri->download_md5,0,sizeof(uri->download_md5));
        abup_snprintf((abup_char*)uri->download_md5,ABUP_DOWNLOAD_MD5_LEN,"/%s",id);
#ifdef ABUP_SLIM_RES
        abup_char* http = NULL;
        abup_char* bk = NULL;
#else
        abup_char* bk = abup_strstr((const abup_char*)tmp[4].content,(const abup_char*)uri->download_md5);
        abup_char* http = abup_strstr((const abup_char*)tmp[4].content,(const abup_char*)"http://");
        if(http)
        {
            http += abup_strlen("http://");
            if(bk)
            {
                if(((abup_char*)bk - (abup_char*)http) < sizeof(uri->bkup_host))
                {
                    abup_memcpy(uri->bkup_host,http,((abup_char*)bk - (abup_char*)http));
                }
            }
        }
#endif
        bk = abup_strstr((const abup_char*)tmp[0].content,(const abup_char*)"com/");
        http = abup_strstr((const abup_char*)tmp[0].content,(const abup_char*)"http://");
        if(http)
        {
            abup_uint8 http_len = abup_strlen("http://");
            if(bk)
            {
                bk += 3;
                abup_memcpy(uri->download_host,http + http_len,((abup_char*)bk - (abup_char*)http - http_len));
                if((http + tmp[0].content_len - 2 - bk) < sizeof(uri->download_url))
                {
                    abup_memcpy(uri->download_url,bk,http + tmp[0].content_len - 2 - bk);
                }
                result = abup_true;
            }
        }
        abup_memset(uri->download_md5,0,sizeof(uri->download_md5));
        abup_hexstr2byte((abup_char*)uri->download_md5,(const abup_uint8 *)tmp[3].content + 1,(tmp[3].content_len - 2)>(sizeof(uri->download_md5)*2)?(sizeof(uri->download_md5)*2):(tmp[3].content_len - 2));
    }
    return result;
}
abup_uint abup_MD5Calc_result(void)
{
    download_uri_struct* abup_app_uri = abup_get_download_atp_uri();
    abup_char* download_md5 = abup_get_buf();
    abup_uint result = 8;
    abup_memset(download_md5,0,ABUP_DOWNLOAD_MD5_LEN + 1);
    if(abup_MD5Calc(abup_flash_get_delta_addr(),abup_app_uri->download_delta_size,download_md5) == 0)
    {
        if(abup_memcmp(download_md5,abup_app_uri->download_md5,ABUP_DOWNLOAD_MD5_LEN) == 0)
        {
            result = 1;
        }
    }
    if(result == 1)
    {
        abup_info_printf("[Abup] MD5 calc OK\r\n");
    }
    else
    {
        abup_info_printf("[Abup] MD5 calc error\r\n");
    }
    return result;
}
abup_int abup_MD5Calc(abup_uint addr, abup_uint buflen, abup_char *md5out)
{
    abup_uint idx=0, segnum=0, remain=0;
    abup_uint16 segmaxsize = abup_get_data_max_len();
    ABUP_MD5_CTX md5;
    abup_uint8* tmp = NULL;
    if((buflen==0) || !md5out)
    {
        return -1;
    }

    tmp = (abup_uint8*)abup_get_ATBuf();
    AbupMD5Init(&md5);

    segnum = buflen / segmaxsize;
    remain = buflen - (segnum*segmaxsize);

    if((segnum==0) && (remain==0))
        return -1;

    for(idx=0; idx<segnum; idx++)
    {
        abup_memset(tmp,0,segmaxsize);
        abup_hal_flash_read(addr + idx*segmaxsize, (abup_uint8 *)tmp, segmaxsize);
        AbupMD5Update(&md5, tmp, segmaxsize);
    }

    if(remain > 0)
    {
        abup_memset(tmp,0,segmaxsize);
        abup_hal_flash_read(addr + buflen - remain, (abup_uint8 *)tmp, segmaxsize);
        AbupMD5Update(&md5, tmp, remain);
    }

    AbupMD5Final(&md5,(abup_uint8 *)md5out);
    return 0;
}
abup_int8 abup_hal_get_current_state(void)
{
    abup_int8 abup_state = -1;
#if defined(PKG_USING_ABUP_FOTA)
    extern abup_uint8 AbupState;
    abup_state = AbupState;
#else
    switch(*abup_get_atp_state())
    {
    case ABUP_SETUP_REGISTER:
    {
        abup_state = STATE_RG;
    }
    break;
    case ABUP_SETUP_CHECK_VERSION:
    {
        abup_state = STATE_CV;
    }
    break;
    case ABUP_SETUP_GET_KEY:
    {
        abup_state = STATE_KY;
    }
    break;
    case ABUP_SETUP_RTP_DOWNLOAD:
    {
        abup_state = STATE_RD;
    }
    break;
    case ABUP_SETUP_RTP_UPDATE:
    {
        abup_state = STATE_RU;
    }
    break;
    case ABUP_SETUP_DOWNLOAD:
    {
        abup_state = STATE_DL;
    }
    break;
    default:
        break;
    }
#endif
    return abup_state;
}
void abup_hal_update_device(abup_uint8* mid,abup_int16 mid_len,abup_uint8* deviceId,abup_int16 deviceIdLen,abup_uint8* deviceSecret,abup_int16 deviceSecretLen)
{
    abup_update_struct* abup_update = abup_hal_get_update();
    abup_bool NeedSave = abup_false;
    if((mid != NULL)&&(mid_len > 0))
    {
        if(abup_strncmp((const abup_char*)mid,(const abup_char*)abup_update->mid,ABUP_MID_MAXLEN) != 0)
        {
            abup_memset(abup_update->mid,0,sizeof(abup_update->mid));
            abup_memset(abup_update->deviceId,0,sizeof(abup_update->deviceId));
            abup_memset(abup_update->deviceSecret,0,sizeof(abup_update->deviceSecret));
            abup_memcpy(abup_update->mid,mid,mid_len);
            NeedSave = abup_true;
        }
    }
    else if((deviceSecret != NULL)&&(deviceSecretLen > 0)&&(deviceId != NULL)&&(deviceIdLen > 0))
    {
        deviceIdLen = deviceIdLen>sizeof(abup_update->deviceId)?sizeof(abup_update->deviceId):deviceIdLen;
        deviceSecretLen = deviceSecretLen>sizeof(abup_update->deviceSecret)?sizeof(abup_update->deviceSecret):deviceSecretLen;
        abup_memset(abup_update->deviceId,0,sizeof(abup_update->deviceId));
        abup_memset(abup_update->deviceSecret,0,sizeof(abup_update->deviceSecret));
        abup_memcpy(abup_update->deviceSecret,deviceSecret,deviceSecretLen);
        abup_memcpy(abup_update->deviceId,deviceId,deviceIdLen);
        NeedSave = abup_true;
    }
    if(NeedSave)
    {
        abup_hal_set_update(abup_update);
    }
}
#endif
