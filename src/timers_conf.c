#include "timers_conf.h"

__IO uint32_t millis = 0;

static __IO uint32_t TimingDelay;
void delay_ms(uint32_t nTime){
  TimingDelay = nTime;
  while(TimingDelay != 0);
}

void SysTick_Handler(void){
  if (TimingDelay != 0x00) --TimingDelay;
  ++millis;
}

void RTC_Software_Init(){
  // rtc
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP | RCC_APB1Periph_PWR, ENABLE);
  PWR_BackupAccessCmd(ENABLE);
  /* Reset Backup Domain */
  //BKP_DeInit();

  /* Enable LSE */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  RCC_RTCCLKCmd(ENABLE);
  RTC_WaitForSynchro();

  RTC_WaitForLastTask();
  RTC_SetPrescaler(32767);
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
  = 1124; //1 ms
  TIM_TimeBaseStructure.TIM_Period = 31999; // every 50ms interrupt
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInit(TIM2 , &TIM_TimeBaseStructure);
  // Enable Timer
  TIM_ITConfig(TIM2 , TIM_IT_Update , ENABLE);
  TIM_Cmd(TIM2 , ENABLE);
}
