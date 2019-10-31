#ifndef __ABUP_REQUEST__
#define __ABUP_REQUEST__

#include "abup_typedef.h"
#include "abup_hal_uart.h"


typedef struct  
{  
	abup_uint8 type[32];
	abup_uint16 content_len;
	abup_uint8* content;
}ABUP_ALIGN(1) abup_http_parameter; 
extern abup_int abup_http_content_len;


extern void abup_fota_make_json_request(abup_int8 state,abup_char *ptr,abup_uint len);


extern abup_int8 *abup_make_http_data(abup_uint8 state);
extern abup_bool abup_parse_http_data(abup_char* data,abup_http_parameter* http_parameter,abup_uint16 count);
extern abup_char* abup_is_http_data(abup_char* data,abup_uint16 len);
extern abup_uint abup_http_callback(abup_uint8 state,abup_char *data, abup_uint len);


void abup_http_get_new_version(void);

abup_char* abup_get_http_server_host(void);
void abup_http_report_result(void);
extern abup_char * abup_get_signptr(abup_char *buf,abup_int *len,abup_char* mid,abup_char *productId, abup_char* productSecret,abup_uint32 utc_time);
extern abup_uint8 *abup_get_common_data(void);
extern abup_uint abup_get_common_data_len(void);
extern abup_bool abup_get_domain(abup_char *server,abup_char* domain,abup_int16 domain_len,abup_int* port);
#endif

