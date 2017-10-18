#ifndef SIM900A_H_INCLUDED
#define SIM900A_H_INCLUDED

#include "usart_conf.h"
#include <string.h>

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
extern char AT_CMD_BUF[255];

USART_BUFFER* SIM_USART_TX;

void sim900_config_buffer(USART_BUFFER*);
void send_sim_cmd(char*);

#endif /* SIM900A_H_INCLUDED */
