#ifndef __RFIDTASK_H
#define __RFIDTASK_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <MFRC522/MFRC522.h>
//extern QueueHandle_t RFIDQueue;

void RFIDTask(void* parameter);



#endif
