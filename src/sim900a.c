#include "sim900a.h"

char CMD_AT[] = "AT";
char CMD_AT_CIPSHUT[] = "AT+CIPSHUT\r\n";
char CMD_AT_CIPMUX[] = "AT+CIPMUX=0\r\n";
char CMD_AT_CGATT[] = "AT+CGATT=1\r\n";
char CMD_AT_CSTT[] = "AT+CSTT=\"internet\",\"wap\",\"wap123\"\r\n";
char CMD_AT_CIICR[] = "AT+CIICR\r\n";
char CMD_AT_CIFSR[] = "AT+CIFSR\r\n";
char CMD_AT_CIPSTART[] = "AT+CIPSTART=\"TCP\",\"gtc.ariefatkhur.web.id\",80\r\n";
char CMD_AT_CIPSEND[] = "AT+CIPSEND\r\n";
char CMD_AT_CIPCLOSE[] = "AT+CIPCLOSE\r\n";

char _http_post[] = "POST /bot/gate_entry.php HTTP/1.1\r\nHost: gtc.ariefatkhur.web.id\r\nConnection: close\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\n\r\n%s\r\n";
char _post_params[] = "rfid=e2960207&gate=1&time=1506602290";
char _post_params_3[] = "rfid=%02x%02x%02x%02x&gate=1&time=1506602290";
char _post_params_2[] = "rfid=86ffecd5&gate=1&time=1506602290";
char _params[50];
char AT_CMD_BUF[255];

void sim900_config_buffer(USART_BUFFER* buf_tx){
  SIM_USART_TX = buf_tx;
}

void send_sim_cmd(char* cmd)
{
  int len = strlen(cmd);
  for(int i = 0; i < len; ++i)
  {
    put_char(SIM_USART_TX, cmd[i]);
  }
}
