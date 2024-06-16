#include "../Tasks/DoorCtrl.h"



extern QueueHandle_t Ctrl_RFID_Queue;
extern QueueHandle_t RFID_Ctrl_Queue;
extern QueueHandle_t Ctrl_Motor_Queue;
extern QueueHandle_t FPQueue;

extern QueueHandle_t pxRFID_Ctrl_Queue;

extern QueueData gCtrl_RFID;


SystemMode_t Mode;
uint8_t submode;

extern char press_key;
char Password[PASSWORD_LENGTH];
char Password2[PASSWORD_LENGTH];
char FP_ID[2];
char Default_Password[PASSWORD_LENGTH];
char Manage_Password[PASSWORD_LENGTH];
char *ItemListCH[]= {
        "修改密碼",
        "新增卡片",
        "刪除卡片",
        "新增指紋",
        "刪除指紋",
		"關閉　　",
    };
char Item_test[] ="修改密碼";

int RDID_pass;

QueueData RFIDqueueData_rx;
QueueData  *pxRFIDqueueData_rx;
BaseType_t xStatus;


extern uint8_t data;
extern uint8_t status;
extern uint8_t str[MAX_LEN]; // Max_LEN = 16
extern uint8_t sNum[5];
extern uint8_t storedRFIDCards[MAX_CARDS][RFID_CARD_LENGTH];

extern uint8_t fp_activate;


extern SemaphoreHandle_t xKeyScanSemaphore;
extern SemaphoreHandle_t xRFIDSemaphore;

void DoorCtrlTask(void* parameter)
{
	Mode = INIT;
//	Mode = MANAGE_CLEAR;
	press_key = '\0';
	strcpy(Default_Password, "123");
	strcpy(Manage_Password, "000");
	uint8_t SubManagePage = 0;
	uint8_t SubChangePWPage = 0;
	uint8_t SubNewRFIDPage = 0;
	uint8_t SubDelRFIDPage = 0;
	uint8_t SubNewFPPage = 0;
	uint8_t SubDelFPPage = 0;

	int Sel_Item;
	int Data = 0;
	int check_idx = -1;
	uint8_t new_cmd,del_cmd;

//	int test = 0;

	int fail = 0;

	int set_ID;

	UBaseType_t uxMessagesWaiting;

    QueueData queueData;
//    QueueData RFIDqueueData_tx = {
//        .cmd = {0},
//        .result = {0},
//		.systemmode = INIT,
//		.status = Idle
//    };
//
//    QueueData RFIDqueueData_rx = {
//        .cmd = {0},
//        .result = {0},
//		.systemmode = INIT,
//		.status = Idle
//    };
	QueueData MotorqueueData_tx = {
		.cmd = {0},
		.systemmode = {0},
	};


	for(;;)
	{


		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
//		vTaskDelay(200);
//		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET );
//		vTaskDelay(100);
//
//		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET );
//		vTaskDelay(100);

//        if (xSemaphoreTake(xKeyScanSemaphore, pdMS_TO_TICKS(100)) == pdTRUE)
//        {
//        	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
//        }

//		RFIDqueueData.systemmode = Mode;
		switch (Mode)
		{

	    case INIT:
//	    	pw_fail_flag = 1;

	    	OLED_ShowChinese(0,24, "智慧門鎖");
	    	OLED_Clear();
	    	OLED_Update();
	    	vTaskDelay(2000);

	    	Mode = CLOSE_CLEAR;


	        break;

	    case CLOSE_CLEAR:

	    	OLED_Clear();
	    	OLED_ShowChinese(0, 0, "輸入密碼卡片指紋");
	    	OLED_ShowChinese(0, 48, "＊確認　　＃清除");
	    	OLED_Update();

			MotorqueueData_tx.systemmode = CLOSE;
			xStatus = xQueueSend(Ctrl_Motor_Queue, &MotorqueueData_tx, pdMS_TO_TICKS(100));
			if (xStatus != pdPASS) {
				// Fail
				// ...
			}
	    	Mode = CLOSE;

	    	break;


	    case CLOSE:

//	    	OLED_ShowChinese(0, 0, "輸入密碼卡片指紋");
	    	/* Key password */

//	    	OLED_ShowChinese(0, 48, "＊確認　　＃清除");

//	    	press_key = KEY_SCAN();
//	    	printf("press_key = %c \r\n",press_key);

	        if (xSemaphoreTake(xKeyScanSemaphore, pdMS_TO_TICKS(500)) == pdTRUE)
	        {
	            // 執行按鍵掃描
//	        	press_key = KEY_SCAN();
				if (press_key != '\0')
				{

					if(press_key != 'A' && press_key != 'B'
							&& press_key != 'C'&& press_key != 'D'
							&& press_key != '#'&& press_key != '*')
					{
						  printf("press_key = %c \r\n",press_key);
						  appendChar(Password,press_key,sizeof(Password));
					}
					else
					{
						if(press_key == '#')
						{
							  removeChar(Password);
							  OLED_ClearArea(0, 16, 128, 16);
						}
						else if(press_key == '*')
						{
							/* check password */
							Mode = CHECK_PW;

						}

					}

					OLED_ShowString(0, 16, Password, OLED_8X16);
					printf("Password = %s \r\n",Password);
//					OLED_Update();
				}
				vTaskDelay(50);
				OLED_Update();
	        }


	    	/* RFID password */
			/* *****************************
			 * Queue of RFID
			 * DoorCtrl -> RFID
			 * Queue : Ctrl_RFID_Queue
			 * QueueData : RFIDqueueData_tx
			 * RFID -> DoorCtrl
			 * Queue : RFID_Ctrl_Queue
			 * QueueData : RFIDqueueData_rx
			 ******************************* */


	    	/* DoorCtrl -> RFID */

//			status = MFRC522_Request(PICC_REQIDL, str);
//			status = MFRC522_Anticoll(str);
			if (status == MI_OK)
			{

			   memcpy(sNum, str, 5);
			   vTaskDelay(100);
			   printf("Card UID: %02X %02X %02X %02X %02X\r\n", sNum[0], sNum[1], sNum[2], sNum[3], sNum[4]);

			   check_idx = CheckCardSerialNumber(sNum);
			   if (check_idx != -1)
			   {
				   HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
				   vTaskDelay(100);
				   HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
				   gCtrl_RFID.result.pass = 1;
			   }
			}


			if(gCtrl_RFID.result.pass == 1)
			{
				Mode = CHECK_RFID;
			}




	    	/* FingerPrint password */
			/* *****************************
			 * Queue of FingerPrint
			 * DoorCtrl -> FP
			 * Queue : Ctrl_FP_Queue
			 * QueueData : FPqueueData_tx
			 * FP -> DoorCtrl
			 * Queue : FP_Ctrl_Queue
			 * QueueData : FPqueueData_rx
			 ******************************* */

			/*  check fp   */
			if(fp_activate > 5)
			{
				fail = search_fingerprint(&huart2);
				vTaskDelay(100);
				if (fail)
				{
//					Mode = CHECK_FP;
				}
				else
				{
					Mode = CHECK_FP;
				}
	//			HAL_Delay(1000);
				fp_activate = 0;
			}



//	    	OLED_Update();
	        break;


	    case CHECK_PW:


//			if (strcmp(Password, Manage_Password) == 0) {
//				printf("Password is correct.\r\n");
//
//				OLED_ClearArea(0, 16, 128, 16);
//				OLED_ShowChinese(0, 16, "管理密碼正確");
//				OLED_Update();
//				vTaskDelay(500);
//				Mode = MANAGE_CLEAR;
//			} else {
//				printf("Password is incorrect.\r\n");
//
//				OLED_ClearArea(0, 16, 128, 16);
//				OLED_ShowChinese(0, 16, "管理密碼錯誤");
//				OLED_Update();
//				vTaskDelay(500);
//				Mode = OPEN;
//			}



			if (strcmp(Password, Default_Password) == 0) {
				printf("Password is correct.\r\n");

				OLED_ClearArea(0, 16, 128, 16);
				OLED_ShowChinese(0, 16, "密碼正確");
				OLED_Update();
				vTaskDelay(500);

//				MotorqueueData_tx.systemmode = OPEN;
//				xStatus = xQueueSend(Ctrl_Motor_Queue, &MotorqueueData_tx, pdMS_TO_TICKS(100));
//				if (xStatus != pdPASS) {
//					// Fail
//					// ...
//				}


				vTaskDelay(500);
				Mode = OPEN_CLEAR;
			} else {
				printf("Password is incorrect.\r\n");

				OLED_ClearArea(0, 16, 128, 16);
				OLED_ShowChinese(0, 16, "密碼錯誤");
				OLED_Update();
				vTaskDelay(500);
				Mode = CLOSE_CLEAR;
			}

			memset(Password, '\0', PASSWORD_LENGTH);
			OLED_Clear();
			OLED_Update();

	    	break;

	    case CHECK_RFID:

			if (gCtrl_RFID.result.pass == 1)
			{
				printf("RDID pass.\r\n");
				gCtrl_RFID.result.pass = 0;
				OLED_ClearArea(0, 16, 128, 16);
				OLED_ShowChinese(0, 16, "卡片正確");

				OLED_Update();
				vTaskDelay(500);
				Mode = OPEN_CLEAR;
			}
//			else if (gCtrl_RFID.result.fail == 1)
//			{
//				printf("RDID fail.\r\n");
//
//				OLED_ClearArea(0, 16, 128, 16);
//				OLED_ShowChinese(0, 16, "卡片錯誤");

//				vTaskDelay(500);
//				Mode = CLOSE_CLEAR;
//			}
			OLED_Clear();
			OLED_Update();

	    	break;

	    case CHECK_FP:


			if (fail)
			{
				printf("FP fail.\r\n");
				OLED_ClearArea(0, 16, 128, 16);
				OLED_ShowChinese(0, 16, "指紋錯誤");
				OLED_Update();
				vTaskDelay(500);
				Mode = CLOSE_CLEAR;
			}
			else
			{
				printf("FP pass.\r\n");
				OLED_ClearArea(0, 16, 128, 16);
				OLED_ShowChinese(0, 16, "指紋正確");
				OLED_Update();
				vTaskDelay(500);
				Mode = OPEN_CLEAR;
			}



	    	break;


	    case OPEN_CLEAR:



			MotorqueueData_tx.systemmode = OPEN;
			xStatus = xQueueSend(Ctrl_Motor_Queue, &MotorqueueData_tx, pdMS_TO_TICKS(100));
			if (xStatus != pdPASS) {
				// Fail
				// ...
			}

			memset(Password, '\0', PASSWORD_LENGTH);

	    	OLED_Clear();
	    	OLED_ShowChinese(0, 0, "輸入管理密碼");
	    	OLED_ShowChinese(0, 32, "　　　　　Ａ關閉");
	    	OLED_ShowChinese(0, 48, "＊確認　　＃清除");
	    	OLED_Update();

	    	Mode = OPEN;

	    	break;


	    case OPEN:

	        if (xSemaphoreTake(xKeyScanSemaphore, pdMS_TO_TICKS(500)) == pdTRUE)
	        {

				if (press_key != '\0')
				{

					if(press_key != 'A' && press_key != 'B'
							&& press_key != 'C'&& press_key != 'D'
							&& press_key != '#'&& press_key != '*')
					{
						  printf("press_key = %c \r\n",press_key);
						  appendChar(Password,press_key,sizeof(Password));
					}
					else
					{
						if(press_key == '#')
						{
							  removeChar(Password);
							  OLED_ClearArea(0, 16, 128, 16);
						}
						else if(press_key == '*')
						{
							/* check password */
							Mode = CHECK_MNPW;

						}
						else if(press_key == 'A')
						{
							memset(Password, '\0', PASSWORD_LENGTH);
							OLED_ClearArea(0, 32, 128, 16);
							Mode = CLOSE_CLEAR;
						}

					}

					OLED_ShowString(0, 16, Password, OLED_8X16);
					printf("Manage Password = %s \r\n",Password);

				}
				OLED_Update();
	        }

	        break;

	    case CHECK_MNPW:

			if (strcmp(Password, Manage_Password) == 0) {
				printf("Password is correct.\r\n");

				OLED_ClearArea(0, 16, 128, 16);
				OLED_ShowChinese(0, 16, "管理密碼正確");
				OLED_Update();
				vTaskDelay(500);
				Mode = MANAGE_CLEAR;
			} else {
				printf("Password is incorrect.\r\n");

				OLED_ClearArea(0, 16, 128, 16);
				OLED_ShowChinese(0, 16, "管理密碼錯誤");
				OLED_Update();
				vTaskDelay(500);
				Mode = OPEN;
			}
			memset(Password, '\0', PASSWORD_LENGTH);
			OLED_Clear();
			OLED_Update();

	    	break;

	    case MANAGE_CLEAR:

	    	OLED_Clear();

	    	OLED_ShowChinese(0, 48, "＊確認　Ｃ上Ｄ下");
	    	Sel_Item = 0;


			for(int i = 0;i<3;i++)
			{
				OLED_ShowChinese(40, 16*i, ItemListCH[i]);
			}

	    	OLED_ShowChinese(Sel_Item, Sel_Item*16, "－＞");

	    	OLED_Update();
	    	Mode = MANAGE;

	    	break;


	    case MANAGE:



	        if (xSemaphoreTake(xKeyScanSemaphore, pdMS_TO_TICKS(500)) == pdTRUE)
	        {
				if (press_key != '\0')
				{
					if(press_key == 'C')/*Up*/
					{
						if(Sel_Item == 0)
						{
							Sel_Item =(sizeof(ItemListCH) / sizeof(ItemListCH[0]))-1;
						}
						else
						{
							Sel_Item--;
						}
					}
					else if(press_key == 'D')/*Down*/
					{
						if(Sel_Item == 5)
						{
							Sel_Item = 0;
						}
						else
						{
							Sel_Item++;
						}

					}
					else if(press_key == '*')/*Enter*/
					{

						if(Sel_Item == 0) /* CHANGE_PASSWORD */
						{
							SubChangePWPage = 0;
							Mode = CHANGE_PW;
						}
						else if (Sel_Item == 1) /* NEW_RFID */
						{
							SubNewRFIDPage = 0;
							Mode = NEW_RFID;
						}
						else if (Sel_Item == 2) /* DEL_RFID */
						{
							SubDelRFIDPage = 0;
							Mode = DEL_RFID;
						}
						else if (Sel_Item == 3) /* NEW_FP */
						{
							SubNewFPPage = 0;
							Mode = NEW_FP;
						}
						else if (Sel_Item == 4) /* DEL_FP */
						{
							SubDelFPPage = 0;
							Mode = DEL_FP;
						}
						else if (Sel_Item == 5) /* CLOSE */
						{
							Mode = CLOSE_CLEAR;
						}

					}

					OLED_ClearArea(0, 0, 128, 48);
				}


		    	OLED_ShowChinese(0, 48, "＊確認　Ｃ上Ｄ下");



		    	if(Sel_Item < 3)
		    	{
		    		SubManagePage = 1;
		    		OLED_ShowChinese(0, Sel_Item*16, "－＞");


		    	}
		    	else if(Sel_Item > 2)
		    	{
		    		SubManagePage = 2;
		    		OLED_ShowChinese(0, (Sel_Item-3)*16, "－＞");

		    	}

				switch (SubManagePage)
				{
					case 0:
						SubManagePage = 1;
						break;

					case 1:
						for(int i = 0;i<3;i++)
						{
							OLED_ShowChinese(40, 16*i, ItemListCH[i]);
						}
						break;

					case 2:
						for(int i = 0;i<3;i++)
						{
							OLED_ShowChinese(40, 16*i, ItemListCH[i+3]);
						}
						break;
				}



				OLED_Update();
	        }

//	    	if(Sel_Item < 3)
//	    	{
//	    		SubManagePage = 1;
//	    		OLED_ShowChinese(0, Sel_Item*16, "－＞");
//
//
//	    	}
//	    	else if(Sel_Item > 2)
//	    	{
//	    		SubManagePage = 2;
//	    		OLED_ShowChinese(0, (Sel_Item-3)*16, "－＞");
//
//	    	}
//
////	    	printf("Sel_Item = %d", Sel_Item);
//			switch (SubManagePage)
//			{
//				case 0:
//					SubManagePage = 1;
//					break;
//
//				case 1:
//					for(int i = 0;i<3;i++)
//					{
//						OLED_ShowChinese(40, 16*i, ItemListCH[i]);
//					}
//					break;
//
//				case 2:
//					for(int i = 0;i<3;i++)
//					{
//						OLED_ShowChinese(40, 16*i, ItemListCH[i+3]);
//					}
//					break;
//			}



//			OLED_Update();
	        break;

	    case CHANGE_PW:

			switch (SubChangePWPage)
			{
				case 0:
					OLED_Clear();
					OLED_ShowChinese(0, 0, "輸入第一次新密碼");
					OLED_ShowChinese(0, 48, "＊確認　　＃清除");
					OLED_Update();
					SubChangePWPage = 1;
					break;

				case 1:
//					OLED_ShowChinese(0, 0, "輸入第一次新密碼");

			        if (xSemaphoreTake(xKeyScanSemaphore, pdMS_TO_TICKS(500)) == pdTRUE)
			        {
						if (press_key != '\0')
						{

							if(press_key != 'A' && press_key != 'B'
									&& press_key != 'C'&& press_key != 'D'
									&& press_key != '#'&& press_key != '*')
							{
								  printf("press_key = %c \r\n",press_key);
								  appendChar(Password,press_key,sizeof(Password));
							}
							else
							{
								if(press_key == '#')
								{
									  removeChar(Password);
									  OLED_ClearArea(0, 16, 128, 16);
								}
								else if(press_key == '*')
								{
									/* change password */
									OLED_ShowChinese(0, 0, "輸入第二次新密碼");
									SubChangePWPage = 2;
									OLED_ClearArea(0, 16, 128, 16);
								}
							}

							OLED_ShowString(0, 16, Password, OLED_8X16);
							printf("New 1 Password = %s \r\n",Password);
						}
						OLED_Update();
			        }

					break;
				case 2:
//					OLED_ShowChinese(0, 0, "輸入第二次新密碼");


			        if (xSemaphoreTake(xKeyScanSemaphore, pdMS_TO_TICKS(500)) == pdTRUE)
			        {
						if (press_key != '\0')
						{

							if(press_key != 'A' && press_key != 'B'
									&& press_key != 'C'&& press_key != 'D'
									&& press_key != '#'&& press_key != '*')
							{
								  printf("press_key = %c \r\n",press_key);
								  appendChar(Password2,press_key,sizeof(Password2));
							}
							else
							{
								if(press_key == '#')
								{
									  removeChar(Password2);
									  OLED_ClearArea(0, 32, 128, 16);
								}
								else if(press_key == '*')
								{
									/* change password */

									if (strcmp(Password, Password2) == 0)
									{
	//									memset(Default_Password, Password, PASSWORD_LENGTH);
										strcpy(Default_Password, Password);
										printf("change Password OK \r\n");
										OLED_Clear();
										OLED_ShowChinese(0, 0, "修改密碼成功");
										OLED_Update();
										vTaskDelay(500);
									}
									else
									{
										printf("change Password fail \r\n");
										OLED_Clear();
										OLED_ShowChinese(0, 0, "修改密碼失敗");
										OLED_Update();
										vTaskDelay(500);
									}
									memset(Password, '\0', PASSWORD_LENGTH);
									memset(Password2, '\0', PASSWORD_LENGTH);

									Mode = MANAGE_CLEAR;


								}
							}
							OLED_ShowString(0, 32, Password2, OLED_8X16);
							printf("New 2 Password = %s \r\n",Password2);
						}
						OLED_Update();
			        }
					break;




			/* Key password */
			//	    	OLED_ShowString(0, 48, "Enter A  Clean B", OLED_8X16);
//			OLED_ShowChinese(0, 48, "＊確認　　＃清除");


			}
//			OLED_ShowChinese(0, 48, "＊確認　　＃清除");


//			OLED_Update();
			 break;

	    case NEW_RFID:


				switch (SubNewRFIDPage)
				{
					case 0:
						OLED_Clear();
					    OLED_ShowChinese(0, 0, "請接觸卡片");
						OLED_ShowChinese(0, 48, "＊確認　　＃退出");
						OLED_Update();
						SubNewRFIDPage = 1;
						break;

					case 1:
						/* check RFID */

				        if (xSemaphoreTake(xKeyScanSemaphore, pdMS_TO_TICKS(500)) == pdTRUE)
				        {

							if (press_key != '\0')
							{
								printf("press_key in NEW_RFID = %c \r\n",press_key);
								if(press_key != 'A' && press_key != 'B'
										&& press_key != 'C'&& press_key != 'D'
										&& press_key != '#'&& press_key != '*')
								{
									  printf("press_key = %c \r\n",press_key);
								}
								else
								{
									if(press_key == '*')
									{
//										if(check_idx == -1 && sNum[0] != 00 )
										if(check_idx == -1 && sNum[0] != 00 )
										{
//										   OLED_Clear();
										   SubNewRFIDPage = 2;
//											printf("new_cmd = 1\r\n");
										}
									}
									else if(press_key == '#')
									{
	//									Mode = MANAGE_CLEAR;
										SubNewRFIDPage = 3;
									}
								}
							}
							OLED_Update();
				        }


//						if (xSemaphoreTake(xRFIDSemaphore, pdMS_TO_TICKS(200)) == pdTRUE)
//						{
//						   memcpy(sNum, str, 5);
//						   check_idx = CheckCardSerialNumber(sNum);
//						   OLED_ClearArea(0, 16, 128, 16);
//						   if (check_idx == -1 )
//						   {
//							   printf("Card not enroll yet.\r\n");
//	//						   OLED_ShowChinese(0, 0, "can enroll");
////								   OLED_ShowString(0, 0, "can enroll",OLED_8X16);
//							   OLED_ShowChinese(0, 16, "未注冊　可新增");
//							   if(new_cmd == 1)
//							   {
//								   new_cmd = 0;
//								   OLED_Clear();
//								   SubNewRFIDPage = 2;
//							   }
//						   }
//						   else
//						   {
//							   printf("Card enrolled \r\n");
////								   OLED_ShowString(0, 0, "enrolled",OLED_8X16);
//							   OLED_ShowChinese(0, 16, "已注冊　不可新增");
//						   }
//						   OLED_Update();
//						}
//						else
//						{
//							new_cmd = 0;
//							OLED_ClearArea(0, 16, 128, 16);
//							OLED_Update();
//						}

//				       status = MFRC522_Request(PICC_REQIDL, str);
//					   if (status == MI_OK)
//					   {
//						   status = MFRC522_Anticoll(str);
//						   if (status == MI_OK)
//						   {
//							   memcpy(sNum, str, 5);
//							   check_idx = CheckCardSerialNumber(sNum);
//							   OLED_ClearArea(0, 16, 128, 16);
//							   if (check_idx == -1 )
//							   {
//								   printf("Card not enroll yet.\r\n");
//		//						   OLED_ShowChinese(0, 0, "can enroll");
////								   OLED_ShowString(0, 0, "can enroll",OLED_8X16);
//								   OLED_ShowChinese(0, 16, "未注冊　可新增");
//								   if(new_cmd == 1)
//								   {
//									   new_cmd = 0;
//									   OLED_Clear();
//									   SubNewRFIDPage = 2;
//								   }
//							   }
//							   else
//							   {
//								   printf("Card enrolled \r\n");
////								   OLED_ShowString(0, 0, "enrolled",OLED_8X16);
//								   OLED_ShowChinese(0, 16, "已注冊　不可新增");
//							   }
//						   }
//					   }
//					   else
//					   {
//						  OLED_ClearArea(0, 16, 128, 16);
//						  new_cmd = 0;
//
//					   }
//					   OLED_Update();

//				       printf("check_idx = %d\r\n",check_idx);
//				       printf("Array in hexadecimal: ");
//				       for (int i = 0; i < 5; i++) {
//				           printf("%02X ", sNum[i]);
//				       }
//				       printf("\n");

					   if (status == MI_OK)
					   {

						   memcpy(sNum, str, 5);
						   check_idx = CheckCardSerialNumber(sNum);
						   OLED_ClearArea(0, 16, 128, 16);
						   vTaskDelay(100);
						   if (check_idx == -1 )
						   {
							   printf("Card not enroll yet.\r\n");
							   OLED_ShowChinese(0, 16, "未注冊　可新增");
						   }
						   else
						   {
							   printf("Card enrolled \r\n");
							   OLED_ShowChinese(0, 16, "已注冊　不可新增");
						   }
						   OLED_Update();
					   }




						break;

					case 2:
						OLED_Clear();
			    		check_idx = CheckEmptyIdxStoredRFIDCards();
			    		printf("check_idx = %d", check_idx );
			    		printf("sNum = %d", sNum );
						if (check_idx != -1)
						{
							if (StoreCardSerialNumber(check_idx, sNum) == MI_OK)
							{
								OLED_ShowChinese(0, 16, "新增卡片成功");
							}
							else
							{
								OLED_ShowChinese(0, 16, "新增卡片失敗");
							}
						}
						else
						{
							OLED_ShowChinese(0, 0, "注冊已滿");
						}
						OLED_Update();
						vTaskDelay(500);
						SubNewRFIDPage = 3;

						break;

					case 3:
						OLED_Clear();
						OLED_Update();
			    		Mode = MANAGE_CLEAR;
						break;
				}



			 break;


	    case DEL_RFID:

			switch (SubDelRFIDPage)
			{
				case 0:

					OLED_Clear();
				    OLED_ShowChinese(0, 0, "請接觸卡片");
					OLED_ShowChinese(0, 48, "＊確認　　＃退出");
					OLED_Update();
					SubDelRFIDPage = 1;
					break;

				case 1:
					/* check RFID */


			        if (xSemaphoreTake(xKeyScanSemaphore, pdMS_TO_TICKS(500)) == pdTRUE)
			        {
						if (press_key != '\0')
						{

							if(press_key != 'A' && press_key != 'B'
									&& press_key != 'C'&& press_key != 'D'
									&& press_key != '#'&& press_key != '*')
							{
								  printf("press_key = %c \r\n",press_key);
							}
							else
							{
								if(press_key == '*')
								{
									del_cmd = 1;

									if(check_idx != -1 )
									{
										SubDelRFIDPage = 2;
									}

								}
								else if(press_key == '#')
								{
	//									Mode = MANAGE_CLEAR;
									SubDelRFIDPage = 3;
								}
							}
						}
			        }



				   if (status == MI_OK)
				   {

					   memcpy(sNum, str, 5);
					   check_idx = CheckCardSerialNumber(sNum);
					   OLED_ClearArea(0, 16, 128, 16);
					   vTaskDelay(100);
					   if (check_idx != -1 )
					   {
						   printf("Card delete .\r\n");
						   OLED_ShowChinese(0, 16, "已注冊　可刪除");
					   }
					   else
					   {
						   printf("Card not delete \r\n");
						   OLED_ShowChinese(0, 16, "未注冊　不可刪除");
					   }
					   OLED_Update();
				   }



//					status = MFRC522_Request(PICC_REQIDL, str);
//				   if (status == MI_OK)
//				   {
//					   status = MFRC522_Anticoll(str);
//					   if (status == MI_OK)
//					   {
//						   memcpy(sNum, str, 5);
//						   check_idx = CheckCardSerialNumber(sNum);
//						   OLED_ClearArea(0, 16, 128, 16);
//						   if (check_idx != -1 )
//						   {
//							   printf("Card enrolled .\r\n");
//							   OLED_ShowChinese(0, 16, "已注冊　可刪除");
//							   if(del_cmd == 1)
//							   {
//								   del_cmd = 0;
//								   OLED_Clear();
//								   SubDelRFIDPage = 2;
//							   }
//						   }
//						   else
//						   {
//							   printf("Card not enrolled \r\n");
////								   OLED_ShowString(0, 0, "enrolled",OLED_8X16);
//							   OLED_ShowChinese(0, 16, "未注冊　不可刪除");
//						   }
//					   }
//				   }

					break;

				case 2:

					OLED_Clear();
					if (check_idx != -1)
					{
						if (DeleteCardSerialNumber(check_idx) == MI_OK)
						{
							OLED_ShowChinese(0, 16, "刪除卡片成功");
						}
						else
						{
							OLED_ShowChinese(0, 16, "刪除卡片失敗");
						}
					}

					OLED_Update();
					vTaskDelay(500);
					SubDelRFIDPage = 3;


					break;

				case 3:

					OLED_Clear();
					OLED_Update();
		    		Mode = MANAGE_CLEAR;
					break;
			}

			 break;


		case NEW_FP:

				printf("SubNewFPPage = %u\r\n",SubNewFPPage );
				switch (SubNewFPPage)
				{
					case 0:
						OLED_Clear();
					    OLED_ShowChinese(0, 0, "請輸入指紋編號");
				    	OLED_ShowChinese(0, 32, "　　　　　Ａ退出");
				    	OLED_ShowChinese(0, 48, "＊確認　　＃清除");
						OLED_Update();
						SubNewFPPage = 1;
						break;

					case 1:
						/* check FP */


				        if (xSemaphoreTake(xKeyScanSemaphore, pdMS_TO_TICKS(500)) == pdTRUE)
				        {

							if (press_key != '\0')
							{

								if(press_key != 'A' && press_key != 'B'
										&& press_key != 'C'&& press_key != 'D'
										&& press_key != '#'&& press_key != '*')
								{
									  printf("press_key = %c \r\n",press_key);
									  appendChar(FP_ID,press_key,sizeof(FP_ID));
								}
								else
								{
									if(press_key == '#')
									{
										  removeChar(FP_ID);
										  OLED_ClearArea(0, 16, 128, 16);
									}
									else if(press_key == '*')
									{
										/* FP ID */
										if (strlen(FP_ID) != 0)
										{
											SubNewFPPage = 2;
											OLED_Clear();
											OLED_ShowChinese(0, 0, "請接觸指紋");
											OLED_ShowChinese(0, 32, "　　　　　Ａ退出");

										}
										else
										{
											printf("FP_ID length is zero.\n");
										}

									}
									else if(press_key == 'A')
									{
										memset(FP_ID, '\0', 2);
										OLED_ClearArea(0, 32, 128, 16);
										SubNewFPPage = 4;
									}

								}

								OLED_ShowString(0, 16, FP_ID, OLED_8X16);
								printf("FP ID = %s \r\n",FP_ID);

							}
							OLED_Update();
				        }

//				        if (xSemaphoreTake(xKeyScanSemaphore, pdMS_TO_TICKS(500)) == pdTRUE)
//				        {
//
//							if (press_key != '\0')
//							{
//								printf("press_key in NEW_RFID = %c \r\n",press_key);
//								if(press_key != 'A' && press_key != 'B'
//										&& press_key != 'C'&& press_key != 'D'
//										&& press_key != '#'&& press_key != '*')
//								{
//									  printf("press_key = %c \r\n",press_key);
//								}
//								else
//								{
//									if(press_key == '*')
//									{
//
//										SubNewFPPage = 2;
//									}
//									else if(press_key == '#')
//									{
//	//									Mode = MANAGE_CLEAR;
//										SubNewFPPage = 4;
//									}
//								}
//							}
//							OLED_Update();
//				        }


						break;

					case 2:
						/* check FP */
						vTaskDelay(200);
						set_ID = FP_ID[0] - '0';

						if(fp_activate > 10)
						{
							fail = register_new_fingerprint(&huart2, set_ID);
							vTaskDelay(100);
							if (fail)
							{
								OLED_ShowChinese(0, 16, "新增指紋失敗");
							}
							else
							{
								SubNewFPPage = 3;
								OLED_Clear();
							}
				//			HAL_Delay(1000);
							fp_activate = 0;
							HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin | LD3_Pin , GPIO_PIN_RESET);
							OLED_Update();
						}


				        if (xSemaphoreTake(xKeyScanSemaphore, pdMS_TO_TICKS(500)) == pdTRUE)
				        {

							if (press_key != '\0')
							{
									if(press_key == 'A')
									{
										OLED_ClearArea(0, 32, 128, 16);
										SubNewFPPage = 4;
									}

							}
							OLED_Update();
				        }





						break;

					case 3:
						OLED_Clear();


						if (fail)
						{
//							OLED_ShowChinese(0, 16, "新增指紋成功");
							OLED_ShowChinese(0, 16, "新增指紋失敗");
						}
						else
						{
//							OLED_ShowChinese(0, 16, "新增指紋失敗");
							OLED_ShowChinese(0, 16, "新增指紋成功");
						}

						OLED_Update();
						vTaskDelay(500);
						SubNewFPPage = 4;

						break;

					case 4:
						OLED_Clear();
						OLED_Update();
			    		Mode = MANAGE_CLEAR;
						break;
				}

			 break;
	    case DEL_FP:
			printf("SubDelFPPage = %u\r\n",SubDelFPPage );
				switch (SubDelFPPage)
				{
					case 0:
						OLED_Clear();
						OLED_ShowChinese(0, 0, "請輸入指紋編號");
						OLED_ShowChinese(0, 32, "　　　　　Ａ退出");
						OLED_ShowChinese(0, 48, "＊確認　　＃清除");
						OLED_Update();
						SubDelFPPage = 1;
						break;

					case 1:
						/* check FP */


						if (xSemaphoreTake(xKeyScanSemaphore, pdMS_TO_TICKS(500)) == pdTRUE)
						{

							if (press_key != '\0')
							{

								if(press_key != 'A' && press_key != 'B'
										&& press_key != 'C'&& press_key != 'D'
										&& press_key != '#'&& press_key != '*')
								{
									  printf("press_key = %c \r\n",press_key);
									  appendChar(FP_ID,press_key,sizeof(FP_ID));
								}
								else
								{
									if(press_key == '#')
									{
										  removeChar(FP_ID);
										  OLED_ClearArea(0, 16, 128, 16);
									}
									else if(press_key == '*')
									{
										/* FP ID */
										if (strlen(FP_ID) != 0)
										{
											SubDelFPPage = 2;
											OLED_Clear();
										} else {
											printf("FP_ID length is zero.\n");
										}

									}
									else if(press_key == 'A')
									{
										memset(FP_ID, '\0', 2);
										OLED_ClearArea(0, 32, 128, 16);
										SubDelFPPage = 4;
									}

								}

								OLED_ShowString(0, 16, FP_ID, OLED_8X16);
								printf("FP ID = %s \r\n",FP_ID);

							}
							OLED_Update();
						}



						break;

					case 2:
						/* del FP */

//						set_ID = (int)FP_ID[0];
						set_ID = FP_ID[0] - '0';

						if(fp_activate > 10)
						{

							if(set_ID == 0 )
							{
								printf("DEL FP ID = %d \r\n",set_ID);
								/*delete all*/
								fail = FP_Empty(&huart2);
								vTaskDelay(100);
								if (fail)
								{
									SubDelFPPage = 5;
								}
								else
								{
									SubDelFPPage = 5;
								}
							}
							else
							{
								printf("DEL FP ID = %d \r\n",set_ID);
								/*delete select ID*/
								fail = FP_Delete(&huart2, set_ID);
								vTaskDelay(100);
								if (fail)
								{
									SubDelFPPage = 3;
								}
								else
								{
									SubDelFPPage = 3;
								}
							}
//							vTaskDelay(100);
//							if (fail)
//							{
//								SubDelFPPage = 3;
//							}
//							else
//							{
//								SubDelFPPage = 3;
//							}
				//			HAL_Delay(1000);
							fp_activate = 0;
						}


						break;

					case 3:
						OLED_Clear();

						if (fail)
						{
							OLED_ShowChinese(0, 16, "刪除指定指紋失敗");
						}
						else
						{
							OLED_ShowChinese(0, 16, "刪除指定指紋成功");
						}

						OLED_Update();
						vTaskDelay(500);
						SubDelFPPage = 4;

						break;

					case 5:
						OLED_Clear();

						if (fail)
						{
							OLED_ShowChinese(0, 16, "刪除全部指紋失敗");
						}
						else
						{
							OLED_ShowChinese(0, 16, "刪除全部指紋成功");
						}

						OLED_Update();
						vTaskDelay(500);
						SubDelFPPage = 4;

						break;

					case 4:
						OLED_Clear();
						OLED_Update();
						Mode = MANAGE_CLEAR;
						break;
				}


			 break;



	    default:

	        break;


		}

		vTaskDelay(100);
	}


}



