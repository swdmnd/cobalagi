#ifndef GTCLIB_H_INCLUDED
#define GTCLIB_H_INCLUDED

#include "timers_conf.h"
#include "usart_conf.h"
#include "sim900a.h"

#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <stdint.h>
#include <stdbool.h>

#define GTC_GATE_IDL 0x01
#define GTC_GATE_IDH 0x00
#define GTC_HEADH 0x00
#define GTC_HEADL 0xFF
#define GTC_HEADn 2
#define GTC_CMD_PING 0x01
#define GTC_CMD_ECHO 0x02
#define GTC_CMD_ACK 0xFF

#define GTC_ID_LEN 4

#define GTC_BUFFER_SIZE 32
#define GTC_QUEUE_SIZE 10
#define GTC_SEND_TIMEOUT 30000
#define GTC_SEND_NUMEL_LIMIT 5

typedef struct{
  uint8_t buffer[GTC_BUFFER_SIZE];
  int length;
} GTC_BUFFER_STRUCT;

typedef struct{
  uint8_t id[GTC_ID_LEN];
  int id_length;
  uint32_t timestamp;
} GTC_ID_STRUCT;

extern GTC_BUFFER_STRUCT GTC_Buffer;
extern uint8_t GTC_Header[];

GTC_ID_STRUCT GTC_PendingDequeue();
void GTC_AbortDequeue();
void GTC_FinalizeDequeue();
void GTC_Enqueue(GTC_ID_STRUCT*);
void GTC_ClearQueue();
bool GTC_IsQueueEmpty();
bool GTC_IsQueueFull();
uint8_t GTC_GetNumel();
void GTC_Send();
void GTC_Ack(GTC_ID_STRUCT*);
void GTC_Ping();

#define GTC_Init() GTC_ClearQueue()

#endif /* GTCLIB_H_INCLUDED */
