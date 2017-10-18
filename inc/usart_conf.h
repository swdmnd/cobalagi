#ifndef USART_CONF_H_INCLUDED
#define USART_CONF_H_INCLUDED

#define USART_BUFFER_SIZE 320

#define _DEBUG_SIM900A_

#ifdef _DEBUG_SIM900A_
  #include "hw_config.h"
  #include "usb_lib.h"
  #include "usb_desc.h"
  #include "usb_pwr.h"
  #include <stdio.h>
  #include <string.h>
#endif

#include "platform_config.h"
#include <stdbool.h>

typedef struct{
  USART_TypeDef* USART_num;
  uint8_t buffer[USART_BUFFER_SIZE];
  uint16_t tail;
  uint16_t head;
  bool isoverflow;
} USART_BUFFER;

extern USART_BUFFER U1_buf_rx, U2_buf_rx, U1_buf_tx, U2_buf_tx;

void USART_Config();
uint8_t get_char(USART_BUFFER*);
void put_char(USART_BUFFER*, uint8_t);
void init_usart_buffer(USART_BUFFER*, USART_TypeDef*);
void enqueue(USART_BUFFER*, uint8_t);
uint8_t dequeue(USART_BUFFER*);
void clear_queue(USART_BUFFER*);
bool isempty(USART_BUFFER*);
void print_r(USART_BUFFER*, uint8_t*, int);

#endif /* USART_CONF_H_INCLUDED */
