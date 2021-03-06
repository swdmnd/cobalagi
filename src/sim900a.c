#include "sim900a.h"

char CMD_AT[] = "AT\r\n";
char CMD_AT_CIPSHUT[] = "AT+CIPSHUT\r\n";
char CMD_AT_CIPMUX[] = "AT+CIPMUX=0\r\n";
char CMD_AT_CGATT[] = "AT+CGATT=1\r\n";
char CMD_AT_CSTT[] = "AT+CSTT=\"internet\",\"wap\",\"wap123\"\r\n";
char CMD_AT_CIICR[] = "AT+CIICR\r\n";
char CMD_AT_CIFSR[] = "AT+CIFSR\r\n";
char CMD_AT_CIPSTART[] = "AT+CIPSTART=\"TCP\",\"gtc.ariefatkhur.web.id\",80\r\n";
char CMD_AT_CIPSEND[] = "AT+CIPSEND\r\n";
char CMD_AT_CIPCLOSE[] = "AT+CIPCLOSE\r\n";

char _http_post[] = "POST /bot/gate_entry.php HTTP/1.1\r\nHost: gtc.ariefatkhur.web.id\r\nConnection: close\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n%s\r\n";
char _http_json[] = "POST /bot/tes_json.php HTTP/1.1\r\nHost: gtc.ariefatkhur.web.id\r\nConnection: close\r\nContent-Type: application/json\r\nContent-Length: 18\r\n\r\n{'tes':'tes lagi'}\r\n\r\n";
char _http_get_time[] = "GET /bot/get_server_time.php HTTP/1.1\r\nHost: gtc.ariefatkhur.web.id\r\nConnection: close\r\n\r\n";
char _post_params_3[] = "rfid=e2960207&gate=1&time=1506602290";
char _post_params[] = "rfid=%02x%02x%02x%02x&gate=%s&time=%ld";
char _post_params_2[] = "rfid=86ffecd5&gate=1&time=1506602290";
char _params[50];
char AT_CMD_BUF[1024];

void SIM900A_Init(__IO USART_BUFFER* buf_tx, __IO USART_BUFFER* buf_rx){
  SIM_USART_TX = buf_tx;
  SIM_USART_RX = buf_rx;
  while(1){
    SIM900A_Cmd(CMD_AT);
    if(USART_wait_for_str(SIM_USART_RX, "OK", 2500)) break;
  }
  delay_ms(2500);
}

void SIM900A_StartMobileData(){
  USART_ClearBuffer(SIM_USART_RX);
  while(1){
    SIM900A_Cmd(CMD_AT_CIPSHUT);
    if(!USART_wait_for_str(SIM_USART_RX, "OK", 5000)) continue;
    SIM900A_Cmd(CMD_AT_CIPMUX);
    if(!USART_wait_for_str(SIM_USART_RX, "OK", 5000)) continue;
    SIM900A_Cmd(CMD_AT_CGATT);
    if(!USART_wait_for_str(SIM_USART_RX, "OK", 5000)) continue;
    SIM900A_Cmd(CMD_AT_CSTT);
    if(!USART_wait_for_str(SIM_USART_RX, "OK", 5000)) continue;
    SIM900A_Cmd(CMD_AT_CIICR);
    if(!USART_wait_for_str(SIM_USART_RX, "OK", 10000)) continue;
    SIM900A_Cmd(CMD_AT_CIFSR);
    if(!USART_wait_for_str(SIM_USART_RX, ".", 5000)) continue;
    break;
  }
}

void SIM900A_StopMobileData(){
  USART_ClearBuffer(SIM_USART_RX);
  while(1){
    SIM900A_Cmd(CMD_AT_CIPSHUT);
    if(!USART_wait_for_str(SIM_USART_RX, "OK", 5000)) continue;
    break;
  }
}

void SIM900A_OpenConnection(){
  USART_ClearBuffer(SIM_USART_RX);
  while(1){
    SIM900A_Cmd(CMD_AT_CIPSTART);
    if(!USART_wait_for_str(SIM_USART_RX, "CONNECT OK", 5000)) continue;
    break;
  }
}

void SIM900A_Cmd(char* cmd)
{
  int len = strlen(cmd);
  for(int i = 0; i < len; ++i)
  {
    USART_PutChar(SIM_USART_TX, cmd[i]);
  }
}

void SIM900A_GTCSend(char* rfid, char* gate_id, uint32_t timestamp){
  sprintf(_params, _post_params, rfid[0], rfid[1], rfid[2], rfid[3], gate_id, timestamp);
  sprintf(AT_CMD_BUF, _http_post, strlen(_params), _params);

  SIM900A_StartMobileData();
  SIM900A_OpenConnection();
  SIM900A_Cmd(CMD_AT_CIPSEND);
  USART_wait_for_str(SIM_USART_RX, ">", 5000);
  SIM900A_Cmd(AT_CMD_BUF);
  USART_PutChar(&U2_buf_tx, (uint8_t) 26);
  USART_ClearBuffer(SIM_USART_RX);
}

void SIM900A_GTCSendText(char* text){
  uint16_t length = strlen(text);
  sprintf(AT_CMD_BUF, _http_post, strlen(text), text);

  SIM900A_StartMobileData();
  SIM900A_OpenConnection();
  SIM900A_Cmd(CMD_AT_CIPSEND);
  USART_wait_for_str(SIM_USART_RX, ">", 5000);
  SIM900A_Cmd(AT_CMD_BUF);
  USART_PutChar(&U2_buf_tx, (uint8_t) 26);
  USART_ClearBuffer(SIM_USART_RX);
}

uint32_t SIM900A_GetServerTime(){
  uint8_t ch, i=0, time_a[20]="99999";
  uint32_t time_start = millis;
  while(1){
    SIM900A_StartMobileData();
    SIM900A_OpenConnection();
    SIM900A_Cmd(CMD_AT_CIPSEND);
    USART_wait_for_str(SIM_USART_RX, ">", 5000);
    SIM900A_Cmd(_http_get_time);
    USART_PutChar(&U2_buf_tx, (uint8_t) 26);
    USART_ClearBuffer(SIM_USART_RX);
    if(!USART_wait_for_str(SIM_USART_RX, "timestamp=", 10000)) continue;
    break;
  }
  while(1){
    ch=USART_GetChar(SIM_USART_RX);
    if(ch == '&') break;
    time_a[i++] = ch;
  }
  time_a[i] = 0;
  delay_ms(2500);
  return (uint32_t)strtoul(time_a, NULL, 10) + ((uint32_t)((millis-time_start)/1000)+1) + (uint32_t) 25200;
}
