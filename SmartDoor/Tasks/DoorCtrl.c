#include "../Tasks/DoorCtrl.h"



extern QueueHandle_t Ctrl_RFID_Queue;
extern QueueHandle_t RFID_Ctrl_Queue;
extern QueueHandle_t FPQueue;

extern QueueHandle_t pxRFID_Ctrl_Queue;

extern QueueData gCtrl_RFID;

SystemMode_t Mode;
uint8_t submode;

char press_key;
char Password[PASSWORD_LENGTH];
char Password2[PASSWORD_LENGTH];
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

void DoorCtrlTask(void* parameter)
{
	Mode = MANAGE_CLEAR;
	press_key = '\0';
	strcpy(Default_Password, "123");
	strcpy(Manage_Password, "000");
	int SubManagePage = 0;
	int SubChangePWPage = 0;
	int SubNewRFIDPage = 0;
	int SubDelRFIDPage = 0;
	int Sel_Item;
	int Data = 0;
	int check_idx = -1;
	uint8_t new_cmd,del_cmd;


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


	for(;;)
	{


//		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
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

	    	OLED_ShowChinese(32,24, "智慧門鎖");
	    	OLED_Clear();
	    	OLED_Update();
//	    	vTaskDelay(200);

	    	Mode = CLOSE_CLEAR;


	        break;

	    case CLOSE_CLEAR:

	    	OLED_Clear();
	    	OLED_ShowChinese(0, 0, "輸入密碼卡片指紋");
	    	OLED_ShowChinese(0, 48, "＊確認　　＃清除");
	    	OLED_Update();
	    	Mode = CLOSE;

	    	break;


	    case CLOSE:

//	    	OLED_ShowChinese(0, 0, "輸入密碼卡片指紋");
	    	/* Key password */

//	    	OLED_ShowChinese(0, 48, "＊確認　　＃清除");

//	    	press_key = KEY_SCAN();
//	    	printf("press_key = %c \r\n",press_key);

//	        if (xSemaphoreTake(xKeyScanSemaphore, pdMS_TO_TICKS(100)) == pdTRUE)
//	        {
	            // 執行按鍵掃描
	        	press_key = KEY_SCAN();
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
					OLED_Update();
				}
//	        }

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
//	        xStatus = xQueueSend(Ctrl_RFID_Queue, &RFIDqueueData_tx, pdMS_TO_TICKS(100));
//	        if (xStatus != pdPASS) {
//	            // Fail
//	            // ...
//	        }
//	        RFIDqueueData_tx.cmd.start = 1;

	        /* RFID -> DoorCtrl */

//	        configASSERT(RFID_Ctrl_Queue != NULL);
//
//	        UBaseType_t uxMessagesWaiting = uxQueueMessagesWaiting(RFID_Ctrl_Queue);
//	        printf("Messages in queue before send: %d\r\n", uxMessagesWaiting);

//	        portMAX_DELAY
//			pdMS_TO_TICKS(100)
//			if(xQueueReceive(RFID_Ctrl_Queue, &RFIDqueueData_rx, pdMS_TO_TICKS(100))== pdPASS)
//			{
//				printf("RFIDqueueData_rx pass in DoorCtrl --  mode CLOSE \r\n");
//				if(RFIDqueueData_rx.result.pass == 1)
//				{
//					Mode = CHECK_RFID;
////					RFIDqueueData.cmd.start = 0;
//
////					OLED_ShowString(0, 32, "RFID pass ", OLED_8X16);
////
////					if(RFIDqueueData_rx.status == ERROR)
////					{
////						OLED_ShowString(0, 48, "RFID ERROR ", OLED_8X16);
////					}
//				}
//
//			}


//			status = MFRC522_Request(PICC_REQIDL, str);
//			status = MFRC522_Anticoll(str);
//			if (status == MI_OK)
//			{
//				memcpy(sNum, str, 5);
//				vTaskDelay(100);
//				printf("Card UID: %02X %02X %02X %02X %02X\r\n", sNum[0], sNum[1], sNum[2], sNum[3], sNum[4]);
//
//				if((str[0]==67) && (str[1]==4) && (str[2]==23) && (str[3]==20) && (str[4]==68) )
//				{
//
//				  gCtrl_RFID.result.pass = 1;
//
//
//				}
//				else if((str[0]==243) && (str[1]==188) && (str[2]==5) && (str[3]==53) && (str[4]==127) )
//				{
//
//				  gCtrl_RFID.result.pass = 1;
//
//				}
//				memset(str, 0, sizeof(str));
//			}


			status = MFRC522_Request(PICC_REQIDL, str);
			if (status == MI_OK)
			{
			   status = MFRC522_Anticoll(str);
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


			/* DoorCtrl -> FP */


			/* FP -> DoorCtrl */

	    	OLED_Update();
	        break;


	    case CHECK_PW:

			if (strcmp(Password, Default_Password) == 0) {
				printf("Password is correct.\r\n");

				OLED_ClearArea(0, 16, 128, 16);
				OLED_ShowChinese(0, 16, "密碼正確");
				OLED_Update();
//				vTaskDelay(500);
				Mode = OPEN_CLEAR;
			} else {
				printf("Password is incorrect.\r\n");

				OLED_ClearArea(0, 16, 128, 16);
				OLED_ShowChinese(0, 16, "密碼錯誤");
				OLED_Update();
//				vTaskDelay(500);
				Mode = CLOSE_CLEAR;
			}
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
//				OLED_Update();
//				vTaskDelay(500);
//				Mode = CLOSE_CLEAR;
//			}
			OLED_Clear();
			OLED_Update();

	    	break;

	    case CHECK_FP:

			if (RDID_pass == 1) {
				printf("Finger Print pass.\r\n");

				OLED_ClearArea(0, 16, 128, 16);
				OLED_ShowChinese(0, 16, "指紋正確");
				OLED_Update();
				vTaskDelay(500);
				Mode = OPEN_CLEAR;
			} else {
				printf("Finger Print fail.\r\n");

				OLED_ClearArea(0, 16, 128, 16);
				OLED_ShowChinese(0, 16, "指紋錯誤");
				OLED_Update();
				vTaskDelay(500);
				Mode = CLOSE_CLEAR;
			}
			OLED_Clear();
			OLED_Update();

	    	break;


	    case OPEN_CLEAR:

			memset(Password, '\0', PASSWORD_LENGTH);
	    	OLED_Clear();
	    	OLED_Update();
	    	Mode = OPEN;

	    	break;


	    case OPEN:

//	        uxMessagesWaiting = uxQueueMessagesWaiting(RFID_Ctrl_Queue);
//	        printf("Messages in queue before send: %d\r\n", uxMessagesWaiting);

//			if(xQueueReceive(RFID_Ctrl_Queue, &RFIDqueueData_rx, pdMS_TO_TICKS(100))== pdPASS)
//			{
//				printf("RFIDqueueData_rx pass in DoorCtrl --  mode OPEN \r\n");
//
////					RFIDqueueData.cmd.start = 0;
//
////					OLED_ShowString(0, 32, "RFID pass ", OLED_8X16);
////
////					if(RFIDqueueData_rx.status == ERROR)
////					{
////						OLED_ShowString(0, 48, "RFID ERROR ", OLED_8X16);
////					}
//
//
//			}

//	    	OLED_ShowString(0, 0, "Mode: OPEN", OLED_8X16);
//	    	OLED_ShowString(0, 0, "Input Manage PW", OLED_8X16);
	    	OLED_ShowChinese(0, 0, "輸入管理密碼");

	    	/* Key password */
//	    	OLED_ShowString(0, 48, "Enter A  Clean B", OLED_8X16);
	    	OLED_ShowChinese(0, 32, "　　　　　Ａ關閉");
	    	OLED_ShowChinese(0, 48, "＊確認　　＃清除");
	    	press_key = KEY_SCAN();

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

	    	Sel_Item = 0;

	    	OLED_Clear();
	    	OLED_Update();
	    	Mode = MANAGE;

	    	break;


	    case MANAGE:


	    	OLED_ShowChinese(0, 48, "＊確認　Ｃ上Ｄ下");
	    	press_key = KEY_SCAN();

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
						Mode = NEW_FP;
					}
					else if (Sel_Item == 4) /* DEL_FP */
					{
						Mode = DEL_FP;
					}
					else if (Sel_Item == 5) /* CLOSE */
					{
						Mode = CLOSE_CLEAR;
					}

				}

				OLED_ClearArea(0, 0, 128, 48);
	    	}

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

//	    	printf("Sel_Item = %d", Sel_Item);
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
	        break;

	    case CHANGE_PW:

			switch (SubChangePWPage)
			{
				case 0:
					OLED_Clear();
					SubChangePWPage = 1;
					break;

				case 1:
					OLED_ShowChinese(0, 0, "輸入第一次新密碼");

			    	press_key = KEY_SCAN();
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
				    			SubChangePWPage = 2;
								OLED_ClearArea(0, 16, 128, 16);
				    		}
			    		}

						OLED_ShowString(0, 16, Password, OLED_8X16);
						printf("New 1 Password = %s \r\n",Password);
			    	}

					break;
				case 2:
					OLED_ShowChinese(0, 0, "輸入第二次新密碼");

			    	press_key = KEY_SCAN();
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
					break;




			/* Key password */
			//	    	OLED_ShowString(0, 48, "Enter A  Clean B", OLED_8X16);
//			OLED_ShowChinese(0, 48, "＊確認　　＃清除");


			}
			OLED_ShowChinese(0, 48, "＊確認　　＃清除");


			OLED_Update();
			 break;

	    case NEW_RFID:

				switch (SubNewRFIDPage)
				{
					case 0:
						OLED_Clear();
						SubNewRFIDPage = 1;
						break;

					case 1:
						/* check RFID */

					    OLED_ShowChinese(0, 0, "請接觸卡片");
						OLED_ShowChinese(0, 48, "＊確認　　＃退出");

						press_key = KEY_SCAN();
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
									new_cmd = 1;
								}
								else if(press_key == '#')
								{
//									Mode = MANAGE_CLEAR;
									SubNewRFIDPage = 3;
								}
							}
						}
						status = MFRC522_Request(PICC_REQIDL, str);
					   if (status == MI_OK)
					   {
						   status = MFRC522_Anticoll(str);
						   if (status == MI_OK)
						   {
							   memcpy(sNum, str, 5);
							   check_idx = CheckCardSerialNumber(sNum);
							   OLED_ClearArea(0, 16, 128, 16);
							   if (check_idx == -1 )
							   {
								   printf("Card not enroll yet.\r\n");
		//						   OLED_ShowChinese(0, 0, "can enroll");
//								   OLED_ShowString(0, 0, "can enroll",OLED_8X16);
								   OLED_ShowChinese(0, 16, "未注冊　可新增");
								   if(new_cmd == 1)
								   {
									   new_cmd = 0;
									   OLED_Clear();
									   SubNewRFIDPage = 2;
								   }
							   }
							   else
							   {
								   printf("Card enrolled \r\n");
//								   OLED_ShowString(0, 0, "enrolled",OLED_8X16);
								   OLED_ShowChinese(0, 16, "已注冊　不可新增");
							   }
						   }
					   }
					   new_cmd = 0;

						break;

					case 2:
			    		check_idx = CheckEmptyIdxStoredRFIDCards();
			    		printf("check_idx = %d", check_idx );
			    		printf("sNum = %d", sNum );
						if (check_idx != -1)
						{
							if (StoreCardSerialNumber(check_idx, sNum) == MI_OK)
							{
//								OLED_ShowString(0, 12, "new OK",OLED_8X16);
								OLED_ShowChinese(0, 0, "新增卡片成功");
							}
							else
							{
//								OLED_ShowString(0, 12, "new fail",OLED_8X16);
								OLED_ShowChinese(0, 0, "新增卡片失敗");
							}
						}
						else
						{
//							OLED_ShowString(0, 12, "no space",OLED_8X16);
							OLED_ShowChinese(0, 0, "注冊已滿");
						}
						vTaskDelay(500);
						SubNewRFIDPage = 3;

						break;

					case 3:
						OLED_Clear();
			    		Mode = MANAGE_CLEAR;
						break;
				}

			   OLED_Update();

			 break;


	    case DEL_RFID:

			switch (SubDelRFIDPage)
			{
				case 0:
					OLED_Clear();
					SubDelRFIDPage = 1;
					break;

				case 1:
					/* check RFID */

				    OLED_ShowChinese(0, 0, "請接觸卡片");
					OLED_ShowChinese(0, 48, "＊確認　　＃退出");

					press_key = KEY_SCAN();
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
							}
							else if(press_key == '#')
							{
//									Mode = MANAGE_CLEAR;
								SubDelRFIDPage = 3;
							}
						}
					}
					status = MFRC522_Request(PICC_REQIDL, str);
				   if (status == MI_OK)
				   {
					   status = MFRC522_Anticoll(str);
					   if (status == MI_OK)
					   {
						   memcpy(sNum, str, 5);
						   check_idx = CheckCardSerialNumber(sNum);
						   OLED_ClearArea(0, 16, 128, 16);
						   if (check_idx != -1 )
						   {
							   printf("Card enrolled .\r\n");
							   OLED_ShowChinese(0, 16, "已注冊　可刪除");
							   if(del_cmd == 1)
							   {
								   del_cmd = 0;
								   OLED_Clear();
								   SubDelRFIDPage = 2;
							   }
						   }
						   else
						   {
							   printf("Card not enrolled \r\n");
//								   OLED_ShowString(0, 0, "enrolled",OLED_8X16);
							   OLED_ShowChinese(0, 16, "未注冊　不可刪除");
						   }
					   }
				   }
				   del_cmd = 0;

					break;

				case 2:

					if (check_idx != -1)
					{
						if (DeleteCardSerialNumber(check_idx) == MI_OK)
						{
//								OLED_ShowString(0, 12, "new OK",OLED_8X16);
							OLED_ShowChinese(0, 0, "刪除卡片成功");
						}
						else
						{
//								OLED_ShowString(0, 12, "new fail",OLED_8X16);
							OLED_ShowChinese(0, 0, "刪除卡片失敗");
						}
					}

					vTaskDelay(500);
					SubDelRFIDPage = 3;

					break;

				case 3:
					OLED_Clear();
		    		Mode = MANAGE_CLEAR;
					break;
			}

			OLED_Update();
			 break;
	    case NEW_FP:

			 break;
	    case DEL_FP:

			 break;



	    default:
	        break;


		}


//		uxMessagesWaiting =  uxQueueMessagesWaiting(pxRFID_Ctrl_Queue);
//		printf("Messages in queue before send: %d\r\n", uxMessagesWaiting);

//        if(uxQueueMessagesWaiting(pxRFID_Ctrl_Queue) > 0)
//        {
//	//		if(pxRFIDqueueData_rx)
//			if(xQueueReceive(pxRFID_Ctrl_Queue, &pxRFIDqueueData_rx, pdMS_TO_TICKS(100))== pdPASS)
//			{
////				RFIDqueueData_rx = *pxRFIDqueueData_rx;
//				printf("RFIDqueueData_rx pass in DoorCtrl \r\n");
//			}
//			else if (xQueueReceive(pxRFID_Ctrl_Queue, &pxRFIDqueueData_rx, pdMS_TO_TICKS(100))== pdFALSE)
//			{
//				printf("RFIDqueueData_rx fail in DoorCtrl \r\n");
//			}
//        }
//		else if (xQueueReceive(pxRFID_Ctrl_Queue, &pxRFIDqueueData_rx, pdMS_TO_TICKS(100))== pdFALSE)
//		{
//			printf("RFIDqueueData_rx fail in DoorCtrl \r\n");
//		}

//		vTaskDelay(50);



//		if(xQueueReceive(pxRFID_Ctrl_Queue, &pxRFIDqueueData_rx, pdMS_TO_TICKS(500))== pdPASS)
//		{
////			RFIDqueueData_rx = *pxRFIDqueueData_rx;
//			printf("RFIDqueueData_rx pass in DoorCtrl \r\n");
//		}
//		else if (xQueueReceive(pxRFID_Ctrl_Queue, &pxRFIDqueueData_rx, pdMS_TO_TICKS(500))== pdFALSE)
//		{
//			printf("RFIDqueueData_rx fail in DoorCtrl \r\n");
//		}
		vTaskDelay(200);
//		Taskmonitor();
	}


}



