/*****************************************************************************
* Copyright (c) 2018 ABUP.Co.Ltd. All rights reserved.
* File name: abup_hal.c
* Description: 逻辑、AT处理
* Author: Ray Shen
* Version: v1.0
* Date: 20181101
*****************************************************************************/
#include "abup_hal.h"
#if defined(ABUP_RTTHREAD)
#include "fal.h"
#else
#include "abup_hal_flash.h"
#include "abup_hal_uart.h"
#endif
abup_char abup_hal_buf[ABUP_BUF_MAX_LEN];
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
#if !defined(ABUP_BOOTLOADER)&&!defined(ABUP_RTTHREAD)&&!defined(ABUP_FREERTOS)
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
abup_uint32 abup_hal_get_blocksize(void)
{
    return ABUP_DEFAULT_SECTOR_SIZE;
}
abup_uint32 abup_hal_get_info_addr(void)
{
    return ABUP_UPDATE_ADDR + ABUP_UPDATE_SIZE - ABUP_DEFAULT_SECTOR_SIZE;
}
abup_uint32 abup_hal_get_backup_addr(void)
{
    return ABUP_UPDATE_ADDR + ABUP_UPDATE_SIZE - 2*ABUP_DEFAULT_SECTOR_SIZE;
}
abup_uint32 abup_hal_get_app_addr(void)
{
    return ABUP_FLASH_BASE_ADDR + ABUP_BL_SIZE;
}
abup_uint32 abup_hal_get_delta_addr(void)
{
    return ABUP_UPDATE_ADDR;
}
abup_uint32 abup_hal_get_delta_size(void)
{
    return ABUP_UPDATE_SIZE - 2*ABUP_DEFAULT_SECTOR_SIZE;
}
abup_bool abup_hal_erase_sector(abup_uint8 type,abup_uint32 addr)
{
    abup_int abup_result = 0;
    if((addr%ABUP_DEFAULT_SECTOR_SIZE) == 0)
    {
        abup_result = abup_hal_flash_erase(type,addr,ABUP_DEFAULT_SECTOR_SIZE);

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
#if (ABUP_KEY_VERSION == 0xFF)
    return NULL;
#else
    return &abup_update->activation_code;
#endif
}
abup_activation_code_t* abup_hal_get_activation_code_bk(void)
{
    abup_update_struct* abup_update = abup_hal_get_update();
#if (ABUP_KEY_VERSION == 0xFF)
    return NULL;
#else
    return &abup_update->activation_code_bk;
#endif
}
void abup_hal_update_activation_code(void)
{
    abup_update_struct* abup_update = abup_hal_get_update();
    abup_hal_set_update(abup_update);
}
void abup_hal_reset_activation_code(void)
{
#if (ABUP_KEY_VERSION != 0xFF)
    if(abup_hal_server_support_key())
    {
        abup_update_struct* abup_update = abup_hal_get_update();
        abup_memset(&abup_update->activation_code_bk,0,sizeof(abup_activation_code_t));
        abup_memcpy(&abup_update->activation_code_bk,&abup_update->activation_code,sizeof(abup_activation_code_t));
        abup_memset(&abup_update->activation_code,0,sizeof(abup_update->activation_code));
        abup_hal_set_update(abup_update);
    }
#endif
}
abup_update_struct* abup_hal_get_update(void)
{
    static abup_bool inited = abup_false;
    if(inited == abup_false)
    {
        abup_uint abup_hal_update_size = sizeof(abup_hal_flash_tmp);
        abup_memset(abup_hal_update,0,abup_hal_update_size);
        abup_hal_flash_read(ABUP_FILETYPE_PATCH_INFO,0,(abup_uint8*)abup_hal_update,abup_hal_update_size);
        if(abup_hal_update->inited != 0x8)
        {
            abup_hal_flash_erase(ABUP_FILETYPE_PATCH_INFO,0,abup_hal_update_size);
            abup_memset(abup_hal_update,0,abup_hal_update_size);
            abup_hal_update->inited = 0x8;
//            abup_hal_update->serverNO = 0x2;//测试服务器
            abup_hal_update->default_module = (abup_uint)-1;
            abup_hal_flash_write(ABUP_FILETYPE_PATCH_INFO,0,(abup_uint8*)abup_hal_update,abup_hal_update_size);
            abup_memset(abup_hal_update,0,abup_hal_update_size);
            abup_hal_flash_read(ABUP_FILETYPE_PATCH_INFO,0,(abup_uint8*)abup_hal_update,abup_hal_update_size);
        }
#ifndef ABUP_BOOTLOADER
        if(!abup_hal_legal_moudule(abup_hal_update->default_module))
        {
            for(abup_uint i = 0; i < ABUP_MODULE_MAX; i++)
            {
                if(abup_hal_legal_moudule(i))
                {
                    abup_hal_flash_erase(ABUP_FILETYPE_PATCH_INFO,0,abup_hal_update_size);
                    abup_hal_update->default_module = i;
                    abup_hal_flash_write(ABUP_FILETYPE_PATCH_INFO,0,(abup_uint8*)abup_hal_update,abup_hal_update_size);
                    abup_memset(abup_hal_update,0,abup_hal_update_size);
                    abup_hal_flash_read(ABUP_FILETYPE_PATCH_INFO,0,(abup_uint8*)abup_hal_update,abup_hal_update_size);
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
    abup_uint abup_hal_update_size = sizeof(abup_hal_flash_tmp);
    if(abup_hal_erase_sector(ABUP_FILETYPE_PATCH_INFO,0))
    {
        if(abup_hal_flash_write(ABUP_FILETYPE_PATCH_INFO,0,(abup_uint8*)abup_update, abup_hal_update_size) == abup_hal_update_size)
        {
            return abup_true;
        }
    }
    return abup_false;
}
abup_bool abup_hal_write_delta(abup_uint16 index,abup_uint8* data,abup_uint16 len)
{
    if(abup_hal_erase_sector(ABUP_FILETYPE_PATCH,index *ABUP_DATA_SEQ_MAX_LEN))
    {
        if(abup_hal_flash_write(ABUP_FILETYPE_PATCH,index *ABUP_DATA_SEQ_MAX_LEN, data, len) == len)
        {
            return abup_true;
        }
    }
    return abup_false;
}
#if defined(ABUP_RTTHREAD)
abup_char* abup_hal_get_device_name(abup_uint8 type)
{
    abup_char* device = NULL;
    switch(type)
    {
    case ABUP_FILETYPE_APP:
        device = ABUP_RTTHREAD_APP;
        break;
    case ABUP_FILETYPE_BACKUP:
    case ABUP_FILETYPE_PATCH:
    case ABUP_FILETYPE_PATCH_INFO:
        device = ABUP_RTTHREAD_FLASH;
        break;
    default:
        break;
    }
    return device;
}
abup_uint32 abup_hal_get_device_addr(abup_uint8 type,abup_uint32 addr)
{
    abup_uint32 address = 0;
    switch(type)
    {
    case ABUP_FILETYPE_APP:
        address = addr;
        break;
    case ABUP_FILETYPE_BACKUP:
        address = ABUP_UPDATE_SIZE - 2*ABUP_DEFAULT_SECTOR_SIZE + addr;
        break;
    case ABUP_FILETYPE_PATCH:
        address = addr;
        break;
    case ABUP_FILETYPE_PATCH_INFO:
        address = ABUP_UPDATE_SIZE - ABUP_DEFAULT_SECTOR_SIZE + addr;
        break;
    default:
        break;
    }
    return address;
}
#else
abup_uint32 abup_hal_get_addr(abup_uint8 type,abup_uint32 addr)
{
    abup_uint32 address = 0;
    switch(type)
    {
    case ABUP_FILETYPE_APP:
        address = abup_hal_get_app_addr() + addr;
        break;
    case ABUP_FILETYPE_BACKUP:
        address = abup_hal_get_backup_addr() + addr;
        break;
    case ABUP_FILETYPE_PATCH:
        address = abup_hal_get_delta_addr() + addr;
        break;
    case ABUP_FILETYPE_PATCH_INFO:
        address = abup_hal_get_info_addr() + addr;
        break;
    default:
        break;
    }
    return address;
}
#endif
abup_int abup_hal_flash_read(abup_uint8 type,abup_uint32 addr, abup_uint8 *buf, abup_uint size)
{
#if defined(ABUP_RTTHREAD)
    const struct fal_partition  *device;
    device = fal_partition_find(abup_hal_get_device_name(type));
    return fal_partition_read(device, abup_hal_get_device_addr(type,addr), buf, size);
#else
    return abup_flash_read(abup_hal_get_addr(type,addr),buf,size);
#endif
}

abup_int abup_hal_flash_write(abup_uint8 type,abup_uint32 addr, abup_uint8 *buf, abup_uint size)
{
#if defined(ABUP_RTTHREAD)
    const struct fal_partition  *device;
    device = fal_partition_find(abup_hal_get_device_name(type));
    return fal_partition_write(device, abup_hal_get_device_addr(type,addr), buf, size);
#else
    return abup_flash_write(abup_hal_get_addr(type,addr),buf,size);
#endif
}
abup_int abup_hal_flash_erase(abup_uint8 type,abup_uint32 addr, abup_uint size)
{
#if defined(ABUP_RTTHREAD)
    const struct fal_partition  *device;
    device = fal_partition_find(abup_hal_get_device_name(type));
    return fal_partition_erase(device, abup_hal_get_device_addr(type,addr), size);
#else
    return abup_erase_sector(abup_hal_get_addr(type,addr),size);
#endif
}

