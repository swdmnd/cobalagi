#include "usart_conf.h"

USART_BUFFER U1_buf_rx, U2_buf_rx, U1_buf_tx, U2_buf_tx;

void USART_Config(){
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);

  GPIO_InitTypeDef gpio;
  // Initialize USART1_Tx
  gpio.GPIO_Pin = GPIO_Pin_6;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB , &gpio);

  // Initialize USART1_RX
  gpio.GPIO_Pin = GPIO_Pin_7;
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
  USART_Init(USART1,&USART_InitStructure);
  USART_Cmd(USART1, ENABLE);

  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

  // Initialize USART2_Tx
  gpio.GPIO_Pin = GPIO_Pin_10;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB , &gpio);

  // Initialize USART2_RX
  gpio.GPIO_Pin = GPIO_Pin_11;
  gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &gpio);

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
  USART_Init(USART3, &USART_InitStructure);
  USART_Cmd(USART3, ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);

  init_usart_buffer(&U1_buf_rx, USART1);
  init_usart_buffer(&U2_buf_rx, USART3);
  init_usart_buffer(&U1_buf_tx, USART1);
  init_usart_buffer(&U2_buf_tx, USART3);
}

uint8_t get_char(USART_BUFFER* u_buf){
  return dequeue(u_buf);
}

void put_char(USART_BUFFER* u_buf, uint8_t c)
{
  enqueue(u_buf , c);
  //if (!TxPrimed) {
    //TxPrimed = 1;
  if(USART_GetITStatus(u_buf->USART_num , USART_IT_TXE) == RESET)
    USART_ITConfig(u_buf->USART_num , USART_IT_TXE , ENABLE);
  //}
}

void init_usart_buffer(USART_BUFFER* u_buf, USART_TypeDef* USART_num){
  u_buf->tail=0;
  u_buf->head=0;
  u_buf->isoverflow=false;
  u_buf->USART_num=USART_num;
}

void enqueue(USART_BUFFER* u_buf, uint8_t data)
{
  uint16_t tail = u_buf->tail;
  if(++u_buf->tail >= USART_BUFFER_SIZE)
  {
    u_buf->tail = 0;
  }
  if(u_buf->tail == u_buf->head)
  {
//    if(++u_buf->head >= USART_BUFFER_SIZE)
//    {
//      u_buf->head = 0;
//    }
    u_buf->tail = tail;
    u_buf->isoverflow=true;
  }
  u_buf->buffer[tail] = data;
}

uint8_t dequeue(USART_BUFFER* u_buf)
{
  if(u_buf->tail == u_buf->head)
  {
    return 0;
  }
  uint8_t data =u_buf->buffer[u_buf->head];
  if(++u_buf->head >= USART_BUFFER_SIZE)
  {
    u_buf->head = 0;
  }
  return data;
}

void clear_queue(USART_BUFFER* u_buf)
{
  u_buf->tail = u_buf->head = u_buf->isoverflow = 0;
}

bool isempty(USART_BUFFER* u_buf)
{
  if(u_buf->tail == u_buf->head)
  {
    return true;
  } else
  {
    return false;
  }
}

void USART1_IRQHandler(void)
{
  if(USART_GetITStatus(USART1 , USART_IT_RXNE) != RESET)
  {
    uint8_t data;
    // buffer the data (or toss it if there's no room
    // Flow control will prevent this
    data = USART_ReceiveData(USART1) & 0xff;
    enqueue(&U1_buf_rx, data);
  }
  if(USART_GetITStatus(USART1 , USART_IT_TXE) != RESET)
  {
    // buffer the data (or toss it if there's no room
    // Flow control will prevent this
    if (!isempty(&U1_buf_tx)){
      uint8_t data = dequeue(&U1_buf_tx);
      USART_SendData(USART1 , data);
    } else {
      // if we have nothing to send , disable the interrupt
      // and wait for a kick
      USART_ITConfig(USART1, USART_IT_TXE , DISABLE);
      //TxPrimed = 0;
    }
  }
}

void USART3_IRQHandler(void)
{
  if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
  {
    uint8_t data;
    // buffer the data (or toss it if there's no room
    // Flow control will prevent this
    data = USART_ReceiveData(USART3) & 0xff;
#ifdef _DEBUG_SIM900A_
//    if(bDeviceState == CONFIGURED){
//      unsigned char buf[2];
//      if(data=='\n') {
//        sprintf(buf, "\r");
//        CDC_Send_DATA (buf,1);
//      }
//      sprintf(buf, "%c", data);
//      CDC_Send_DATA (buf,strlen(buf));
//    }
#else
    enqueue(&U2_buf_rx, data);
#endif
  }
  if(USART_GetITStatus(USART3 , USART_IT_TXE) != RESET)
  {

    // buffer the data (or toss it if there's no room
    // Flow control will prevent this
    if (!isempty(&U2_buf_tx)){
      uint8_t data = dequeue(&U2_buf_tx);
      USART_SendData(USART3 , data);
    } else {
      // if we have nothing to send , disable the interrupt
      // and wait for a kick
      USART_ITConfig(USART3, USART_IT_TXE , DISABLE);
      //TxPrimed = 0;
    }
  }
}

void print_r(USART_BUFFER* u_tx, uint8_t* p_arr, int length){
  for(int i = 0; i < length; ++i){
      put_char(u_tx, *(p_arr++));
  }
}
