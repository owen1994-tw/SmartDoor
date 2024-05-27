#include "RFID.h"

extern QueueHandle_t Ctrl_RFID_Queue;
extern QueueHandle_t RFID_Ctrl_Queue;
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

uint32_t counter =0;


extern uint8_t data;
extern uint8_t status;
extern uint8_t str[MAX_LEN]; // Max_LEN = 16
extern uint8_t sNum[5];

//uint8_t data;
//uint8_t status;
//uint8_t str[MAX_LEN]; // Max_LEN = 16
//uint8_t sNum[5];


void RFIDTask(void* parameter)
{


	for(;;)
	{

		status = MFRC522_Request(PICC_REQIDL, str);
		status = MFRC522_Anticoll(str);

		if (status == MI_OK)
		{
			memcpy(sNum, str, 5);
			vTaskDelay(100);
			printf("Card UID: %02X %02X %02X %02X %02X\r\n", sNum[0], sNum[1], sNum[2], sNum[3], sNum[4]);

			if((str[0]==67) && (str[1]==4) && (str[2]==23) && (str[3]==20) && (str[4]==68) )
			{
			  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_SET);
			  vTaskDelay(500);
			  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_RESET);
			}
			else if((str[0]==243) && (str[1]==188) && (str[2]==5) && (str[3]==53) && (str[4]==127) )
			{
			  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_SET);
			  vTaskDelay(500);
			  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_RESET);
			}
			memset(str, 0, sizeof(str));
			CtrlqueueData_tx.result.pass = 1;
		}
		else
		{
			CtrlqueueData_tx.result.pass = 0;
		}


		/* DoorCtrl -> RFID */
		if(xQueueReceive(Ctrl_RFID_Queue, &CtrlqueueData_rx, pdMS_TO_TICKS(200))== pdPASS)
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
		xStatus = xQueueSend(RFID_Ctrl_Queue, &CtrlqueueData_tx, pdMS_TO_TICKS(200));
		if (xStatus != pdPASS) {
			printf("RFID CtrlqueueData_tx fail");
		}


     vTaskDelay(20);
	}

}




