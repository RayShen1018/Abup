/*
* Copyright (c) 2006-2018, RT-Thread Development Team
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date             Author      Notes
*
*/
#include <rtthread.h>
#include <sys/socket.h> /* 使用BSD socket，需要包含socket.h头文件 */
#include <netdb.h>
#include <string.h>
#include <finsh.h>
#include "at_socket.h"
#include "abup_hal.h"

extern const abup_uint8  testdata1[];
extern const abup_uint8  testdata2[];
typedef abup_int8 (*abup_read_cb)(abup_int8* State,abup_int sock,struct sockaddr_in *addr,abup_char *data,abup_int len);
abup_char abup_rtt_data[ABUP_UARTRX_MAXLEN];
abup_int8 AbupState = STATE_INIT;
void AbupUDPCreateDownload(abup_int8 state,abup_read_cb read_cb);
void abup_closesocket(abup_int sock)
{
    if(sock >= 0)
    {
        closesocket(sock);
        abup_debug_printf("%s() %d\r\n",__func__,__LINE__);
        /* 线程休眠一段时间 */
        rt_thread_delay(500);
    }
}
void abup_set_mid(abup_char* mid)
{
    abup_char mid_str[ABUP_MID_MAXLEN];
    abup_uint16 mid_len = ABUP_MID_MAXLEN;
    abup_memset(mid_str,0,mid_len);
    abup_snprintf(mid_str,mid_len,"%02x:%02x:%02x:%02x:%02x:%02x",*(mid),*(mid + 1),*(mid + 2),*(mid + 3),*(mid + 4),*(mid + 5));
    abup_hal_update_device((abup_uint8*)mid_str,mid_len,NULL,0,NULL,0);
}

abup_uint abup_rtt_callback(abup_uint8 state,abup_char *buf, abup_uint len)
{
    abup_uint pl = (abup_uint)-1;
    abup_char *data = NULL;
#if (ABUP_DEFAULT_NETWORK_PROTOCOL == ABUP_PROTOCOL_HTTP)
    data = (abup_char*)abup_is_http_data((abup_char*)buf,len);
    if(data)
    {
        pl = abup_http_callback(state,data,abup_http_content_len);
    }
#else
    if((len == 44)||(len == 66))
    {
        for(abup_uint i = 0; i <len/22; i++)
        {
            data = abup_parse_message((abup_char *)buf+i*22,22);
            if(data)
            {
                pl = abup_coap_callback(state,data,22);
                if(pl != (abup_uint)-1)
                    break;
            }
        }
    }
    else
    {
        data = abup_parse_message((abup_char *)buf,len);
        if(data)
        {
            pl = abup_coap_callback(state,data,len);
        }
    }
#endif
    switch(pl)
    {
    case 1000:
        ABUP_INFO(ABUP_TAG,"success\r\n");
        break;
    case 1001:
        ABUP_INFO(ABUP_TAG,"The productId is invalid\r\n");
        break;
    case 1002:
        ABUP_INFO(ABUP_TAG,"The project does not exist\r\n");
        break;
    case 1003:
        ABUP_INFO(ABUP_TAG,"The param is invalid\r\n");
        break;
    case 1004:
        ABUP_INFO(ABUP_TAG,"Parameter missing\r\n");
        break;
    case 1005:
        ABUP_INFO(ABUP_TAG,"The system is error\r\n");
        break;
    case 1006:
        ABUP_INFO(ABUP_TAG,"JSON parsing exception\r\n");
        break;
    case 1007:
        ABUP_INFO(ABUP_TAG,"Parameters do not conform to the rules\r\n");
        break;
    case 1008:
        ABUP_INFO(ABUP_TAG,"MID Length Error\r\n");
        break;
    case 1009:
        ABUP_INFO(ABUP_TAG,"AES Encryption Error\r\n");
        break;
    case 1010:
        ABUP_INFO(ABUP_TAG,"To the maximum number of visits\r\n");
        break;
    case 2001:
        ABUP_INFO(ABUP_TAG,"The sign is error\r\n");
        break;
    case 2101:
        ABUP_INFO(ABUP_TAG,"No new updates were found\r\n");
        break;
    case 2103:
        ABUP_INFO(ABUP_TAG,"Equipment not registered\r\n");
        break;
    case 2104:
        ABUP_INFO(ABUP_TAG,"The project has no project team\r\n");
        break;
    case 2201:
        ABUP_INFO(ABUP_TAG,"The download status is not legal\r\n");
        break;
    case 2202:
    case 2301:
        ABUP_INFO(ABUP_TAG,"The deltaID is not exist\r\n");
        break;
    case 2302:
        ABUP_INFO(ABUP_TAG,"The upgrade status is not legal\r\n");
        break;
    case (abup_uint)-1:
        break;
    default:
        ABUP_INFO(ABUP_TAG,"error code\r\n",pl);
        break;
    }
    return pl;
}
abup_bool abup_state_send(abup_int8* State,abup_int sock,struct sockaddr_in *addr)
{
    abup_uint8 *data = NULL;
#if (ABUP_DEFAULT_NETWORK_PROTOCOL == ABUP_PROTOCOL_HTTP)
    data = (abup_uint8 *)abup_make_http_data(*State);
    if(data != NULL)
    {
        send(sock,data,abup_strlen((abup_char *)data),0);
        abup_debug_printf("data:%s\r\n",data);
        return abup_true;
    }
#else
    abup_int data_len = abup_get_ATBuf_len();
    data = (abup_uint8 *)abup_coap_make_data(*State,abup_md5_calc_result,abup_get_ATBuf(),&data_len);
    if(data != NULL)
    {
        sendto(sock, data, data_len, 0,
               (struct sockaddr *)addr, sizeof(struct sockaddr));
        abup_debug_printf("\nSent len = %d data = ",data_len);
        for(abup_int i = 0; i < data_len; i++)
        {
            abup_debug_printf("%02X", data[i]);
        }
        abup_debug_printf("\n");
        return abup_true;
    }
#endif
    return abup_false;
}
abup_int8 abup_state_continue(abup_int8* State,abup_int8 end,abup_int sock,struct sockaddr_in *addr,abup_char *data,abup_int len,abup_uint result)
{
    if(*State == end)
    {
        if(result == 1000)
        {
            return 0;
        }
    }
    else
    {
        if(data != NULL)
        {
            memset(data,0,len);
        }
        else
        {
            result = 1000;
        }
        if(abup_get_conn_try_count() < abup_get_try_count())
        {
            if((*State > STATE_INIT)&&(*State < STATE_END))
            {
#if (ABUP_DEFAULT_NETWORK_PROTOCOL == ABUP_PROTOCOL_HTTP)
                if((result == 1000)||(result == (abup_uint)-1))
#else
                if((result == 1000)||((result == (abup_uint)-1)&&(*State != STATE_RG)))
#endif
                {
                    if(abup_state_send(State,sock,addr))
                    {
                        return 1;
                    }
                }
                else if(result != (abup_uint)-1)
                {
                    return -2;
                }
            }
        }
    }
    return -1;
}
void abup_set_try_count(abup_int8* State)
{
    static abup_int8	state = 0;
    if(state != *State)
    {
        state = *State;
        abup_set_conn_try_count(0);
    }
    else
    {
        abup_set_conn_try_count(abup_get_conn_try_count() + 1);
    }
}
abup_int8 abup_dl_callback(abup_int8* State,abup_int sock,struct sockaddr_in *addr,abup_char *data,abup_int len)
{
    if(*State == STATE_DL)
    {
        abup_uint abup_cb_result = (abup_uint)-1;
        abup_set_try_count(State);
        if(data != NULL)
        {
            abup_cb_result = abup_rtt_callback(*State,data,len);
            if(abup_cb_result == 1000)
            {
                rt_kprintf("[Abup] %d/%d\n", download_index,download_index_max);
                abup_update_struct *abup_update = abup_hal_get_update();
                if(abup_get_download_index()== abup_get_download_index_max())
                {
                    abup_md5_calc_result = abup_MD5Calc_result();
                    if(abup_md5_calc_result == 1)
                    {
                        abup_update->update_result = 100;
                    }
                    else
                    {
                        abup_update->index = 0;
                        abup_update->index_max = 0;
                        abup_update->delta_id = 0;
                        abup_update->update_result = 0;
                    }
                    abup_hal_set_update(abup_update);
                    *State = STATE_RD;
                }
                else
                {
                    abup_set_conn_try_count(0);
                    download_index++;
                    abup_update->index = download_index;
                    abup_hal_set_update(abup_update);
                }

            }
        }
        if(*State == STATE_DL)
            return abup_state_continue(State,STATE_RD,sock,addr,data,len,abup_cb_result);
    }
    return abup_false;
}

abup_int8 abup_cv_callback(abup_int8* State,abup_int sock,struct sockaddr_in *addr,abup_char *data,abup_int len)
{
    abup_uint abup_cb_result = (abup_uint)-1;
    abup_set_try_count(State);
    if(*State == STATE_INIT)
    {
#if (ABUP_DEFAULT_NETWORK_PROTOCOL == ABUP_PROTOCOL_HTTP)
        abup_update_struct* abup_update = abup_hal_get_update();
        if((abup_strlen((abup_char*)abup_update->deviceId) == 0)
                ||(abup_strlen((abup_char*)abup_update->deviceSecret) == 0))
        {
            *State = STATE_RG;
        }
        else
        {
            *State = STATE_CV;
        }
#else
        *State = STATE_RG;
#endif
    }
    else if(*State == STATE_END)
    {
    }
    else
    {
        if(data != NULL)
        {
            abup_cb_result = abup_rtt_callback(*State,data,len);
            if(abup_cb_result == 1000)
            {
                switch(*State)
                {
                case STATE_RG:
                    *State = STATE_CV;
                    break;
                case STATE_CV:
                {
                    abup_bool new_download = abup_false;
                    abup_uint16 block_size = abup_get_data_max_len();
                    abup_update_struct *abup_update = abup_hal_get_update();
                    download_uri_struct* abup_app_uri = abup_get_download_atp_uri();
                    if(abup_app_uri->download_delta_size > abup_flash_get_delta_size())
                    {
                        abup_action_result = ABUP_RESULT_CV_FAIL;
                        ABUP_INFO(ABUP_TAG,"download_delta_size = %d flash delta_size = %d!\r\n",abup_app_uri->download_delta_size,abup_flash_get_delta_size());
                        *State = STATE_END;
                        break;
                    }
                    download_index_max = abup_app_uri->download_delta_size%block_size?(abup_app_uri->download_delta_size/block_size + 1):(abup_app_uri->download_delta_size/block_size);

                    if((abup_update->delta_id == abup_app_uri->download_delta_id)
                            &&(abup_update->delta_id != 0)
                            &&(abup_update->index_max != 0)
                            &&(abup_update->index_max == download_index_max))
                    {
                        if(abup_update->index < download_index_max)
                        {
                            download_index = abup_update->index + 1;
                        }
                        else
                        {
                            if((abup_update->index == download_index_max)&&(abup_update->delta_id == abup_app_uri->download_delta_id))
                            {
                                abup_md5_calc_result = abup_MD5Calc_result();
                                if(abup_md5_calc_result == 1)
                                {
                                    abup_action_result = ABUP_RESULT_CV_SUCC;
                                    abup_update->update_result = 100;
                                    abup_hal_set_update(abup_update);
                                    ABUP_INFO(ABUP_TAG,"Download end!\r\n");
                                    *State = STATE_END;
                                    break;
                                }
                                new_download = abup_true;
                            }
                            else
                            {
                                new_download = abup_true;
                            }
                        }
                    }
                    else
                    {
                        new_download = abup_true;
                    }
                    if(new_download)
                    {
                        abup_md5_calc_result = 0;
                        download_index = 1;
                        abup_update->delta_id = abup_app_uri->download_delta_id;
                        abup_update->update_result = 0;
                        abup_update->index_max = download_index_max;
                        abup_hal_set_update(abup_update);
                    }
                    if (ABUPCheckKey())
                    {
                        *State = STATE_DL;
                    }
                    else
                    {
                        *State = STATE_KY;
                    }
                }
                break;
                case STATE_KY:
                {
                    *State = STATE_DL;
                }
                break;
                case STATE_RD:
                    *State = STATE_END;
                    break;
                case STATE_RU:
                {
                    abup_update_struct *abup_update = abup_hal_get_update();
                    abup_update->index = 0;
                    abup_update->index_max = 0;
                    abup_update->delta_id = 0;
                    abup_update->update_result = 0;
                    abup_hal_set_update(abup_update);
                    *State = STATE_END;
                }
                break;
                default:
                    break;
                }

            }
        }
    }
    return abup_state_continue(State,STATE_DL,sock,addr,data,len,abup_cb_result);
}
abup_int8 abup_ru_callback(abup_int8* State,abup_int sock,struct sockaddr_in *addr,abup_char *data,abup_int len)
{
    abup_uint abup_cb_result = (abup_uint)-1;
    abup_set_try_count(State);
    if(*State == STATE_INIT)
    {
        *State = STATE_RG;
    }
    else if(*State == STATE_END)
    {
    }
    else
    {
        if(data != NULL)
        {
            abup_cb_result = abup_rtt_callback(*State,data,len);
            if(abup_cb_result == 1000)
            {
                switch(*State)
                {
                case STATE_RG:
                    *State = STATE_RU;
                    break;
                case STATE_RU:
                {
                    abup_update_struct *abup_update = abup_hal_get_update();
                    abup_update->index = 0;
                    abup_update->index_max = 0;
                    abup_update->delta_id = 0;
                    abup_update->update_result = 0;
                    abup_hal_set_update(abup_update);
                    *State = STATE_END;
                }
                break;
                default:
                    break;
                }

            }
        }
    }
    return abup_state_continue(State,STATE_END,sock,addr,data,len,abup_cb_result);
}
abup_int8 abup_rd_callback(abup_int8* State,abup_int sock,struct sockaddr_in *addr,abup_char *data,abup_int len)
{
    abup_uint abup_cb_result = (abup_uint)-1;
    abup_set_try_count(State);
    if(*State == STATE_INIT)
    {
        *State = STATE_RG;
    }
    else if(*State == STATE_END)
    {
    }
    else
    {
        if(data != NULL)
        {
            abup_cb_result = abup_rtt_callback(*State,data,len);
            if(abup_cb_result == 1000)
            {
                switch(*State)
                {
                case STATE_RG:
                    *State = STATE_RD;
                    break;
                case STATE_RD:
                {
                    *State = STATE_END;
                }
                break;
                default:
                    break;
                }

            }
        }
    }
    return abup_state_continue(State,STATE_END,sock,addr,data,len,abup_cb_result);
}
abup_char *AbupGetURLPort(abup_int8 state,abup_int *port)
{
    abup_char *url = NULL;
    if(state == STATE_DL)
    {
        download_uri_struct* uri = abup_get_download_atp_uri();
        url = uri->download_host;
        *port = uri->download_port;
    }
    else
    {
        abup_uint8* domain = (abup_uint8*)abup_get_buf();
        if(abup_get_domain((abup_char*)abup_get_server_host(),(abup_char *)domain,abup_get_buf_len(),(abup_int*)abup_get_host_port()))
        {
            url = abup_get_buf();
            *port = *(abup_int*)abup_get_host_port();
        }
    }
    return url;
}
abup_bool AbupCreateSOC(abup_int8 state,abup_read_cb read_cb)
{
    abup_int sock;
    abup_int port;
    abup_int8 cb;
    struct hostent *host;
    struct sockaddr_in server_addr;
    const abup_char *url;
    abup_int bytes_received = 0;
    abup_bool result = abup_false;
#if (ABUP_DEFAULT_NETWORK_PROTOCOL == ABUP_PROTOCOL_HTTP)
#else
    short count = 0;
    socklen_t addr_len = 0;
    struct sockaddr_in client_addr;
#endif
    if(read_cb == NULL)
    {
        return result;
    }
    url = AbupGetURLPort(state,&port);

    /* 通过函数入口参数url获得host地址（如果是域名，会做域名解析） */
    host = gethostbyname(url);

    memset(abup_rtt_data,0,sizeof(abup_rtt_data));

#if (ABUP_DEFAULT_NETWORK_PROTOCOL == ABUP_PROTOCOL_HTTP)
    /* 创建一个socket，类型是SOCKET_STREAM，TCP类型 */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        /* 创建socket失败 */
        abup_closesocket(-1);
        return result;
    }
#else
    /* 创建一个socket，类型是SOCK_DGRAM，UDP类型 */
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        ABUP_INFO(ABUP_TAG,"Socket error\n");
        return result;
    }
#endif

    /* 初始化预连接的服务端地址 */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));
#if (ABUP_DEFAULT_NETWORK_PROTOCOL == ABUP_PROTOCOL_HTTP)
    /* 连接到服务端 */
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        /* 连接失败 */

        abup_closesocket(sock);
        AbupState = STATE_END;
        return result;
    }
    else
    {
        /* 连接成功 */
        ABUP_INFO(ABUP_TAG,"Connect successful\n");
    }
#else
#endif
    AbupState = state;
    if(read_cb(&AbupState,sock,&server_addr,NULL,0) == -1)
    {
        /* 关闭这个socket */
        abup_closesocket(sock);
        return result;
    }

    struct at_socket *socket = RT_NULL;
    socket = at_get_socket(sock);
    if (socket == RT_NULL)
    {
        return result;
    }
    else
    {
        socket->recv_timeout = abup_get_try_timer();
    }
    while (1)
    {
        if(AbupState == STATE_END)
        {
            /* 关闭这个socket */
            abup_closesocket(sock);
            break;
        }
        /* 从sock中收取最大ABUP_UARTRX_MAXLEN - 1字节数据 */
#if (ABUP_DEFAULT_NETWORK_PROTOCOL == ABUP_PROTOCOL_HTTP)
        bytes_received = recv(sock, abup_rtt_data, ABUP_UARTRX_MAXLEN - 1, 0);

#else
        bytes_received = recvfrom(sock, abup_rtt_data, ABUP_UARTRX_MAXLEN - 1, 0,
                                  (struct sockaddr *)&client_addr, &addr_len);
#endif
        /* 有接收到数据，把末端清零 */
        if(bytes_received > 0)
        {
            abup_rtt_data[bytes_received] = '\0';
            /* 在控制终端显示收到的数据 */
#if (ABUP_DEFAULT_NETWORK_PROTOCOL == ABUP_PROTOCOL_HTTP)
            abup_debug_printf("Received len = %d data = %s\n",bytes_received,abup_rtt_data);
#else
            abup_debug_printf("\nReceived len = %d data = ",bytes_received);
            for(abup_int i = 0; i < bytes_received; i++)
            {
                abup_debug_printf("%02X", abup_rtt_data[i]);
            }
            abup_debug_printf("\n");
#endif
            cb = read_cb(&AbupState,sock,&server_addr,abup_rtt_data,bytes_received);
            if(cb < 0)
            {
#if (ABUP_DEFAULT_NETWORK_PROTOCOL == ABUP_PROTOCOL_HTTP)
                /* 关闭这个socket */
                abup_closesocket(sock);
                break;
#else
                if(cb == -2)
                {
                    /* 关闭这个socket */
                    abup_closesocket(sock);
                    break;
                }
                if(count < abup_get_try_count())
                {
                    ABUP_INFO(ABUP_TAG,"count = %d\n",count);
                    count++;
                }
                else
                {
                    if(read_cb(&AbupState,sock,&server_addr,NULL,0) == -1)
                    {
                        /* 关闭这个socket */
                        abup_closesocket(sock);
                        return result;
                    }
                    else
                    {
                        count = 0;
                    }
                }
#endif
            }
            else if(cb > 0)
            {
#if (ABUP_DEFAULT_NETWORK_PROTOCOL == ABUP_PROTOCOL_HTTP)
#else
                count = 0;
#endif
            }
            else
            {
                result = abup_true;
                /* 关闭这个socket */
                abup_closesocket(sock);
                break;
            }

        }
        else
        {
            ABUP_INFO(ABUP_TAG,"Received NULL\n");
            /* 关闭这个socket */
            abup_closesocket(sock);
            break;
        }
    }
    return result;
}
void AbupProgress(abup_int8 state)
{
    if(STATE_RU == state)
    {
        if(AbupCreateSOC(STATE_INIT,abup_ru_callback))
        {
            ABUP_INFO(ABUP_TAG,"Report successful upgrade results!\r\n");
        }
    }
    else
    {
        if(AbupCreateSOC(STATE_INIT,abup_cv_callback))
        {
            abup_md5_calc_result = 0;
            if(AbupCreateSOC(STATE_DL,abup_dl_callback))
            {
                if(AbupCreateSOC(STATE_RD,abup_rd_callback))
                {
                    if(abup_md5_calc_result == 1)
                    {
                        ABUP_INFO(ABUP_TAG,"Check new version success!\r\n");
                    }
                }
            }
        }
    }
    return;
}
void abupcv(void)
{
    AbupProgress(STATE_INIT);
}

MSH_CMD_EXPORT(abupcv, a tcp client sample);

abup_int abup_init_update_result(void)
{
    ABUP_INFO(ABUP_TAG,"###############################\r\n");
    ABUP_INFO(ABUP_TAG,"   CURRENT APP VERSION: V%s\r\n",abup_get_firmware_version());
    ABUP_INFO(ABUP_TAG,"###############################\r\n");
//    ABUP_INFO(ABUP_TAG,"abup_task_buf:0x%x\r\n",testdata1[1]);
//    ABUP_INFO(ABUP_TAG,"abup_task_buf:0x%x\r\n",testdata2[1]);

    abup_update_struct* abup_update = abup_hal_get_update();
    if((abup_update->update_result == ABUP_UPDATE_SUCCESS)
			||(abup_update->update_result == ABUP_UPDATE_AUTH_FAIL)
			||(abup_update->update_result == ABUP_UPDATE_FAIL))
    {   //检测是否需要上报升级结果
        AbupProgress(STATE_RU);
    }
    else
    {
        ABUP_INFO(ABUP_TAG,"No upgrade results!\r\n");
    }
    return RT_EOK;
}
