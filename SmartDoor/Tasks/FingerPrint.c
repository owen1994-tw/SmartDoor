#include "FingerPrint.h"


extern QueueHandle_t Ctrl_FP_Queue;
extern QueueHandle_t FP_Ctrl_Queue;

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




void FPTask(void* parameter)
{

	for(;;)
	{

		/* FP -> DoorCtrl */
		if(xQueueReceive(Ctrl_FP_Queue, &CtrlqueueData_rx, portMAX_DELAY)== pdPASS)
		{

		}



		/* DoorCtrl -> FP */
		xStatus = xQueueSend(FP_Ctrl_Queue, &CtrlqueueData_tx, portMAX_DELAY);
		if (xStatus != pdPASS) {
			printf("FP CtrlqueueData_tx fail");
		}


	}

}




