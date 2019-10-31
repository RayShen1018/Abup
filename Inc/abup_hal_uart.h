/*****************************************************************************
* Copyright (c) 2018 ABUP.Co.Ltd. All rights reserved.
* File name: abup_hal_uart.h
* Description:
* Author: Ray Shen
* Version: v1.0
* Date: 20181101
*****************************************************************************/
#ifndef ABUP_HAL_UART
#define ABUP_HAL_UART
#include "abup_stdlib.h"
#ifdef ABUP_TIMERS
#include "abup_timer.h"
#endif
#include "main.h"

#ifdef ABUP_TIMERS
extern AbupTimerHandle_t AbupAtpTimer;
#endif
#if defined(__ICCSTM8__)
#define ABUP_USART_TypeDef USART_TypeDef
#ifdef STM8L05X_HD_VL
#define ABUP_USART_DEBUG USART1
#define ABUP_USART_AT USART3
#endif
#else
#define ABUP_USART_TypeDef USART_TypeDef
#endif
#if (ABUP_DEFAULT_NETWORK_PROTOCOL == ABUP_PROTOCOL_HTTP)
#define ABUP_UARTRX_MAXLEN                   ABUP_HTTP_MAX_LEN
#define ABUP_UARTTX_MAXLEN                   (512)
#elif (ABUP_DEFAULT_NETWORK_PROTOCOL == ABUP_PROTOCOL_COAP_HTTP)
#if ((ABUP_COAP_MAX_LEN*2 + 100) > ABUP_HTTP_MAX_LEN)
#define ABUP_UARTRX_MAXLEN		(ABUP_COAP_MAX_LEN*2 + 100)
#else
#define ABUP_UARTRX_MAXLEN                   ABUP_HTTP_MAX_LEN
#endif
#if (ABUP_UARTRX_MAXLEN > 512)
#define ABUP_UARTTX_MAXLEN		ABUP_UARTRX_MAXLEN
#else
#define ABUP_UARTTX_MAXLEN                   (400)
#endif
#else
#define ABUP_UARTRX_MAXLEN		(ABUP_COAP_MAX_LEN*2 + 100)
#define ABUP_UARTTX_MAXLEN                   ABUP_UARTRX_MAXLEN
#endif

#ifndef LOG_DEBUG
#define LOG_DEBUG NULL
#endif
#if defined(ABUP_BOOTLOADER)&&defined(ABUP_DEBUG_MODE)

#ifndef ADUPS_FOTA_ENABLE_DIFF_DEBUG
#define ADUPS_FOTA_ENABLE_DIFF_DEBUG
#endif
#ifndef ADUPS_FOTA_ENABLE_DIFF_ERROR
#define ADUPS_FOTA_ENABLE_DIFF_ERROR
#endif

#ifndef ABUP_BL_PRINT_MAXLEN
#define ABUP_BL_PRINT_MAXLEN                   512
#endif

#endif
extern void abup_bl_main_printf(abup_char *data);
#ifdef ABUP_BL_PRINT_MAXLEN
extern void abup_bl_debug_printf(const abup_char *content, ...);

#define abup_full_debug_printf abup_bl_debug_printf
#define abup_bl_full_printf abup_bl_debug_printf
#define abup_bl_debug_printf abup_bl_debug_printf
#define adups_bl_debug_print(TAG,CONTENT,...)  \
        do{ \
					abup_bl_debug_printf(CONTENT,##__VA_ARGS__); \
        }while(0);
#define peer_logout      abup_bl_full_printf
#else
#define abup_full_debug_printf(...)
#define abup_bl_full_printf(...)
#define abup_bl_debug_printf(...)
#define adups_bl_debug_print(...)
#define peer_logout(...)
#endif
extern abup_char ATBuf[ABUP_UARTTX_MAXLEN];
#if defined(PKG_USING_ABUP_FOTA)
#define ABUP_USART_DEBUG (ABUP_USART_TypeDef*)&uart_debug
#define ABUP_USART_AT (ABUP_USART_TypeDef*)&uart_at
#else
#if defined(STM32L0_DISCO)||defined(STM32L0_NUCLEO)
#include "stm32l0xx_hal.h"
#if defined(STM32L0_NUCLEO)
#ifdef USE_FULL_LL_DRIVER
#define ABUP_USART_DEBUG USART2
#define ABUP_USART_AT USART1
#else
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
#define ABUP_USART_DEBUG &huart2
#define ABUP_USART_AT &huart1
#endif
#else
#define ABUP_USART_DEBUG USART1
#define ABUP_USART_AT USART1
#endif
#endif
#if defined(STM32F030CC)
#include "stm32f0xx_hal.h"
#define ABUP_USART_DEBUG USART2
#define ABUP_USART_AT USART5
#endif
#if defined(STM32F103RC)
#include "stm32f103xe.h"
#define ABUP_USART_DEBUG USART1
#define ABUP_USART_AT USART2
#endif
#if defined(STM32L4_NUCLEO)
#include "stm32l4xx_hal.h"
#define ABUP_USART_DEBUG USART2
#define ABUP_USART_AT USART1
#endif
#if defined(STM32L452RCT6)
#include "stm32l4xx_hal.h"
#define ABUP_USART_DEBUG UART4
#define ABUP_USART_AT USART1
#endif
#if defined(STM32G0_NUCLEO)
#include "stm32g0xx_hal.h"
#define ABUP_USART_DEBUG USART2
#define ABUP_USART_AT USART1
#endif
#if defined(STM32F4_DISCO)
#include "stm32f4xx_hal.h"
#define ABUP_USART_DEBUG USART3
#define ABUP_USART_AT USART6
#endif
#if defined(STM32F7_NUCLEO)
#include "stm32f767xx.h"
#define ABUP_USART_DEBUG USART2
#define ABUP_USART_AT USART1
#endif
#endif
#if defined(ABUP_RTT_LOG)
#include "SEGGER_RTT.h"
#if defined(ABUP_USART_DEBUG)
#undef ABUP_USART_DEBUG
#endif
#define ABUP_USART_DEBUG (ABUP_USART_TypeDef*)&uart_debug
#else 
#define ABUP_USART_DEBUG_AT
#endif
#if !defined(ABUP_USART_DEBUG_BAUDRATE)
#define ABUP_USART_DEBUG_BAUDRATE 115200
#endif

extern abup_char AbupATRxBuf[ABUP_UARTRX_MAXLEN];
#ifdef ABUP_USART_DEBUG_AT
#define ABUP_UARTRX_DEBUGLEN                   255
extern abup_uint8 AbupDebugRxBuf[ABUP_UARTRX_DEBUGLEN];
extern abup_uint16 AbupDebugRxBufLen;
extern abup_char *abup_get_AbupDebugRxBuf(void);
extern abup_uint16 abup_get_AbupDebugRxBuf_len(void);
#endif
#ifdef ABUP_DEBUG_MODE
#define abup_debug_printf abup_info_printf
#else
#define abup_debug_printf(...)
#endif
#define ABUP_TAG  "Abup"
#define ABUP_INFO(TAG,CONTENT,...)  \
        do{ \
          abup_info_printf("[%s] "CONTENT"\n",TAG, ##__VA_ARGS__); \
        }while(0)
extern abup_int abup_info_printf(const abup_char *fmt, ...);
extern abup_int abup_at_send_cmd(const abup_char *fmt, ...);
extern void abup_at_send_data(abup_char *data,abup_int16 len);
extern void AbupTransmitData(ABUP_USART_TypeDef *USARTx,abup_char *data,abup_int len);
extern abup_char *abup_get_ATBuf(void);
extern abup_char *abup_get_AbupATRxBuf(void);
extern abup_uint16 abup_get_ATBuf_len(void);
extern abup_uint16 abup_get_AbupATRxBuf_len(void);
#endif


