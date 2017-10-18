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
#define GTC_GATE_IDL 0x01
#define GTC_GATE_IDH 0x00
#define GTC_CMD_PING 0x01
#define GTC_CMD_ECHO 0x02
#define GTC_CMD_ACK 0xFF

#define GTC_BUFFER_SIZE 32

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
long millis = 0;

/* Extern variables ----------------------------------------------------------*/
extern __IO uint8_t Receive_Buffer[64];
extern __IO  uint32_t Receive_length;
extern __IO  uint32_t length;
uint8_t packet_receive=1;
uint8_t packet_sent=1;

int led_state=0;

typedef struct{
  uint8_t buffer[GTC_BUFFER_SIZE];
  int length;
} GTC_BUFFER_STRUCT;

GTC_BUFFER_STRUCT GTC_Buffer;

unsigned char welcome_msg[] = "---Debug log start---\r\n";
/* Private function prototypes -----------------------------------------------*/
void delay_ms(uint32_t);
void Timers_Init();

int main(void)
{
  Set_System();
  Set_USBClock();
  USB_Interrupts_Config();
  USB_Init();

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

  GPIO_InitTypeDef gpio;
  GPIO_StructInit(&gpio);
  gpio.GPIO_Pin = GPIO_Pin_13; // Green LED
  gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  gpio.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOC, &gpio);

  USART_Config();
  sim900_config_buffer(&U2_buf_tx);

  SysTick_Config(SystemCoreClock / 1000);

  Timers_Init();

  int first_time = 1;
  unsigned char data[50];
  uint8_t d = 0;
  int cur_i = 0;
  unsigned char id[5];
  id[4] = 0;
  while (1)
  {
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
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
            //sprintf(data, "%c", Receive_Buffer[0]);
            //CDC_Send_DATA (data,strlen(data));
            if(Receive_Buffer[0] == '1'){
              send_sim_cmd(CMD_AT_CIPSHUT);
//              put_char(&U1_buf_tx, (uint8_t) Receive_Buffer[0]);
              //sprintf(data, "%c", Receive_Buffer[0]);
              //CDC_Send_DATA (data,strlen(data));
            } else if(Receive_Buffer[0] == '2'){
              send_sim_cmd(CMD_AT_CIPMUX);
//              put_char(&U1_buf_tx, (uint8_t) Receive_Buffer[0]);
              //sprintf(data, "%c", Receive_Buffer[0]);
              //CDC_Send_DATA (data,strlen(data));
            } else if(Receive_Buffer[0] == '3'){
              send_sim_cmd(CMD_AT_CGATT);
//              put_char(&U1_buf_tx, (uint8_t) Receive_Buffer[0]);
              //sprintf(data, "%c", Receive_Buffer[0]);
              //CDC_Send_DATA (data,strlen(data));
            } else if(Receive_Buffer[0] == '4'){
              send_sim_cmd(CMD_AT_CSTT);
//              put_char(&U1_buf_tx, (uint8_t) Receive_Buffer[0]);
              //sprintf(data, "%c", Receive_Buffer[0]);
              //CDC_Send_DATA (data,strlen(data));
            } else if(Receive_Buffer[0] == '5'){
              send_sim_cmd(CMD_AT_CIICR);
//              put_char(&U1_buf_tx, (uint8_t) Receive_Buffer[0]);
              //sprintf(data, "%c", Receive_Buffer[0]);
              //CDC_Send_DATA (data,strlen(data));
            } else if(Receive_Buffer[0] == '6'){
              send_sim_cmd(CMD_AT_CIFSR);
//              put_char(&U1_buf_tx, (uint8_t) Receive_Buffer[0]);
              //sprintf(data, "%c", Receive_Buffer[0]);
              //CDC_Send_DATA (data,strlen(data));
            } else if(Receive_Buffer[0] == '7'){
              send_sim_cmd(CMD_AT_CIPSTART);
//              put_char(&U1_buf_tx, (uint8_t) Receive_Buffer[0]);
              //sprintf(data, "%c", Receive_Buffer[0]);
              //CDC_Send_DATA (data,strlen(data));
            } else if(Receive_Buffer[0] == '8'){
              send_sim_cmd(CMD_AT_CIPSEND);
//              put_char(&U1_buf_tx, (uint8_t) Receive_Buffer[0]);
              //sprintf(data, "%c", Receive_Buffer[0]);
              //CDC_Send_DATA (data,strlen(data));
            } else if(Receive_Buffer[0] == '9'){
              sprintf(AT_CMD_BUF, _http_post, strlen(_post_params), _post_params);
              send_sim_cmd(AT_CMD_BUF);
              put_char(&U2_buf_tx, (uint8_t) 26);
//              put_char(&U1_buf_tx, (uint8_t) Receive_Buffer[0]);
              //sprintf(data, "%c", Receive_Buffer[0]);
              //CDC_Send_DATA (data,strlen(data));
            } else if(Receive_Buffer[0] == '/'){
              sprintf(AT_CMD_BUF, _http_post, strlen(_post_params_2), _post_params_2);
              send_sim_cmd(AT_CMD_BUF);
              put_char(&U2_buf_tx, (uint8_t) 26);
//              put_char(&U1_buf_tx, (uint8_t) Receive_Buffer[0]);
              //sprintf(data, "%c", Receive_Buffer[0]);
              //CDC_Send_DATA (data,strlen(data));
            } else if(Receive_Buffer[0] == '0'){
              send_sim_cmd(CMD_AT_CIPCLOSE);
//              put_char(&U1_buf_tx, (uint8_t) Receive_Buffer[0]);
              //sprintf(data, "%c", Receive_Buffer[0]);
              //CDC_Send_DATA (data,strlen(data));
            } else if(Receive_Buffer[0] != 0x0d){
              for(int i = 0; i < Receive_length; ++i){
                put_char(&U2_buf_tx, (uint8_t) Receive_Buffer[i]);
              }
              //sprintf(data, "%c", Receive_Buffer[0]);
              //CDC_Send_DATA (data,strlen(data));
            } else {
              put_char(&U2_buf_tx, (uint8_t) '\r');
              put_char(&U2_buf_tx, (uint8_t) '\n');
//              put_char(&U1_buf_tx, (uint8_t) '\r');
//              put_char(&U1_buf_tx, (uint8_t) '\n');
              //sprintf(data, "\r\n");
              //CDC_Send_DATA (data,2);
            }
          }
        }
//          CDC_Send_DATA ("aaa",3);
        Receive_length = 0;
      }
      if(!first_time){
        if(!isempty(&U1_buf_rx))
        {
          d = get_char(&U1_buf_rx);
          //sprintf(data, "%02x", d);
          //CDC_Send_DATA (data,strlen(data));
          id[cur_i++] = d;
          if(cur_i>3){
            sprintf(data, "[%ld] RFID Detected: %02x%02x%02x%02x\r\n", millis, id[0], id[1], id[2], id[3]);
            CDC_Send_DATA (data,strlen(data));
            sprintf(data, "[%ld] Connecting...\r\n", millis);
            CDC_Send_DATA (data,strlen(data));
            cur_i = 0;
            send_sim_cmd(CMD_AT_CIPSHUT);
            delay_ms(1500);
            send_sim_cmd(CMD_AT_CIPMUX);
            delay_ms(1500);
            send_sim_cmd(CMD_AT_CGATT);
            delay_ms(1500);
            send_sim_cmd(CMD_AT_CSTT);
            delay_ms(1500);
            send_sim_cmd(CMD_AT_CIICR);
            delay_ms(1500);
            send_sim_cmd(CMD_AT_CIFSR);
            delay_ms(1500);
            send_sim_cmd(CMD_AT_CIPSTART);
            delay_ms(1500);
            sprintf(data, "[%ld] Sending data...\r\n", millis);
            CDC_Send_DATA (data,strlen(data));
            send_sim_cmd(CMD_AT_CIPSEND);
            delay_ms(1500);
            sprintf(data, _post_params_3, id[0], id[1], id[2], id[3]);
            sprintf(AT_CMD_BUF, _http_post, strlen(data), data);
            delay_ms(1500);
            send_sim_cmd(AT_CMD_BUF);
            put_char(&U2_buf_tx, (uint8_t) 26);
            sprintf(data, "[%ld] Data sent.\r\n", millis);
            CDC_Send_DATA (data,strlen(data));
            //send_sim_cmd(CMD_AT_CIPCLOSE);
          }
        }

        if(!isempty(&U2_buf_rx))
        {
          d = get_char(&U2_buf_rx);
          if(d=='\n') {
            sprintf(data, "\r");
            CDC_Send_DATA (data,strlen(data));
          }
          sprintf(data, "%c", d);
          CDC_Send_DATA (data,strlen(data));
        }
      }
    } else {
      first_time=1;
      GPIO_WriteBit(GPIOC, GPIO_Pin_13, led_state ? Bit_SET : Bit_RESET);
      led_state = !led_state;

      delay_ms(200);
    }


  }
}

static __IO uint32_t TimingDelay;
void delay_ms(uint32_t nTime){
  TimingDelay = nTime;
  while(TimingDelay != 0);
}

void SysTick_Handler(void){
  if (TimingDelay != 0x00) --TimingDelay;
  ++millis;
}

void Timers_Init(){
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  // enable timer clock
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
  // configure NVIC
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  // configure timer
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Prescaler
  = SystemCoreClock / 10000 - 1; //1 ms
  TIM_TimeBaseStructure.TIM_Period = 10000-1; // every 50ms interrupt
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInit(TIM2 , &TIM_TimeBaseStructure);
  // Enable Timer
  TIM_ITConfig(TIM2 , TIM_IT_Update , ENABLE);
  TIM_Cmd(TIM2 , ENABLE);

  // rtc
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP | RCC_APB1Periph_PWR, ENABLE);
  PWR_BackupAccessCmd(ENABLE);
  /* Reset Backup Domain */
  //BKP_DeInit();

  /* Enable LSE */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  RCC_RTCCLKCmd(ENABLE);
  RTC_WaitForSynchro();

  RTC_WaitForLastTask();
  RTC_SetPrescaler(32767);
  RTC_WaitForLastTask();
}

void TIM2_IRQHandler(void)
{
  TIM_ClearITPendingBit(TIM2 ,TIM_IT_Update);
  /* do something */
  GTC_Buffer.buffer[0] = 0x00;
  GTC_Buffer.buffer[1] = 0xFF;
  GTC_Buffer.buffer[2] = 3;
  GTC_Buffer.buffer[3] = GTC_CMD_PING;
  GTC_Buffer.buffer[4] = GTC_GATE_IDH;
  GTC_Buffer.buffer[5] = GTC_GATE_IDL;
  GTC_Buffer.length = 6;

  print_r(&U1_buf_tx, GTC_Buffer.buffer, GTC_Buffer.length);
  if(bDeviceState == CONFIGURED){
    char buf[512];
    sprintf(buf, "[%ld] PING sent\r\n", RTC_GetCounter());
    CDC_Send_DATA(buf, strlen(buf));
  }
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
