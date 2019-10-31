/*****************************************************************************
* Copyright (c) 2018 ABUP.Co.Ltd. All rights reserved.
* File name: abup_hal_uart.c
* Description: 串口驱动
* Author: Ray Shen
* Version: v1.0
* Date: 20181101
*****************************************************************************/
#include "abup_typedef.h"
#include "abup_stdlib.h"
#include "abup_hal_uart.h"

#if defined(PKG_USING_ABUP_FOTA)
abup_char uart_debug = 1;
abup_char uart_at = 2;
#elif defined(ABUP_RTT_LOG)
abup_char uart_debug = 1;
#endif
ABUP_ALIGN(1) abup_char ATBuf[ABUP_UARTTX_MAXLEN]= {0};
ABUP_ALIGN(1) abup_char AbupATRxBuf[ABUP_UARTRX_MAXLEN]= {0};
#ifdef ABUP_USART_DEBUG_AT
ABUP_ALIGN(1) abup_uint8 AbupDebugRxBuf[ABUP_UARTRX_DEBUGLEN]= {0};
abup_uint16 AbupDebugRxBufLen = 0;
abup_char *abup_get_AbupDebugRxBuf(void)
{
    return (abup_char *)AbupDebugRxBuf;
}
abup_uint16 abup_get_AbupDebugRxBuf_len(void)
{
    return ABUP_UARTRX_DEBUGLEN;
}
#endif
abup_char *abup_get_ATBuf(void)
{
    return (abup_char *)ATBuf;
}
abup_char *abup_get_AbupATRxBuf(void)
{
    return (abup_char *)AbupATRxBuf;
}
abup_uint16 abup_get_ATBuf_len(void)
{
    return ABUP_UARTTX_MAXLEN;
}
abup_uint16 abup_get_AbupATRxBuf_len(void)
{
    return ABUP_UARTRX_MAXLEN;
}

void AbupTransmitData(ABUP_USART_TypeDef *USARTx,abup_char *data,abup_int len)
{
    if(len)
    {
#if defined(PKG_USING_ABUP_FOTA)
        if(USARTx == ABUP_USART_DEBUG)
            rt_kprintf("%s",data);
#else
#if defined(ABUP_RTT_LOG)
        if(USARTx == ABUP_USART_DEBUG)
        {
            SEGGER_RTT_printf(0,"%s",data);
            return;
        }
#endif
#if defined(__ICCSTM8__)
        abup_char *p = data;
        while((p != NULL)&&(*p != 0))
        {
            USARTx->DR = (abup_uint8)*p;
            while(!(USARTx->SR&0X80));//寄存器是否为空
            p++;
        }
#else
#ifdef USE_FULL_LL_DRIVER
        abup_char *p = data;
        /* Send abup_characters one per one, until last abup_char to be sent */
        while((p != NULL)&&(*p != 0))
        {

            /* Wait for TXE flag to be raised */
            while (!LL_USART_IsActiveFlag_TXE(USARTx))
            {
            }

            /* Write abup_character in Transmit Data register.
            	 TXE flag is cleared by writing data in TDR register */
            LL_USART_TransmitData8(USARTx, *p);
            p++;
        }


        /* Wait for TC flag to be raised for last abup_char */
        while (!LL_USART_IsActiveFlag_TC(USARTx))
        {
        }
#else
        HAL_UART_Transmit(USARTx, (abup_uint8 *)data, len, 200);
#endif
#endif
#endif
    }
}
#if defined(ABUP_BL_PRINT_MAXLEN)
abup_char AbupPrintBuf[ABUP_BL_PRINT_MAXLEN]= {0};
void abup_bl_debug_printf(const abup_char *content, ...)
{
#if defined(ABUP_BL_PRINT_MAXLEN)
    abup_int len = 0;
    va_list Args;
    abup_memset(AbupPrintBuf, 0, ABUP_BL_PRINT_MAXLEN);
    va_start(Args,content);
    len = abup_vsnprintf(AbupPrintBuf,ABUP_BL_PRINT_MAXLEN, content, Args);

    va_end(Args);
    if(len < (ABUP_BL_PRINT_MAXLEN - 2))
    {
        abup_memcpy(AbupPrintBuf + len,"\r\n",2);
        len += 2;
    }
    AbupTransmitData(ABUP_USART_DEBUG,AbupPrintBuf,len);
#endif
}
#endif
void abup_bl_main_printf(abup_char *data)
{
    AbupTransmitData(ABUP_USART_DEBUG,data,abup_strlen(data));
}

abup_int abup_at_send_cmd(const abup_char *fmt, ...)
{
    abup_int n;
    va_list args;

    va_start(args, fmt);
    n = abup_vsnprintf(ATBuf, sizeof(ATBuf), fmt, args);
    va_end(args);
    abup_at_send_data(ATBuf,n);
    return n;
}
abup_int abup_info_printf(const abup_char *fmt, ...)
{
    abup_int n;
    va_list args;

    va_start(args, fmt);
    n = abup_vsnprintf(ATBuf, sizeof(ATBuf), fmt, args);
    va_end(args);
    AbupTransmitData(ABUP_USART_DEBUG,ATBuf,n);
    return n;
}
__weak void abup_at_send_data(abup_char *data,abup_int16 len)
{
    if(len > 0)
    {
        AbupTransmitData(ABUP_USART_AT,data,len);
#ifdef ABUP_DEBUG_MODE
        AbupTransmitData(ABUP_USART_DEBUG,data,len);
#endif
    }
#ifdef ABUP_TIMERS
    AbupReStartTimer(AbupAtpTimer);
#endif
}
