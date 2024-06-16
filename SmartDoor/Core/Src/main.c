/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */



/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

static TaskHandle_t AppTaskCreate_Handle;


//Task Handle
static TaskHandle_t DoorCtrlTask_Handle;
static TaskHandle_t RFIDTask_Handle;
static TaskHandle_t FPTask_Handle;
static TaskHandle_t xMotorTask_Handle;

//Queue Handle
//QueueHandle_t Ctrl_RFID_Queue;
//QueueHandle_t RFID_Ctrl_Queue;
QueueHandle_t Ctrl_FP_Queue;
QueueHandle_t FP_Ctrl_Queue;
QueueHandle_t Ctrl_Motor_Queue;

QueueData gCtrl_RFID;

uint8_t data;
uint8_t status;
uint8_t str[MAX_LEN]; // Max_LEN = 16
uint8_t sNum[5];
uint8_t fp_activate;

uint8_t storedRFIDCards[MAX_CARDS][RFID_CARD_LENGTH];
uint8_t EmptyRFIDCards[RFID_CARD_LENGTH];

SemaphoreHandle_t xKeyScanSemaphore;

char press_key;


//static TaskHandle_t IC_Task_Handle;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
	HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
	return ch;
}



static void Queue_Init(void)
{
	QueueData QueueData_init;

//	Ctrl_RFID_Queue = xQueueCreate(QUEUE_LENGTH, sizeof(QueueData_init));
//	if (Ctrl_RFID_Queue == NULL) {
//		printf("Ctrl_RFID_Queue create fail \r \n");
//	}
//
//	RFID_Ctrl_Queue = xQueueCreate(QUEUE_LENGTH, sizeof(QueueData_init));
//	if (RFID_Ctrl_Queue == NULL) {
//		printf("RFID_Ctrl_Queue create fail \r \n");
//	}


	Ctrl_FP_Queue = xQueueCreate(QUEUE_LENGTH, sizeof(QueueData_init));
	if (Ctrl_FP_Queue == NULL) {
		printf("Ctrl_FP_Queue create fail \r \n");
	}

	FP_Ctrl_Queue = xQueueCreate(QUEUE_LENGTH, sizeof(QueueData_init));
	if (FP_Ctrl_Queue == NULL) {
		printf("FP_Ctrl_Queue create fail \r \n");
	}

	Ctrl_Motor_Queue = xQueueCreate(QUEUE_LENGTH, sizeof(QueueData_init));
	if (Ctrl_Motor_Queue == NULL) {
		printf("Ctrl_Motor_Queue create fail \r \n");
	}
}


static void BSP_Init(void)
{

	OLED_Init();
	OLED_Clear();
	OLED_Update();

}



/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


uint16_t GetCCRFromAngle(float InputAngle){
    float Ret=InputAngle/180*2000+500;
    return Ret;
}

void MotorTask( void ){


	QueueData MotorqueueData_rx;


    for(;;){

//		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
//		vTaskDelay(200);

        if(xQueueReceive(Ctrl_Motor_Queue, &(MotorqueueData_rx), pdMS_TO_TICKS(50)))
        {

            if(MotorqueueData_rx.systemmode == OPEN) { // Forward 180 degrees
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, GetCCRFromAngle(180));
//				vTaskDelay(50);
	            printf("MotorTask OPEN \n");
            }
            else if(MotorqueueData_rx.systemmode == CLOSE)
            { // Backward 180 degrees
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, GetCCRFromAngle(0));
//				vTaskDelay(50);
				printf("MotorTask CLOSE \n");
            }
        }
        vTaskDelay(500);


    }

}

static void AppTaskCreate(void)
{
	BaseType_t xReturn = pdPASS;
	taskENTER_CRITICAL();



	xReturn = xTaskCreate((TaskFunction_t	)DoorCtrlTask,
															(const char* 	)"DoorCtrlTask",
															(uint32_t 		)128,
															(void* 		  	)NULL,
															(UBaseType_t 	)4,
															(TaskHandle_t*  )&DoorCtrlTask_Handle);

	if(pdPASS == xReturn)
		printf("DoorCtrlTask Task Create Successful!\r\n");

//	xReturn = xTaskCreate((TaskFunction_t	)RFIDTask,
//															(const char* 	)"RFIDTask",
//															(uint32_t 		)128,
//															(void* 		  	)NULL,
//															(UBaseType_t 	)4,
//															(TaskHandle_t*  )&RFIDTask_Handle);
//	if(pdPASS == xReturn)
//		printf("RFIDTask Task Create Successful!\r\n");

//	xReturn = xTaskCreate((TaskFunction_t	)FPTask,
//															(const char* 	)"FPTask",
//															(uint32_t 		)128,
//															(void* 		  	)NULL,
//															(UBaseType_t 	)4,
//															(TaskHandle_t*  )&FPTask_Handle);
//	if(pdPASS == xReturn)
//		printf("FPTask Task Create Successful!\r\n");

	xReturn = xTaskCreate((TaskFunction_t	)MotorTask,
															(const char* 	)"MotorTask",
															(uint32_t 		)128,
															(void* 		  	)NULL,
															(UBaseType_t 	)3,
															(TaskHandle_t*  )&xMotorTask_Handle);
	if(pdPASS == xReturn)



  vTaskDelete(AppTaskCreate_Handle);

  taskEXIT_CRITICAL();            //???��?��??��?
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_USART3_UART_Init();
  MX_TIM2_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  BaseType_t xReturn = pdPASS;

  MFRC522_Init();
  InitStoredRFIDCards();
  BSP_Init();

  HAL_TIM_Base_Start_IT(&htim6);
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);

  xKeyScanSemaphore = xSemaphoreCreateBinary();

  printf("BSP Init Done!\r\n");

  Queue_Init();
  printf("Queue Init Done!\r\n");

  xReturn = xTaskCreate((TaskFunction_t	)AppTaskCreate,
														(const char* 	)"AppTaskCreate",
														(uint32_t 		)128,
														(void* 		  	)NULL,
														(UBaseType_t 	)4,
														(TaskHandle_t*  )&AppTaskCreate_Handle);

	if(NULL != AppTaskCreate_Handle)
	vTaskStartScheduler();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM7 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM7) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  if (htim->Instance == TIM6) {
//		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);

    	press_key = KEY_SCAN();

		if (press_key != '\0')
		{
			printf("press_key = %c \r\n",press_key);

			BaseType_t xHigherPriorityTaskWoken;
			xHigherPriorityTaskWoken = pdFALSE;
			xSemaphoreGiveFromISR(xKeyScanSemaphore,&xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}


	   status = MFRC522_Request(PICC_REQIDL, str);
	   if (status == MI_OK)
	   {
		   status = MFRC522_Anticoll(str);
	   }


	   if(fp_activate > 50)
	   {
		   fp_activate = 0;
	   }
	   else
	   {
		   fp_activate++;
//		   printf("fp_activate : %u \r\n",fp_activate );
	   }


//		status = MFRC522_Request(PICC_REQIDL, str);
//		status = MFRC522_Anticoll(str);
//		if (status == MI_OK)
//		{
//			BaseType_t xHigherPriorityTaskWoken;
//			xHigherPriorityTaskWoken = pdFALSE;
//			xSemaphoreGiveFromISR(xRFIDSemaphore,&xHigherPriorityTaskWoken);
//			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//		}

//		if (press_key != '\0')
//		{
//			BaseType_t xHigherPriorityTaskWoken;
//			xHigherPriorityTaskWoken = pdFALSE;
//			xSemaphoreGiveFromISR(xKeyScanSemaphore,&xHigherPriorityTaskWoken);
//			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//		}


//	    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//	    if (xSemaphoreGiveFromISR(xKeyScanSemaphore, &xHigherPriorityTaskWoken) == pdTRUE) {
//	      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//	    }
  }

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
