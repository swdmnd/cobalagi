#ifndef USART_CONF_H_INCLUDED
#define USART_CONF_H_INCLUDED

#define USART_BUFFER_SIZE 320

#define _DEBUG_SIM900A_ 0
#define _DEBUG_HC11_ 0

#ifdef _DEBUG_SIM900A_
  #include "hw_config.h"
  #include "usb_lib.h"
  #include "usb_desc.h"
  #include "usb_pwr.h"
  #include <stdio.h>
  #include <string.h>
#endif

#include "platform_config.h"
#include "timers_conf.h"
#include <stdbool.h>

typedef struct{
  USART_TypeDef* USART_num;
  uint8_t buffer[USART_BUFFER_SIZE];
  uint16_t tail;
  uint16_t head;
  bool isoverflow;
} USART_BUFFER;

extern __IO USART_BUFFER U1_buf_rx, U2_buf_rx, U1_buf_tx, U2_buf_tx;

void USART_Config();
uint8_t USART_GetChar(__IO USART_BUFFER*);
uint8_t USART_Peek(__IO USART_BUFFER*, uint16_t);
void USART_PutChar(__IO USART_BUFFER*, uint8_t);
void USART_InitBuffer(__IO USART_BUFFER*, USART_TypeDef*);
void USART_ClearBuffer(__IO USART_BUFFER*);
bool USART_IsEmpty(__IO USART_BUFFER*);
void print_r(__IO USART_BUFFER*, uint8_t*, int);
bool USART_wait_for_str(__IO USART_BUFFER*, uint8_t*, uint32_t);
bool USART_wait_for_bytes(__IO USART_BUFFER*, uint8_t*, uint8_t, uint32_t);

#endif /* USART_CONF_H_INCLUDED */
