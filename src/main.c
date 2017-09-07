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


/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USART3_BUFFER_SIZE 320

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
extern __IO uint8_t Receive_Buffer[64];
extern __IO  uint32_t Receive_length ;
extern __IO  uint32_t length ;
uint8_t Send_Buffer[64];
uint32_t packet_sent=1;
uint32_t packet_receive=1;

uint8_t USART3_buffer[USART3_BUFFER_SIZE];
uint16_t USART3_tail=0;
uint16_t USART3_head=0;
bool USART3_overflow=false;
bool USART3_empty = false;
int led_state = 0;

unsigned char welcome_msg[] = "Selamat datang di TA Muhammad Arief Fatkhurrahman!\r\n";
/* Private function prototypes -----------------------------------------------*/
void delay_ms(uint32_t);
uint8_t get_char(void);
void enqueue(int8_t);
uint8_t dequeue();
void clear_queue();

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : main.
* Descriptioan    : Main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int main(void)
{
  Set_System();
  Set_USBClock();
  USB_Interrupts_Config();
  USB_Init();


  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

  GPIO_InitTypeDef gpio;
  GPIO_StructInit(&gpio);
  gpio.GPIO_Pin = GPIO_Pin_13; // Green LED
  gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  gpio.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOC, &gpio);

  gpio.GPIO_Pin = GPIO_Pin_12;
  gpio.GPIO_Speed = GPIO_Speed_2MHz;
  gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB , &gpio);

  // Initialize USART1_Tx
  gpio.GPIO_Pin = GPIO_Pin_10;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB , &gpio);

  // Initialize USART1_RX
  gpio.GPIO_Pin = GPIO_Pin_11;
  gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB , &gpio);

  USART_InitTypeDef USART_InitStructure;
  // Initialize USART structure
  USART_StructInit(&USART_InitStructure);
  // Modify USART_InitStructure for non-default values , e.g.
  // USART_InitStructure.USART_BaudRate = 38400;
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(USART3 ,&USART_InitStructure);
  USART_Cmd(USART3 , ENABLE);

  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);


  SysTick_Config(SystemCoreClock / 1000);
  int first_time = 1;
  unsigned char data[20];
  uint8_t d = 0;
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
//            CDC_Send_DATA ((unsigned char*)Receive_Buffer,Receive_length);
          }
        }
//          CDC_Send_DATA ("aaa",3);
        Receive_length = 0;
      }
      if(!first_time){
        d = get_char();
        GPIO_WriteBit(GPIOC, GPIO_Pin_13, d%2 ? Bit_SET : Bit_RESET);
        if(!USART3_empty)
        {
          sprintf(data, "%2x", d);
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
  if (TimingDelay != 0x00)
  TimingDelay --;
}

uint8_t get_char(void){
  return dequeue();
}

void enqueue(int8_t data)
{
  USART3_buffer[USART3_tail] = data;
  if(++USART3_tail >= USART3_BUFFER_SIZE)
  {
    USART3_tail = 0;
  }
  if(USART3_tail == USART3_head)
  {
    if(++USART3_head >= USART3_BUFFER_SIZE)
    {
      USART3_head = 0;
    }
    USART3_overflow=true;
  }
}

uint8_t dequeue()
{
  USART3_empty = false;
  if(USART3_tail == USART3_head)
  {
    USART3_empty = true;
    return 0;
  }
  if(--USART3_tail < 0)
  {
    USART3_tail = USART3_BUFFER_SIZE-1;
  }
  return USART3_buffer[USART3_tail];
}

void clear_queue()
{
  USART3_tail = USART3_head = USART3_overflow = 0;
}

void USART3_IRQHandler(void)
{
  GPIO_WriteBit(GPIOC, GPIO_Pin_13, led_state ? Bit_SET : Bit_RESET);
  led_state = !led_state;
  if(USART_GetITStatus(USART3 , USART_IT_RXNE) != RESET)
  {
    uint8_t data;
    // buffer the data (or toss it if there's no room
    // Flow control will prevent this
    data = USART_ReceiveData(USART3) & 0xff;
    enqueue(data);
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
