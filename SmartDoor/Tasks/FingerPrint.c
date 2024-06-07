#include "FingerPrint.h"
#include "FingerPrint/as608.h"

extern QueueHandle_t Ctrl_FP_Queue;
extern QueueHandle_t FP_Ctrl_Queue;

static BaseType_t xStatus;

static QueueData CtrlqueueData_rx = { .cmd = init, .result = { 0 },
		.systemmode = INIT, .status = Idle };

static QueueData CtrlqueueData_tx = { .cmd = init, .result = { 0 },
		.systemmode = INIT, .status = Idle };

void FPTask(void *parameter) {

	for (;;) {
		int fail = 0;
		HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_SET);
		fail = search_fingerprint(&huart2);
		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin | LD5_Pin | LD6_Pin, GPIO_PIN_RESET);
		if (fail){
			CtrlqueueData_tx.result.pass = 0;
			HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);
		}
		else{
			CtrlqueueData_tx.result.pass = 1;
			HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
		}
		HAL_Delay(1000);
		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin | LD5_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_SET);

//		fail = register_new_fingerprint(&huart2);
//		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin | LD5_Pin | LD6_Pin,
//				GPIO_PIN_RESET);
//		if (fail)
//			HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);
//		else
//			HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
//		HAL_Delay(1000);
//		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin | LD5_Pin, GPIO_PIN_RESET);
		/* FP -> DoorCtrl */
//		if(xQueueReceive(Ctrl_FP_Queue, &CtrlqueueData_rx, pdMS_TO_TICKS(100))== pdPASS)
//		{
//
//		}

		/* DoorCtrl -> FP */
		xStatus = xQueueSend(FP_Ctrl_Queue, &CtrlqueueData_tx, pdMS_TO_TICKS(100));
		if (xStatus != pdPASS) {
			printf("FP CtrlqueueData_tx fail");
		}
	}

}

