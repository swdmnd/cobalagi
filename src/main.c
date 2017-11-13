/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Virtual Com Port Demo main file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : main.
* Descriptioan    : Main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/

#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define __SET_HC11__ 0

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* Extern variables ----------------------------------------------------------*/
extern __IO uint8_t Receive_Buffer[64];
extern __IO  uint32_t Receive_length;
extern __IO  uint32_t length;
uint8_t packet_receive=1;
uint8_t packet_sent=1;

int led_state=0;
GTC_ID_STRUCT id;
int cur_i = 0;

unsigned char welcome_msg[] = "---Debug log start---\r\n";
/* Private function prototypes -----------------------------------------------*/
void ComPort_PrintTimeStamp();

int main(void)
{
  Set_System();
  Set_USBClock();
  USB_Interrupts_Config();
  USB_Init();

  SysTick_Config(SystemCoreClock / 1000);
  NVIC_SetPriority(SysTick_IRQn, 1);
  RTC_Software_Init();

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

  GPIO_InitTypeDef gpio;
  GPIO_StructInit(&gpio);
  gpio.GPIO_Pin = GPIO_Pin_13; // Green LED
  gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  gpio.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOC, &gpio);
  GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);

  USART_Config();

#if __SET_HC11__
  /*
    Set transmitting power of module, x is optional from 1 to 8, respectively representing -30dBm,
    -20dBm, -15dBm, -10dBm, 0dBm, 5dBm, 7dBm, and 10dBm; and the default is 8 (namely,
    10dBm).
    e.g.:
      Send ÅgAT+P6Åh to module
      And the module returns ÅgOK-P6Åh
  */
  delay_ms(1000);
  print_r(&U1_buf_tx, "AT\r\n", 4);
  USART_wait_for_str(&U1_buf_rx, "OK", 10000);
  print_r(&U1_buf_tx, "AT+P8\r\n", 7);

  while(1);
#endif

  //delay_ms(5000);
//  unsigned char data[50];
//  sprintf(data, "Mengirim: Pengujian pengiriman data.\r\nDiterima: ");
//  CDC_Send_DATA (data,strlen(data));
//  sprintf(data, "Pengujian pengiriman data.");
//  print_r(&U1_buf_tx, data, strlen(data));
//  while(1){
//    sprintf(data, "%c", USART_GetChar(&U1_buf_rx));
//    CDC_Send_DATA (data,strlen(data));
//  }
  SIM900A_Init(&U2_buf_tx, &U2_buf_rx);

  //sync time
  RTC_WaitForLastTask();
  RTC_SetCounter(SIM900A_GetServerTime());
  RTC_WaitForLastTask();

  if(bDeviceState == CONFIGURED){
    char buf[512];
    const time_t time = RTC_GetCounter();
    char* time_str = asctime(localtime(&time));
    time_str[strlen(time_str)-1] = 0;
    sprintf(buf, "[%s] Sinkronisasi waktu selesai.\r\n", time_str);
    CDC_Send_DATA(buf, strlen(buf));
  }

  Timers_Init();

  int first_time = 0;
  unsigned char data[50];
  uint8_t d = 0;
  GTC_ID_STRUCT id1;
  while (1)
  {
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
    if (bDeviceState == CONFIGURED)
    {
      CDC_Receive_DATA();
      /*Check to see if we have data yet */
      if (Receive_length  != 0)
      {
        if (packet_sent == 1){
          if(first_time){
            first_time=0;
            CDC_Send_DATA (welcome_msg,strlen(welcome_msg));
          } else {
            if(Receive_Buffer[0] == '1'){
              SIM900A_Cmd(CMD_AT_CIPSHUT);
            } else if(Receive_Buffer[0] == '2'){
              SIM900A_Cmd(CMD_AT_CIPMUX);
            } else if(Receive_Buffer[0] == '3'){
              SIM900A_Cmd(CMD_AT_CGATT);
            } else if(Receive_Buffer[0] == '4'){
              SIM900A_Cmd(CMD_AT_CSTT);
            } else if(Receive_Buffer[0] == '5'){
              SIM900A_Cmd(CMD_AT_CIICR);
            } else if(Receive_Buffer[0] == '6'){
              SIM900A_Cmd(CMD_AT_CIFSR);
            } else if(Receive_Buffer[0] == '7'){
              SIM900A_Cmd(CMD_AT_CIPSTART);
            } else if(Receive_Buffer[0] == '8'){
              SIM900A_Cmd(CMD_AT_CIPSEND);
            } else if(Receive_Buffer[0] == '9'){
              sprintf(AT_CMD_BUF, _http_post, strlen(_post_params_3), _post_params);
              SIM900A_Cmd(AT_CMD_BUF);
              USART_PutChar(&U2_buf_tx, (uint8_t) 26);
            } else if(Receive_Buffer[0] == '/'){
              sprintf(AT_CMD_BUF, _http_post, strlen(_post_params_2), _post_params_2);
              SIM900A_Cmd(AT_CMD_BUF);
              USART_PutChar(&U2_buf_tx, (uint8_t) 26);
            } else if(Receive_Buffer[0] == '0'){
              SIM900A_Cmd(CMD_AT_CIPCLOSE);
            } else if(Receive_Buffer[0] != 0x0d){
              for(int i = 0; i < Receive_length; ++i){
                USART_PutChar(&U2_buf_tx, (uint8_t) Receive_Buffer[i]);
              }
            } else {
              USART_PutChar(&U2_buf_tx, (uint8_t) '\r');
              USART_PutChar(&U2_buf_tx, (uint8_t) '\n');
            }
          }
        }
        Receive_length = 0;
      }
      if(!first_time){
        //GTC_Send();
      }
    } else {
      first_time=1;
      //GPIO_WriteBit(GPIOC, GPIO_Pin_13, led_state ? Bit_SET : Bit_RESET);
      led_state = !led_state;

      delay_ms(200);
    }
    GTC_Send();
  }
}

void TIM2_IRQHandler(void)
{
  GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
  uint8_t max_retry = 0;
  while(!USART_IsEmpty(&U1_buf_rx) && max_retry < 4)
  {
    ++max_retry;
    if(USART_wait_for_bytes(&U1_buf_rx, GTC_Header, GTC_HEADn, 1000)){
      uint8_t msg_length = USART_GetChar(&U1_buf_rx);
      uint8_t checksum=0;
      for(int i = 0; i < msg_length; ++i){
        GTC_Buffer.buffer[i] = USART_GetChar(&U1_buf_rx);
        checksum += GTC_Buffer.buffer[i];
      }
      if((checksum + USART_GetChar(&U1_buf_rx)) & 0xFF != 0x00) continue;
      if(GTC_Buffer.buffer[0] == 0xA2){
        id.timestamp = RTC_GetCounter();

        id.id_length = GTC_Buffer.buffer[1];
        for(int i = 0; i < id.id_length; ++i){
          id.id[i] = GTC_Buffer.buffer[i+2];
        }
        GTC_Enqueue(&id);
        GTC_Ack(&id);
        if(bDeviceState == CONFIGURED){
          char data[50];
          sprintf(data, "RFID Detected: %02x%02x%02x%02x\r\n", id.id[0], id.id[1], id.id[2], id.id[3]);
          CDC_Send_DATA (data,strlen(data));
        }
      }
    }
  }

  /* do something */
  GTC_Ping();
//  if(bDeviceState == CONFIGURED){
//    char buf[512];
//    const time_t time = RTC_GetCounter();
//    char* time_str = asctime(localtime(&time));
//    time_str[strlen(time_str)-1] = 0;
//    sprintf(buf, "[%s] PING sent\r\n", time_str);
//    CDC_Send_DATA(buf, strlen(buf));
//  }
  TIM_ClearITPendingBit(TIM2 ,TIM_IT_Update);
  GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
}
#ifdef USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
