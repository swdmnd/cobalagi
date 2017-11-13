#include "usart_conf.h"

__IO USART_BUFFER U1_buf_rx, U2_buf_rx, U1_buf_tx, U2_buf_tx;

void enqueue(__IO USART_BUFFER*, uint8_t);
uint8_t dequeue(__IO USART_BUFFER*);

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

  USART_InitBuffer(&U1_buf_rx, USART1);
  USART_InitBuffer(&U2_buf_rx, USART3);
  USART_InitBuffer(&U1_buf_tx, USART1);
  USART_InitBuffer(&U2_buf_tx, USART3);
}

uint8_t USART_GetChar(__IO USART_BUFFER* u_buf){
  while (USART_IsEmpty(u_buf));
  return dequeue(u_buf);
}

uint8_t USART_Peek(__IO USART_BUFFER* u_buf, uint16_t offset){
  uint16_t peek_i = offset + u_buf->head;
  if (peek_i >= USART_BUFFER_SIZE) peek_i %= USART_BUFFER_SIZE;
  // Kalau di luar range buffer
  if (u_buf->head < u_buf->tail){
    if(u_buf->head >= peek_i || peek_i >= u_buf->tail) return 0;
  }
  if (u_buf->head > u_buf->tail){
    if(u_buf->head <= peek_i && peek_i <= u_buf->tail) return 0;
  }
  return u_buf->buffer[peek_i];
}

void USART_PutChar(__IO USART_BUFFER* u_buf, uint8_t c)
{
  enqueue(u_buf , c);
  //if (!TxPrimed) {
    //TxPrimed = 1;
  if(USART_GetITStatus(u_buf->USART_num , USART_IT_TXE) == RESET)
    USART_ITConfig(u_buf->USART_num , USART_IT_TXE , ENABLE);
  //}
}

void USART_InitBuffer(__IO USART_BUFFER* u_buf, USART_TypeDef* USART_num){
  u_buf->tail=0;
  u_buf->head=0;
  u_buf->isoverflow=false;
  u_buf->USART_num=USART_num;
}

void enqueue(__IO USART_BUFFER* u_buf, uint8_t data)
{
  uint16_t tail = u_buf->tail;
  if(++u_buf->tail >= USART_BUFFER_SIZE)
  {
    u_buf->tail = 0;
  }
  if(u_buf->tail == u_buf->head)
  {
    u_buf->tail = tail;
    u_buf->isoverflow=true;
    return;
  }
  u_buf->buffer[tail] = data;
}

uint8_t dequeue(__IO USART_BUFFER* u_buf)
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

void USART_ClearBuffer(__IO USART_BUFFER* u_buf)
{
  u_buf->tail = u_buf->head = u_buf->isoverflow = 0;
}

bool USART_IsEmpty(__IO USART_BUFFER* u_buf)
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
#if _DEBUG_HC11_
    if(bDeviceState == CONFIGURED){
      unsigned char buf[2];
      if(data=='\n') {
        sprintf(buf, "\r");
        CDC_Send_DATA (buf,1);
      }
      sprintf(buf, "%c", data);
      CDC_Send_DATA (buf,strlen(buf));
    }
#endif
    enqueue(&U1_buf_rx, data);
  }
  if(USART_GetITStatus(USART1 , USART_IT_TXE) != RESET)
  {
    // buffer the data (or toss it if there's no room
    // Flow control will prevent this
    if (!USART_IsEmpty(&U1_buf_tx)){
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
#if _DEBUG_SIM900A_
    if(bDeviceState == CONFIGURED){
      unsigned char buf[2];
      if(data=='\n') {
        sprintf(buf, "\r");
        CDC_Send_DATA (buf,1);
      }
      sprintf(buf, "%c", data);
      CDC_Send_DATA (buf,strlen(buf));
    }
#endif
    enqueue(&U2_buf_rx, data);
  }
  if(USART_GetITStatus(USART3 , USART_IT_TXE) != RESET)
  {

    // buffer the data (or toss it if there's no room
    // Flow control will prevent this
    if (!USART_IsEmpty(&U2_buf_tx)){
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

void print_r(__IO USART_BUFFER* u_tx, uint8_t* p_arr, int length){
  for(int i = 0; i < length; ++i){
      USART_PutChar(u_tx, *(p_arr++));
  }
}

bool USART_wait_for_str(__IO USART_BUFFER* u_buf, uint8_t* str, uint32_t timeout){
  USART_wait_for_bytes(u_buf, str, strlen(str), timeout);
}

bool USART_wait_for_bytes(__IO USART_BUFFER* u_buf, uint8_t* bytes, uint8_t num_bytes, uint32_t timeout){
  uint32_t ms = millis;
  int i = 0;
  uint8_t byte;
  while( millis-ms < timeout ){
    if(!USART_IsEmpty(u_buf)){
      byte = USART_GetChar(u_buf);
      if(byte == bytes[i]){
        if(++i == num_bytes) return true;
      } else {
        i=0;
        if(byte == bytes[i]){
          ++i;
        }
      }
    }
  }
  return false;
}
