/*****************************************************************************
* Copyright (c) 2018 ABUP.Co.Ltd. All rights reserved.
* File name: abup_stdlib.h
* Description: 
* Author: Ray Shen
* Version: v1.0
* Date: 20181101
*****************************************************************************/
#ifndef _ABUP_STDLIB_H_
#define _ABUP_STDLIB_H_
#include "abup_typedef.h"
#include "stdarg.h"

#undef NULL
#define NULL 0                   /* see <stddef.h> */
#define __ISMASK(x) (_ctype[(abup_int)(abup_uint8)(x)])

#define ISALNUM(c)	((__ISMASK(c)&(ABUP_U|ABUP_L|ABUP_D)) != 0)
#define ISALPHA(c)	((__ISMASK(c)&(ABUP_U|ABUP_L)) != 0)
#define ISCNTRL(c)	((__ISMASK(c)&(ABUP_C)) != 0)
#define ISGRAPH(c)	((__ISMASK(c)&(ABUP_P|ABUP_U|ABUP_L|ABUP_D)) != 0)
#define ISLOWER(c)	((__ISMASK(c)&(ABUP_L)) != 0)
#define ISPRINT(c)	((__ISMASK(c)&(ABUP_P|ABUP_U|ABUP_L|ABUP_D|ABUP_SP)) != 0)
#define ISPUNCT(c)	((__ISMASK(c)&(ABUP_P)) != 0)
#define ISSPACE(c)	((__ISMASK(c)&(ABUP_S)) != 0)
#define ISUPPER(c)	((__ISMASK(c)&(ABUP_U)) != 0)
#define ISXDIGIT(c)	((__ISMASK(c)&(ABUP_D|ABUP_X)) != 0)
#define ISDIGIT(c)  ((unsigned)((c) - '0') < 10)
#define ISASCII(c) (((abup_uint8)(c))<=0x7f)
#define TOASCII(c) (((abup_uint8)(c))&0x7f)

#define ZEROPAD     (1 << 0)    /* pad with zero */
#define SIGN        (1 << 1)    /* unsigned/signed long */
#define PLUS        (1 << 2)    /* show plus */
#define SPACE       (1 << 3)    /* space if plus */
#define LEFT        (1 << 4)    /* left justified */
#define SPECIAL     (1 << 5)    /* 0x */
#define LARGE       (1 << 6)    /* use 'ABCDEF' instead of 'abcdef' */

#define INT_MAX		((abup_int)(~0U>>1))
#define INT_MIN		(-INT_MAX - 1)
#define UINT_MAX	(~0U)
#define LONG_MAX	((abup_int)(~0UL>>1))
#define LONG_MIN	(-LONG_MAX - 1)
#define ULONG_MAX	(~0UL)

#define ABUP_U	0x01	/* upper */
#define ABUP_L	0x02	/* lower */
#define ABUP_D	0x04	/* digit */
#define ABUP_C	0x08	/* cntrl */
#define ABUP_P	0x10	/* punct */
#define ABUP_S	0x20	/* white space (space/lf/tab) */
#define ABUP_X	0x40	/* hex digit */
#define ABUP_SP	0x80	/* hard space (0x20) */
extern abup_int abup_vsnprintf(abup_char       *buf,
                        abup_int   size,
                        const abup_char *fmt,
                        va_list     args);
extern abup_int abup_snprintf(abup_char * s ,abup_int n,const abup_char * content, ...);

extern void *abup_memmove(void *dest, const void *src, abup_int n);

abup_extern void *abup_memcpy(void * dest, const void * src, abup_int count);
abup_extern void *abup_memset(void *s, abup_int c, abup_int count);
abup_extern abup_int abup_memcmp(const void *cs, const void *ct, abup_int count);
abup_extern abup_char *abup_strcat(abup_char * dest, const abup_char * src);

abup_extern abup_int abup_strlen(const abup_char *s);
abup_extern abup_int abup_sscanf(const abup_char * buf, const abup_char * fmt, ...);
abup_extern abup_int abup_strncmp(const abup_char *cs, const abup_char *ct, abup_int count);
abup_extern abup_int abup_strcmp(const abup_char * string1, const abup_char * string2 );
abup_extern abup_char *abup_strcpy(abup_char * strDestination, const abup_char * strSource);
abup_extern abup_char *abup_strncpy(abup_char *dst, const abup_char *src, abup_int n);
abup_extern abup_char *abup_strstr(const abup_char * str, const abup_char * sub);
abup_extern abup_char *abup_strchr(const abup_char *s1, abup_int i);

abup_extern abup_int abup_atoi(const abup_char *str);
abup_extern abup_char *abup_itoa(abup_int num,abup_char* str,abup_int strlen,abup_int base,abup_bool unsignedint);

abup_extern void abup_memfree(void *p);
abup_extern abup_uint abup_str2uint32(const abup_char * str);
abup_extern void abup_byte2hexstr(const abup_uint8* source, abup_char* dest, abup_int len);
abup_extern void abup_hexstr2byte(abup_char* dest, const abup_uint8* source, abup_int sourceLen);
abup_extern abup_int abup_c2i(abup_char ch);
abup_extern abup_int abup_hex2dec(abup_char *hex);
abup_extern abup_bool abup_str_del_all(abup_char* str,abup_char *sub,abup_int* len,abup_int sublen);
abup_extern abup_bool abup_str_is_same_char(abup_char* str,abup_int len,abup_char ch);
abup_extern abup_bool AbupIsBigEndian(void);
#endif
