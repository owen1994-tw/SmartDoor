#include "DoorCtrl.h"


//extern QueueHandle_t Ctrl_RFID_Queue;
//extern QueueHandle_t RFID_Ctrl_Queue;
extern QueueHandle_t FPQueue;

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

void DoorCtrlTask(void* parameter)
{
	Mode = INIT;
	press_key = '\0';
	strcpy(Default_Password, "123");
	strcpy(Manage_Password, "000");
	int SubManagePage = 0;
	int SubChangePWPage = 0;
	int Sel_Item;
	int Data = 0;

    QueueData queueData;
//    QueueData RFIDqueueData_tx = {
//        .cmd = init,
//        .result = {0},
//		.systemmode = INIT,
//		.status = Idle
//    };
//
//    QueueData RFIDqueueData_rx = {
//        .cmd = init,
//        .result = {0},
//		.systemmode = INIT,
//		.status = Idle
//    };

	QueueData FPqueueData_rx_tx = {
		.cmd = init,
		.result = {0},
		.systemmode = INIT,
		.status = Idle
	};
    QueueData  *pxRFIDqueueData;
    BaseType_t xStatus;


    extern uint8_t data;
    extern uint8_t status;
    extern uint8_t str[MAX_LEN]; // Max_LEN = 16
    extern uint8_t sNum[5];

    uint8_t RFID_pass;

	for(;;)
	{

//		xQueueSend(RFIDQueue, &queueData, portMAX_DELAY);
//		xQueueSend(FPQueue, &queueData, portMAX_DELAY);


//		RFIDqueueData.systemmode = Mode;
		switch (Mode)
		{

	    case INIT:
//	    	pw_fail_flag = 1;

	    	OLED_ShowChinese(32,24, "智慧門鎖");
	    	OLED_Clear();
	    	OLED_Update();
	    	vTaskDelay(200);

	    	Mode = CLOSE_CLEAR;


	        break;

	    case CLOSE_CLEAR:

	    	OLED_Clear();
	    	OLED_Update();
	    	Mode = CLOSE;

	    	break;


	    case CLOSE:
//	    	printf("mode Close");
//	    	OLED_ShowString(0, 0, "Input Password", OLED_8X16);
	    	OLED_ShowChinese(0, 0, "輸入密碼卡片指紋");
//	    	OLED_ShowString(0, 16, "or RFID/FP", OLED_8X16);


	    	/* Key password */
//	    	OLED_ShowString(0, 48, "  Clean B", OLED_8X16);
	    	OLED_ShowChinese(0, 48, "＊確認　　＃清除");

	    	press_key = KEY_SCAN();
//	    	printf("press_key = %c \r\n",press_key);
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
					vTaskDelay(200);
					HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_RESET);
					RFID_pass = 1;
					Mode = CHECK_RFID;
				}
				else if((str[0]==243) && (str[1]==188) && (str[2]==5) && (str[3]==53) && (str[4]==127) )
				{
					HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_SET);
					vTaskDelay(200);
					HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_RESET);
					RFID_pass = 1;
					Mode = CHECK_RFID;
				}
				memset(str, 0, sizeof(str));
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

	    	OLED_Update();
	        break;

	    case CHECK_PW:

			if (strcmp(Password, Default_Password) == 0) {
				printf("Password is correct.\r\n");

				OLED_ClearArea(0, 16, 128, 16);
				OLED_ShowChinese(0, 16, "密碼正確");
				OLED_Update();
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
			OLED_Clear();
			OLED_Update();

	    	break;

	    case CHECK_RFID:

			if (RFID_pass == 1)
			{
				printf("RDID pass.\r\n");
				RFID_pass = 0;
				OLED_ClearArea(0, 16, 128, 16);
				OLED_ShowChinese(0, 16, "卡片正確");
				OLED_Update();
				vTaskDelay(500);
				Mode = OPEN_CLEAR;
			}
//			else if (RFID_pass == 1)
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
						Mode = NEW_RFID;
					}
					else if (Sel_Item == 2) /* DEL_RFID */
					{
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

			 break;
	    case DEL_RFID:

			 break;
	    case NEW_FP:

			 break;
	    case DEL_FP:

			 break;
	    default:
	        break;
		}

		vTaskDelay(200);
	}


}



