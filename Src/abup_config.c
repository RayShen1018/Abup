/*****************************************************************************
* Copyright (c) 2018 ABUP.Co.Ltd. All rights reserved.
* File name: abup_config.c
* Description:
* Author: Ray Shen
* Version: v1.0
* Date: 20181101
*****************************************************************************/
#include "abup_typedef.h"

abup_char* abup_get_manufacturer(void)
{
    return (abup_char*)ADUPS_FOTA_SERVICE_OEM;
}
abup_char* abup_get_model_number(void)
{
    return (abup_char*)ADUPS_FOTA_SERVICE_MODEL;
}
abup_char* abup_get_product_id(void)
{
    return (abup_char*)ADUPS_FOTA_SERVICE_PRODUCT_ID;
}
abup_char* abup_get_product_sec(void)
{
    return (abup_char*)ADUPS_FOTA_SERVICE_PRODUCT_SEC;
}
abup_char* abup_get_device_type(void)
{
    return (abup_char*)ADUPS_FOTA_SERVICE_DEVICE_TYPE;
}
abup_char* abup_get_platform(void)
{
    return (abup_char*)ADUPS_FOTA_SERVICE_PLATFORM;
}
abup_char* abup_get_sdk_version(void)
{
    return (abup_char*)ADUPS_FOTA_SDK_VER;
}
abup_char* abup_get_apk_version(void)
{
#if defined(PKG_USING_ABUP_FOTA)
    return (abup_char*)ADUPS_FOTA_RTT_VERSION;
#else
    return (abup_char*)ADUPS_FOTA_APP_VERSION;
#endif
}
abup_char* abup_get_firmware_version(void)
{
    return (abup_char*)ABUP_FIRMWARE_VERSION;
}
abup_char* abup_get_hw_version(void)
{
    return (abup_char*)ABUP_HW_VERSION;
}
abup_char* abup_get_sw_version(void)
{
    return (abup_char*)ABUP_SW_VERSION;
}
abup_char* abup_get_network_type(void)
{
    return (abup_char*)ABUP_NETWORK_TYPE;
}

abup_uint abup_get_try_timer(void)
{
    abup_uint try_timer = 0;
    switch(abup_get_default_module())
    {
#ifdef ABUP_MODULE_SIM7020C_ENABLE
    case ABUP_MODULE_SIM7020C:
    {
        try_timer = 20000;
    }
    break;
#endif
    default:
    {
        try_timer = 10000;
    }
    break;
    }
    return try_timer;
}
abup_uint32 abup_get_default_baudrate(void)
{
#ifdef ABUP_MODULE_ESP8266_ENABLE
    if(abup_get_default_module() == ABUP_MODULE_ESP8266)
    {
        return 115200;
    }
#endif
#ifdef ABUP_MODULE_ME3630_ENABLE
    if(abup_get_default_module() == ABUP_MODULE_ME3630)
    {
        return 115200;
    }
#endif
#ifdef ABUP_MODULE_SLM152_ENABLE
    if(abup_get_default_module() == ABUP_MODULE_SLM152)
    {
        return 57600;
    }
#endif
    return 9600;
}
abup_uint8 abup_get_default_protocol(void)
{
#if defined(PKG_USING_ABUP_FOTA)
    return (abup_uint8)1;
#elif ((ABUP_DEFAULT_NETWORK_PROTOCOL == ABUP_PROTOCOL_COAP)||(ABUP_DEFAULT_NETWORK_PROTOCOL == ABUP_PROTOCOL_HTTP))
    return (abup_uint8)ABUP_DEFAULT_NETWORK_PROTOCOL;
#else
    abup_uint8 protocol = 0;
    switch(abup_get_default_module())
    {
#ifdef ABUP_MODULE_ESP8266_ENABLE
    case ABUP_MODULE_ESP8266:
    {
        protocol = ABUP_PROTOCOL_HTTP;
    }
    break;
#endif
#ifdef ABUP_MODULE_ESP07S_ENABLE
    case ABUP_MODULE_ESP07S:
    {
        protocol = ABUP_PROTOCOL_HTTP;
    }
    break;
#endif
#ifdef ABUP_MODULE_ME3630_ENABLE
    case ABUP_MODULE_ME3630:
    {
        protocol = ABUP_PROTOCOL_HTTP;
    }
    break;
#endif
#ifdef ABUP_MODULE_SIM7600_ENABLE
    case ABUP_MODULE_SIM7600:
    {
        protocol = ABUP_PROTOCOL_HTTP;
    }
    break;
#endif
    default:
    {
        protocol = ABUP_PROTOCOL_COAP;
    }
    break;
    }
    return protocol;
#endif
}

abup_int8 abup_get_try_count(void)
{
    return (abup_int8)ABUP_TRY_COUNT;
}
abup_int32 abup_get_utc_time(void)
{
    return 133;
}
abup_uint8 abup_get_default_segment_size(void)
{
    return (abup_uint8)ABUP_DEFAULT_SEGMENT_SIZE_INDEX;
}
abup_uint abup_get_data_max_len(void)
{
    return (abup_uint)ABUP_DATA_SEQ_MAX_LEN;
}
