//#include "stm32f4xx_hal_uart.h"
#include "usart.h"

#ifndef FREERTOS_STDINT
typedef unsigned char uint8_t;
#endif

enum as608_pkg_type {
    AS608_CMD = 1,
    AS608_DATA,
    AS608_NONE,
    AS608_ACK = 7,
    AS608_EDATA
};

enum as608_cmd_type {
    AS608_GenImg = 0x01,
    AS608_Img2Tz,
    AS608_Match,
    AS608_Search,
    AS608_RegModel,
    AS608_Store,
    AS608_LoadChar,
    AS608_UpChar,
    AS608_DownChr,
    AS608_UpImage,
    AS608_DownImage,
    AS608_DeletChar,
    AS608_Empty,
    AS608_SetSysPara,
    AS608_ReadSysPara,
    AS608_SetPwd = 0x12,
    AS608_VfyPwd,
    AS608_GetRandomCode,
    AS608_SetAdder,
    AS608_TempleteNum = 0x1d,
    AS608_ReadConList
};

/* Low-Level API */
void AS608_Transmit(UART_HandleTypeDef *uart,
                    const uint8_t type,
                    const uint8_t *pkg_len,
                    const uint8_t *data,
                    const uint8_t *checksum);

int AS608_Receive(UART_HandleTypeDef *uart, int n, uint8_t *buffer);

int FP_GenImg(UART_HandleTypeDef *uart);

int FP_GenImg(UART_HandleTypeDef *uart);

int FP_Img2Tz(UART_HandleTypeDef *uart, int buffID);

int FP_RegModel(UART_HandleTypeDef *uart);

int FP_Store(UART_HandleTypeDef *uart, int buffID, int pageID);

int FP_Empty(UART_HandleTypeDef *uart);

int FP_Search(UART_HandleTypeDef *uart, int buffID);


/* High-Level API */
int register_new_fingerprint(UART_HandleTypeDef *uart);

int search_fingerprint(UART_HandleTypeDef *uart);
