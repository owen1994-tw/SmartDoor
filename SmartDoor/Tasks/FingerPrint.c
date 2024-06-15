#include "FingerPrint.h"
#include "FingerPrint/as608.h"

extern QueueHandle_t Ctrl_FP_Queue;
extern QueueHandle_t FP_Ctrl_Queue;

static BaseType_t xStatus;

static QueueData CtrlqueueData_rx = { .cmd = init, .result = { 0 },
		.systemmode = INIT, .status = Idle };

static QueueData CtrlqueueData_tx = { .cmd = init, .result = { 0 },
		.systemmode = INIT, .status = Idle };

int fail = 0;

int set_page_count = 0;

void FPTask(void *parameter) {

	for (;;) {

		/*  check fp   */
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




		/*  btn   */
//		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
//		{
//			vTaskDelay(50);
//			if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
//			{
//				if(set_page_count == 0)
//				{
//					set_page_count = 1;
//				}
//				else
//				{
//					set_page_count = 0;
//				}
//				while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET);      // 等待按键松开
//
//			}
//		}

		/*  enroll fp   */
//		LD6 blue
//		LD5 red
//		LD3 orange
//		LD4 green



//		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin | LD5_Pin | LD6_Pin | LD3_Pin ,
//				GPIO_PIN_RESET);
//
//
//		vTaskDelay(2000);
////		Ready
//		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
//		vTaskDelay(300);
//		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
//		vTaskDelay(300);
//		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
//		vTaskDelay(300);
//		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
//
//		vTaskDelay(2000);
//
//		fail = register_new_fingerprint(&huart2, set_page_count);
//		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin | LD5_Pin | LD6_Pin | LD3_Pin ,
//				GPIO_PIN_RESET);
//		if (fail)
//			HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);
//		else
//			HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
//		vTaskDelay(2000);





		/*  delte one fp   */
//		LD6 blue
//		LD5 red
//		LD3 orange
//		LD4 green


//		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin | LD5_Pin | LD6_Pin,
//				GPIO_PIN_RESET);
//
//		fail = FP_Delete(&huart2, set_page_count);
//
//		if (fail)
//			HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);
//		else
//			HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
//		HAL_Delay(1000);




		/*  delte all fp   */
//		LD6 blue
//		LD5 red
//		LD3 orange
//		LD4 green


//		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin | LD5_Pin | LD6_Pin,
//				GPIO_PIN_RESET);
//
//		fail = FP_Empty(&huart2);
//
//		if (fail)
//			HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);
//		else
//			HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
//		HAL_Delay(1000);


		/* FP -> DoorCtrl */
//		if(xQueueReceive(Ctrl_FP_Queue, &CtrlqueueData_rx, pdMS_TO_TICKS(100))== pdPASS)
//		{
//
//		}

		/* DoorCtrl -> FP */
//		xStatus = xQueueSend(FP_Ctrl_Queue, &CtrlqueueData_tx, pdMS_TO_TICKS(100));
//		if (xStatus != pdPASS) {
//			printf("FP CtrlqueueData_tx fail");
//		}
	}

}

