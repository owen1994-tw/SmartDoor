#include "FingerPrint.h"


extern QueueHandle_t Ctrl_FP_Queue;
extern QueueHandle_t FP_Ctrl_Queue;

static BaseType_t xStatus;

static QueueData CtrlqueueData_rx = {
    .cmd = init,
    .result = {0},
	.systemmode = INIT,
	.status = Idle
};

static QueueData CtrlqueueData_tx = {
    .cmd = init,
    .result = {0},
	.systemmode = INIT,
	.status = Idle
};




void FPTask(void* parameter)
{

	for(;;)
	{

		/* FP -> DoorCtrl */
		if(xQueueReceive(Ctrl_FP_Queue, &CtrlqueueData_rx, pdMS_TO_TICKS(100))== pdPASS)
		{

		}



		/* DoorCtrl -> FP */
		xStatus = xQueueSend(FP_Ctrl_Queue, &CtrlqueueData_tx, pdMS_TO_TICKS(100));
		if (xStatus != pdPASS) {
			printf("FP CtrlqueueData_tx fail");
		}


	}

}




