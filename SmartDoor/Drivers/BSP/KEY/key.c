#include "stm32f4xx_hal.h"
#include "key.h"
#include "gpio.h"
#include <string.h>
#include <stdio.h>

uint8_t Key_row[1]={0xff};


char KEY_ROW_SCAN(void)
{
    //读出行扫描状态
    Key_row[0] = HAL_GPIO_ReadPin(GPIOB,KEY_row0_Pin)<<3;
    Key_row[0] = Key_row[0] | (HAL_GPIO_ReadPin(GPIOB,KEY_row1_Pin)<<2);
    Key_row[0] = Key_row[0] | (HAL_GPIO_ReadPin(GPIOB,KEY_row2_Pin)<<1);
    Key_row[0] = Key_row[0] | (HAL_GPIO_ReadPin(GPIOB,KEY_row3_Pin));

    if(Key_row[0] != 0x0f)         //行扫描有变化，判断该列有按键按下
    {
      HAL_Delay(10);                    //消抖
      if(Key_row[0] != 0x0f)
        {
                //printf("Key_Row_DATA = 0x%x\r\n",Key_row[0]);
                switch(Key_row[0])
                {
                    case 0x07:         //0111 判断为该列第1行的按键按下
                        return 1;
                    case 0x0b:         //1011 判断为该列第2行的按键按下
                        return 2;
                    case 0x0d:         //1101 判断为该列第3行的按键按下
                        return 3;
                    case 0x0e:         //1110 判断为该列第4行的按键按下
                        return 4;
                    default :
                        return 0;
                }
        }
        else return 0;
    }
    else return 0;
}

char KEY_SCAN(void)
{
    char Key_Num=0;       //1-16对应的按键数
    char key_row_num=0;        //行扫描结果记录

    KEY_CLO0_OUT_LOW;
    if( (key_row_num=KEY_ROW_SCAN()) != 0 )
    {
        while(KEY_ROW_SCAN() != 0);  //消抖
        Key_Num = 0 + key_row_num;
        //printf("Key_Clo_1\r\n");
    }
    KEY_CLO0_OUT_HIGH;

    KEY_CLO1_OUT_LOW;
    if( (key_row_num=KEY_ROW_SCAN()) != 0 )
    {
        while(KEY_ROW_SCAN() != 0);
        Key_Num = 4 + key_row_num;
        //printf("Key_Clo_2\r\n");
    }
    KEY_CLO1_OUT_HIGH;

    KEY_CLO2_OUT_LOW;
    if( (key_row_num=KEY_ROW_SCAN()) != 0 )
    {
        while(KEY_ROW_SCAN() != 0);
    Key_Num = 8 + key_row_num;
        //printf("Key_Clo_3\r\n");
    }
    KEY_CLO2_OUT_HIGH;

    KEY_CLO3_OUT_LOW;
    if( (key_row_num=KEY_ROW_SCAN()) != 0 )
    {
        while(KEY_ROW_SCAN() != 0);
        Key_Num = 12 + key_row_num;
        //printf("Key_Clo_4\r\n");
    }
    KEY_CLO3_OUT_HIGH;


	if(0 == Key_Num) Key_Num = '\0';
	else if(1 == Key_Num) Key_Num = '1';
	else if(2 == Key_Num) Key_Num = '2';
	else if(3 == Key_Num) Key_Num = '3';
	else if(4 == Key_Num) Key_Num = 'A';
	else if(5 == Key_Num) Key_Num = '4';
	else if(6 == Key_Num) Key_Num = '5';
	else if(7 == Key_Num) Key_Num = '6';
	else if(8 == Key_Num) Key_Num = 'B';
	else if(9 == Key_Num) Key_Num = '7';
	else if(10 == Key_Num) Key_Num = '8';
	else if(11 == Key_Num) Key_Num = '9';
	else if(12 == Key_Num) Key_Num = 'C';
	else if(13 == Key_Num) Key_Num = '*';
	else if(14 == Key_Num) Key_Num = '0';
	else if(15 == Key_Num) Key_Num = '#';
	else if(16 == Key_Num) Key_Num = 'D';



    return Key_Num;
}



void appendChar(char *str, char c, int max_length) {

	uint8_t error = 0;
    int len = strlen(str);  // 找到字符串的當前長度

    if (len < max_length -1 )
    {
        str[len] = c;           // 在原來的結尾 null 字符位置放置新字符
        str[len + 1] = '\0';    // 添加新的結尾 null 字符
    }
    else
    {
    	error = 1;
    	printf("no space to append new char\n");
    }
}

void removeChar(char *str) {
	uint8_t error;
	int len = strlen(str);  // 找到字符串的當前長度

	if(len > 0 )
	{
		str[len - 1] = '\0';  // 將最後一個字符設置為 null 字符
	}
    else
    {
    	error = 1;
    	printf("no char to remove\n");
    }

}
