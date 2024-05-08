#include "RFID.h"

extern QueueHandle_t Ctrl_RFID_Queue;
extern QueueHandle_t RFID_Ctrl_Queue;
static BaseType_t xStatus;

static QueueData CtrlqueueData_rx = {
    .cmd = {0},
    .result = {0},
	.systemmode = INIT,
	.status = Idle
};

static QueueData CtrlqueueData_tx = {
    .cmd = {0},
    .result = {0},
	.systemmode = INIT,
	.status = Idle
};

uint32_t counter =0;



void RFIDTask(void* parameter)
{


	for(;;)
	{

		/* DoorCtrl -> RFID */
		if(xQueueReceive(Ctrl_RFID_Queue, &CtrlqueueData_rx, portMAX_DELAY)== pdPASS)
		{
//			if(CtrlqueueData_rx.cmd.start == 1)
//			{
//				CtrlqueueData_tx.status = Busy;
//				OLED_ShowString(0, 16, "RFID Busy ", OLED_8X16);
//				counter++;
//
//				if(counter > 10)
//				{
//					CtrlqueueData_tx.status = Idle;
//					CtrlqueueData_tx.result.pass = 1;
//					OLED_Clear();
//
//
//				}
//			}
//			else if(CtrlqueueData_rx.cmd.start != 1)
//			{
//				counter = 0;
//			}

		}

		/* RFID -> DoorCtrl */
		xStatus = xQueueSend(RFID_Ctrl_Queue, &CtrlqueueData_tx, portMAX_DELAY);
		if (xStatus != pdPASS) {
			printf("RFID CtrlqueueData_tx fail");
		}


     vTaskDelay(20);
	}

}




