#include "gtclib.h"

GTC_ID_STRUCT queue_a[GTC_QUEUE_SIZE];
uint8_t queue_head=0;
uint8_t queue_tail=0;
uint8_t dequeue_pending_num=0;
__IO uint8_t numel = 0;

GTC_BUFFER_STRUCT GTC_Buffer;
uint8_t GTC_Header[GTC_HEADn] = {GTC_HEADH, GTC_HEADL};

GTC_ID_STRUCT GTC_PendingDequeue(){
  while(numel == 0);
  uint8_t queue_index = queue_head+dequeue_pending_num;
  if (queue_index >= GTC_QUEUE_SIZE){
    queue_index = GTC_QUEUE_SIZE % queue_index;
  }
  ++dequeue_pending_num;
  return queue_a[queue_index];
}

void GTC_AbortDequeue(){
  dequeue_pending_num = 0;
}

void GTC_FinalizeDequeue(){
  queue_head += dequeue_pending_num;
  if (queue_head >= GTC_QUEUE_SIZE){
    queue_head = GTC_QUEUE_SIZE % queue_head;
  }
  numel -= dequeue_pending_num;
  dequeue_pending_num = 0;
}

void GTC_Enqueue(GTC_ID_STRUCT* data){
  if(GTC_IsQueueFull()) return;
  for(int index=queue_head; index != queue_tail; ++index){
    if (index >= GTC_QUEUE_SIZE){
      index = 0;
    }
    if(queue_a[index].id_length != data->id_length) continue;
    if(memcmp(queue_a[index].id, data->id, data->id_length)==0) return;
  }
  ++numel;
  uint8_t tail = queue_tail;
  if(++queue_tail >= GTC_QUEUE_SIZE){
    queue_tail = 0;
  }
  if(queue_tail == queue_head){
    queue_tail = tail;
  }
  queue_a[tail] = *data;
}

void GTC_ClearQueue(){
  dequeue_pending_num=queue_head=queue_tail=0;
}

bool GTC_IsQueueEmpty(){
//  uint8_t queue_index = queue_head+dequeue_pending_num;
//  if (queue_index >= GTC_QUEUE_SIZE){
//    queue_index = GTC_QUEUE_SIZE % queue_index;
//  }
//  if(queue_index == queue_tail) return true;
  if(numel==0) return true;
  return false;
}

bool GTC_IsQueueFull(){
  if(numel == GTC_QUEUE_SIZE) return true;
  return false;
}

uint8_t GTC_GetNumel(){
  return numel;
}

void GTC_Send(){
  if((millis != 0 && millis%GTC_SEND_TIMEOUT==0) || GTC_GetNumel() >= GTC_SEND_NUMEL_LIMIT){
    if(!GTC_IsQueueEmpty()){
      char buf[512];
      if(bDeviceState == CONFIGURED){
        const time_t time = RTC_GetCounter();
        char* time_str = asctime(localtime(&time));
        time_str[strlen(time_str)-1] = 0;
        sprintf(buf, "[%s] Mengirim data...\r\n", time_str);
        CDC_Send_DATA(buf, strlen(buf));
      }

      GTC_ID_STRUCT id_temp;
      unsigned char text[512];
      unsigned char temp_text[64];
      sprintf(text, "{\"gate_id\":%ld, \"data\":[", (GTC_GATE_IDH<<8 | GTC_GATE_IDL));
      uint8_t numel = GTC_GetNumel();
      for(int i = 0; i < numel; ++i){
        id_temp = GTC_PendingDequeue();
        sprintf(temp_text, "[\"%02x%02x%02x%02x\",\"%ld\"]", id_temp.id[0], id_temp.id[1], id_temp.id[2], id_temp.id[3], id_temp.timestamp);
        strcat(text,temp_text);
        if(numel-i > 1) strcat(text,",");
      }
      strcat(text,"]}");
      SIM900A_GTCSendText(text);
      GTC_FinalizeDequeue();

      if(bDeviceState == CONFIGURED){
        const time_t time = RTC_GetCounter();
        char* time_str = asctime(localtime(&time));
        time_str[strlen(time_str)-1] = 0;
        sprintf(buf, "[%s] Selesai mengirim data.\r\n", time_str);
        CDC_Send_DATA(buf, strlen(buf));
      }
    }
  }
}

void GTC_Ack(GTC_ID_STRUCT* id){
  uint8_t checksum = 0x00, i = 0;
  GTC_Buffer.buffer[0] = 0x00;
  GTC_Buffer.buffer[1] = 0xFF;
  GTC_Buffer.buffer[2] = id->id_length + 3;
  GTC_Buffer.buffer[3] = 0xA1;               checksum += 0xA1;
  GTC_Buffer.buffer[4] = GTC_CMD_ACK;        checksum += GTC_CMD_ACK;
  GTC_Buffer.buffer[5] = id->id_length;      checksum += id->id_length;
  for(i = 0; i < id->id_length; ++i){
    GTC_Buffer.buffer[i+6] = id->id[i];      checksum += id->id[i];
  }
  GTC_Buffer.buffer[i+6] = (~checksum)+1;
  GTC_Buffer.length = GTC_Buffer.buffer[2] + 4;

  print_r(&U1_buf_tx, GTC_Buffer.buffer, GTC_Buffer.length);
}

void GTC_Ping(){
  uint8_t checksum = 0x00;
  GTC_Buffer.buffer[0] = 0x00;
  GTC_Buffer.buffer[1] = 0xFF;
  GTC_Buffer.buffer[2] = 4;
  GTC_Buffer.buffer[3] = 0xA1;           checksum += 0xA1;
  GTC_Buffer.buffer[4] = GTC_CMD_PING;   checksum += GTC_CMD_PING;
  GTC_Buffer.buffer[5] = GTC_GATE_IDH;   checksum += GTC_GATE_IDH;
  GTC_Buffer.buffer[6] = GTC_GATE_IDL;   checksum += GTC_GATE_IDL;
  GTC_Buffer.buffer[7] = (~checksum)+1;
  GTC_Buffer.length = 8;

  print_r(&U1_buf_tx, GTC_Buffer.buffer, GTC_Buffer.length);
}
