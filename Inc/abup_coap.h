/*****************************************************************************
* Copyright (c) 2018 ABUP.Co.Ltd. All rights reserved.
* File name: abup_coap.h
* Description: abup_coap.c头文件
* Author: Ray Shen
* Version: v1.0
* Date: 20181101
*****************************************************************************/
#ifndef __ABUP_COAP__
#define __ABUP_COAP__
#include "abup_hal_uart.h"
#include "abup_stdlib.h"
#include "abup_hal.h"


#define PRINTF(...)

#define COAP_MULTI_OPTION_LEN                 4
#define COAP_TMP_LEN                 50

typedef abup_uint8 coap_status_t;

//coap error code 
#define COAP_NO_ERROR                   (abup_uint8)0x00
#define COAP_IGNORE                     (abup_uint8)0x01

#define COAP_201_CREATED                (abup_uint8)0x41
#define COAP_202_DELETED                (abup_uint8)0x42
#define COAP_204_CHANGED                (abup_uint8)0x44
#define COAP_205_CONTENT                (abup_uint8)0x45
#define COAP_206_CONFORM                (abup_uint8)0x46

#define COAP_231_CONTINUE               (abup_uint8)0x5F
#define COAP_400_BAD_REQUEST            (abup_uint8)0x80
#define COAP_401_UNAUTHORIZED           (abup_uint8)0x81
#define COAP_402_BAD_OPTION             (abup_uint8)0x82
#define COAP_404_NOT_FOUND              (abup_uint8)0x84
#define COAP_405_METHOD_NOT_ALLOWED     (abup_uint8)0x85
#define COAP_406_NOT_ACCEPTABLE         (abup_uint8)0x86
#define COAP_408_REQ_ENTITY_INCOMPLETE  (abup_uint8)0x88
#define COAP_412_PRECONDITION_FAILED    (abup_uint8)0x8C
#define COAP_413_ENTITY_TOO_LARGE       (abup_uint8)0x8F
#define COAP_500_INTERNAL_SERVER_ERROR  (abup_uint8)0xA0
#define COAP_501_NOT_IMPLEMENTED        (abup_uint8)0xA1
#define COAP_503_SERVICE_UNAVAILABLE    (abup_uint8)0xA3
#define COAP_505_PROXYING_NOT_SUPPORTED (abup_uint8)0xA5
#define COAP_UNKOWN_ERROR               (abup_uint8)0xC0


#define STR_COAP_CODE(M)                                \
    ((M) == COAP_201_CREATED ? "COAP_201" :      \
    ((M) == COAP_202_DELETED ? "COAP_202" :      \
    ((M) == COAP_204_CHANGED ? "COAP_204" :  \
    ((M) == COAP_205_CONTENT ? "COAP_205" :        \
    ((M) == COAP_206_CONFORM ? "COAP_206" : \
    ((M) == COAP_231_CONTINUE ? "COAP_231" :      \
    ((M) == COAP_400_BAD_REQUEST ? "COAP_400" :      \
    ((M) == COAP_401_UNAUTHORIZED ? "COAP_401" :      \
    ((M) == COAP_402_BAD_OPTION ? "COAP_402" :      \
    ((M) == COAP_404_NOT_FOUND ? "COAP_404" :      \
    ((M) == COAP_405_METHOD_NOT_ALLOWED ? "COAP_405" :      \
    ((M) == COAP_406_NOT_ACCEPTABLE ? "COAP_406" :      \
    ((M) == COAP_408_REQ_ENTITY_INCOMPLETE ? "COAP_408" :      \
    ((M) == COAP_413_ENTITY_TOO_LARGE ? "COAP_413" :      \
    ((M) == COAP_500_INTERNAL_SERVER_ERROR ? "COAP_500" :      \
    ((M) == COAP_501_NOT_IMPLEMENTED ? "COAP_501" :      \
    ((M) == COAP_503_SERVICE_UNAVAILABLE ? "COAP_503" :      \
    ((M) == COAP_505_PROXYING_NOT_SUPPORTED ? "COAP_505" :      \
    "Unknown"))))))))))))))))))



/*
 * The maximum buffer size that is provided for resource responses and must be respected due to the limited IP buffer.
 * Larger data must be handled by the resource and will be sent chunk-wise through a TCP stream or CoAP blocks.
 */
#ifndef REST_MAX_CHUNK_SIZE
#define REST_MAX_CHUNK_SIZE                  128
#endif

#define COAP_DEFAULT_MAX_AGE                 60
#define COAP_RESPONSE_TIMEOUT                2
//TODO add marco to modify
#define COAP_MAX_RETRANSMIT                  4
#define COAP_ACK_RANDOM_FACTOR               1.5

#define COAP_MAX_TRANSMIT_WAIT               ((COAP_RESPONSE_TIMEOUT * ( (1 << (COAP_MAX_RETRANSMIT + 1) ) - 1) * COAP_ACK_RANDOM_FACTOR))
#define COAP_MIN_TRANSMIT_WAIT               (COAP_RESPONSE_TIMEOUT * ( (1 << (COAP_MAX_RETRANSMIT + 1) ) - 1))

#define COAP_HEADER_LEN                      4 /* | version:0x03 type:0x0C tkl:0xF0 | code | mid:0x00FF | mid:0xFF00 | */
#define COAP_ETAG_LEN                        8 /* The maximum number of bytes for the ETag */
#define COAP_TOKEN_LEN                       8 /* The maximum number of bytes for the Token */
#define COAP_MAX_ACCEPT_NUM                  2 /* The maximum number of accept preferences to parse/store */

#define COAP_MAX_OPTION_HEADER_LEN           5

#define COAP_HEADER_VERSION_MASK             0xC0
#define COAP_HEADER_VERSION_POSITION         6
#define COAP_HEADER_TYPE_MASK                0x30
#define COAP_HEADER_TYPE_POSITION            4
#define COAP_HEADER_TOKEN_LEN_MASK           0x0F
#define COAP_HEADER_TOKEN_LEN_POSITION       0

#define COAP_HEADER_OPTION_DELTA_MASK        0xF0
#define COAP_HEADER_OPTION_SHORT_LENGTH_MASK 0x0F

/*
 * Conservative size limit, as not all options have to be set at the same time.
 */
//TODO used for blocksize
#ifndef COAP_MAX_HEADER_SIZE
/*                            Hdr CoT Age  Tag              Obs  Tok               Blo strings */
#define COAP_MAX_HEADER_SIZE  (4 + 3 + 5 + 1+COAP_ETAG_LEN + 3 + 1+COAP_TOKEN_LEN + 4 + 30) /* 70 */
#endif /* COAP_MAX_HEADER_SIZE */

#define COAP_MAX_PACKET_SIZE  (COAP_MAX_HEADER_SIZE + REST_MAX_CHUNK_SIZE)
/*                                        0/14          48 for IPv6 (28 for IPv4) */
#if COAP_MAX_PACKET_SIZE > (UIP_BUFSIZE - UIP_LLH_LEN - UIP_IPUDPH_LEN)
//#error "UIP_CONF_BUFFER_SIZE too small for REST_MAX_CHUNK_SIZE"
#endif


/* Bitmap for set options */
enum { OPTION_MAP_SIZE = sizeof(abup_uint8) * 8 };
#define SET_OPTION(packet, opt) ((packet)->options[opt / OPTION_MAP_SIZE] |= 1 << (opt % OPTION_MAP_SIZE))
#define IS_OPTION(packet, opt) ((packet)->options[opt / OPTION_MAP_SIZE] & (1 << (opt % OPTION_MAP_SIZE)))

#ifndef MIN
#define MIN(a, b) ((a) < (b)? (a) : (b))
#endif /* MIN */

/* CoAP message types */
typedef enum
{
  COAP_TYPE_CON, /* confirmables */
  COAP_TYPE_NON, /* non-confirmables */
  COAP_TYPE_ACK, /* acknowledgements */
  COAP_TYPE_RST  /* reset */
} coap_message_type_t;

/* CoAP request method codes */
typedef enum
{
  COAP_GET = 1,
  COAP_POST,
  COAP_PUT,
  COAP_DELETE
} coap_method_t;


typedef enum 
{
  COAP_OPTION_IF_MATCH = 1,       /* 0-8 B */
  COAP_OPTION_URI_HOST = 3,       /* 1-255 B */
  COAP_OPTION_ETAG = 4,           /* 1-8 B */
  COAP_OPTION_IF_NONE_MATCH = 5,  /* 0 B */
  COAP_OPTION_OBSERVE = 6,        /* 0-3 B */
  COAP_OPTION_URI_PORT = 7,       /* 0-2 B */
  COAP_OPTION_LOCATION_PATH = 8,  /* 0-255 B */
  COAP_OPTION_URI_PATH = 11,      /* 0-255 B */
  COAP_OPTION_CONTENT_TYPE = 12,  /* 0-2 B */
  COAP_OPTION_MAX_AGE = 14,       /* 0-4 B */
  COAP_OPTION_URI_QUERY = 15,     /* 0-270 B */
  COAP_OPTION_ACCEPT = 17,        /* 0-2 B */
  COAP_OPTION_TOKEN = 19,         /* 1-8 B */
  COAP_OPTION_LOCATION_QUERY = 20, /* 1-270 B */
  COAP_OPTION_BLOCK2 = 23,        /* 1-3 B */
  COAP_OPTION_BLOCK1 = 27,        /* 1-3 B */
  COAP_OPTION_SIZE = 28,          /* 0-4 B */
  COAP_OPTION_PROXY_URI = 35,     /* 1-270 B */
} coap_option_t;

#define ABUP_COAP_OPTION_PROXY_URI 35
#define ABUP_COAP_OPTION_TOKEN 19
/* CoAP Content-Types */
typedef enum 
{
  TEXT_PLAIN = 0,
  TEXT_XML = 1, /* Indented types are not in the initial registry. */
  TEXT_CSV = 2,
  TEXT_HTML = 3,
  IMAGE_GIF = 21,
  IMAGE_JPEG = 22,
  IMAGE_PNG = 23,
  IMAGE_TIFF = 24,
  AUDIO_RAW = 25,
  VIDEO_RAW = 26,
  APPLICATION_LINK_FORMAT = 40,
  APPLICATION_XML = 41,
  APPLICATION_OCTET_STREAM = 42,
  APPLICATION_RDF_XML = 43,
  APPLICATION_SOAP_XML = 44,
  APPLICATION_ATOM_XML = 45,
  APPLICATION_XMPP_XML = 46,
  APPLICATION_EXI = 47,
  APPLICATION_FASTINFOSET = 48,
  APPLICATION_SOAP_FASTINFOSET = 49,
  APPLICATION_JSON = 50,
  APPLICATION_X_OBIX_BINARY = 51,

  CONTENT_TYPE_MAX = 0xFFFF
} coap_content_type_t;

typedef struct _multi_option_t 
{
  struct _multi_option_t *next;
  abup_uint8 is_static;
  abup_uint8 len;
  abup_uint8 *data;
} multi_option_t;

/* Parsed message struct */
// ADD FOR ATCMD
typedef struct _coap_packet_t{
  struct _coap_packet_t *next;
  abup_uint8 *buffer; /* pointer to CoAP header / incoming packet buffer / memory to serialize packet */

  abup_uint8 version;
  coap_message_type_t type;
  abup_uint8 code;
  abup_uint16 mid;

  abup_uint8 options[ABUP_COAP_OPTION_PROXY_URI / OPTION_MAP_SIZE + 1]; /* Bitmap to check if option is set */

  coap_content_type_t content_type; /* Parse options once and store; allows setting options in random order  */
  abup_uint32 max_age;
  abup_size_t proxy_uri_len;
  const abup_uint8 *proxy_uri;
  abup_uint8 etag_len;
  abup_uint8 etag[COAP_ETAG_LEN];
  abup_size_t uri_host_len;
  const abup_uint8 *uri_host;
  multi_option_t *location_path;
  abup_uint16 uri_port;
  abup_size_t location_query_len;
  abup_uint8 *location_query;
  multi_option_t *uri_path;
  abup_uint32 observe;
  abup_uint8 token_len;
  abup_uint8 token[COAP_TOKEN_LEN];
  abup_uint8 accept_num;
  abup_uint16 accept[COAP_MAX_ACCEPT_NUM];
  abup_uint8 if_match_len;
  abup_uint8 if_match[COAP_ETAG_LEN];
  abup_uint32 block2_num;
  abup_uint8 block2_more;
  abup_uint16 block2_size;
  abup_uint32 block2_offset;
  abup_uint32 block1_num;
  abup_uint8 block1_more;
  abup_uint16 block1_size;
  abup_uint32 block1_offset;
  abup_uint32 size;
  multi_option_t *uri_query;
  abup_uint8 if_none_match;

  //ADD FOR ATCMD
  abup_uint8 format;

  abup_uint16 payload_len;
  abup_uint8 *payload;

} coap_packet_t;
typedef struct{
	abup_uint8 abup_coap[ABUP_COAP_MAX_LEN];
	coap_packet_t abup_packet;
}abup_coap_packet_t;
/* Option format serialization*/
#define COAP_SERIALIZE_INT_OPTION(number, field, text)  \
    if (IS_OPTION(coap_pkt, number)) { \
      PRINTF(text" [%u]\n", coap_pkt->field); \
      option += coap_serialize_int_option(number, current_number, option, coap_pkt->field); \
      current_number = number; \
    }
#define COAP_SERIALIZE_BYTE_OPTION(number, field, text)      \
    if (IS_OPTION(coap_pkt, number)) { \
      PRINTF(text" %u [0x%02X%02X%02X%02X%02X%02X%02X%02X]\n", coap_pkt->field##_len, \
        coap_pkt->field[0], \
        coap_pkt->field[1], \
        coap_pkt->field[2], \
        coap_pkt->field[3], \
        coap_pkt->field[4], \
        coap_pkt->field[5], \
        coap_pkt->field[6], \
        coap_pkt->field[7] \
      ); /*FIXME always prints 8 bytes */ \
      option += coap_serialize_array_option(number, current_number, option, coap_pkt->field, coap_pkt->field##_len, '\0'); \
      current_number = number; \
    }
#define COAP_SERIALIZE_STRING_OPTION(number, field, splitter, text)      \
    if (IS_OPTION(coap_pkt, number)) { \
      PRINTF(text" [%.*s]\n", coap_pkt->field##_len, coap_pkt->field); \
      option += coap_serialize_array_option(number, current_number, option, (abup_uint8 *) coap_pkt->field, coap_pkt->field##_len, splitter); \
      current_number = number; \
    }
#define COAP_SERIALIZE_MULTI_OPTION(number, field, text)      \
        if (IS_OPTION(coap_pkt, number)) { \
          PRINTF(text); \
          PRINTF("\n"); \
          option += coap_serialize_multi_option(number, current_number, option, coap_pkt->field); \
          current_number = number; \
        }
#define COAP_SERIALIZE_ACCEPT_OPTION(number, field, text)  \
    if (IS_OPTION(coap_pkt, number)) { \
      abup_int i; \
      for (i=0; i<coap_pkt->field##_num; ++i) \
      { \
        PRINTF(text" [%u]\n", coap_pkt->field[i]); \
        option += coap_serialize_int_option(number, current_number, option, coap_pkt->field[i]); \
        current_number = number; \
      } \
    }
#define COAP_SERIALIZE_BLOCK_OPTION(number, field, text)      \
    if (IS_OPTION(coap_pkt, number)) \
    { \
      abup_uint32 block = coap_pkt->field##_num << 4; \
      PRINTF(text" [%lu%s (%u B/blk)]\n", coap_pkt->field##_num, coap_pkt->field##_more ? "+" : "", coap_pkt->field##_size); \
      if (coap_pkt->field##_more) block |= 0x8; \
      block |= 0xF & coap_log_2(coap_pkt->field##_size/16); \
      PRINTF(text" encoded: 0x%lX\n", block); \
      option += coap_serialize_int_option(number, current_number, option, block); \
      current_number = number; \
    }


extern coap_packet_t abup_coap_packet;
extern abup_uint16 abup_current_msgid;

extern void *abup_parse_message(abup_char *p,abup_int len);
extern abup_uint8* abup_coap_para(abup_char* message,abup_int* data_len);
extern abup_uint8* abup_coap_para_get(abup_int index,const abup_char*get,abup_int* data_len);
extern abup_uint16 coap_get_msgid(void);
extern void coap_msgid_plus(void);
extern void abup_coap_packet_init(void);
extern abup_uint16 abup_init_result_msg(abup_uint8*buf,abup_uint16 msgid,abup_uint8 *token,abup_uint8 token_len);
extern abup_char * abup_get_signptr(abup_char *buf,abup_int *len,abup_char* mid,abup_char *productId, abup_char* productSecret,abup_uint32 utc_time);
extern abup_char* abup_coap_make_data(abup_uint state, abup_uint para,abup_char* buf,abup_int *len);
extern abup_uint8 *abup_get_hal_data(void);
extern void abup_reset_hal_data(void);
extern abup_uint abup_get_hal_data_len(void);
extern abup_uint32 abup_get_download_index(void);
extern abup_uint abup_coap_callback(abup_uint8 state,abup_char *data, abup_uint len);
extern abup_bool abup_legal_ID(abup_int32 mid,abup_int32 msgid);
extern abup_char* abup_get_coap_server_host(void);
#endif
