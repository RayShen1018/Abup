/*****************************************************************************
* Copyright (c) 2018 ABUP.Co.Ltd. All rights reserved.
* File name: abup_os.h
* Description: 
* Author: Ray Shen
* Version: v1.0
* Date: 20181101
*****************************************************************************/
#ifndef _ABUP_OS_H_
#define _ABUP_OS_H_

#if defined(ABUP_RTTHREAD)
#include "rtconfig.h"
#else
//消息队列，不可关闭
#define ABUP_QUEUE_MSG
//定时器，不可关闭
#define ABUP_TIMERS
#endif

//开启debug，否则user 
#define ABUP_APP_DEBUG 1

#endif

