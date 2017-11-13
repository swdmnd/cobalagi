#ifndef SIM900A_H_INCLUDED
#define SIM900A_H_INCLUDED

#include "usart_conf.h"
#include "timers_conf.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

extern char CMD_AT[];
extern char CMD_AT_CIPSHUT[];
extern char CMD_AT_CIPMUX[];
extern char CMD_AT_CGATT[];
extern char CMD_AT_CSTT[];
extern char CMD_AT_CIICR[];
extern char CMD_AT_CIFSR[];
extern char CMD_AT_CIPSTART[];
extern char CMD_AT_CIPSEND[];
extern char CMD_AT_CIPCLOSE[];

extern char _http_post[];
extern char _post_params[];
extern char _post_params_2[];
extern char _post_params_3[];
extern char _params[50];
extern char AT_CMD_BUF[1024];

__IO USART_BUFFER *SIM_USART_TX, *SIM_USART_RX;

void SIM900A_StartMobileData();
void SIM900A_StopMobileData();
void SIM900A_OpenConnection();
void SIM900A_Init(__IO USART_BUFFER*, __IO USART_BUFFER*);
void SIM900A_Cmd(char*);
void SIM900A_GTCSend(char*, char*, uint32_t);
void SIM900A_GTCSendText(char*);
uint32_t SIM900A_GetServerTime();

#endif /* SIM900A_H_INCLUDED */
