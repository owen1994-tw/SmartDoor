#include "as608.h"

static uint8_t as608_hdr[6] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF};

void AS608_Transmit(UART_HandleTypeDef *uart,
                    const uint8_t type,
                    const uint8_t *pkg_len,
                    const uint8_t *data,
                    const uint8_t *checksum)
{
    HAL_UART_Transmit(uart, as608_hdr, 6, 100);
    HAL_UART_Transmit(uart, &type, 1, 100);
    HAL_UART_Transmit(uart, pkg_len, 2, 100);
    HAL_UART_Transmit(uart, data, (pkg_len[0] << 16) + pkg_len[1] - 2, 100);
    HAL_UART_Transmit(uart, checksum, 2, 100);
}

int AS608_Receive(UART_HandleTypeDef *uart, int n, uint8_t *buffer)
{
    /* Receive data byte by byte */
//	for (int i = 0; i < n; i++){
////		vTaskDelay(1);
//		HAL_UART_Receive(uart, &buffer[i], 1, 1000);
//	}
	HAL_UART_Receive(uart, buffer, n, 1000);

    int fail = 0;
    /* Check if the package is from AS608 */
    for (int i = 0; i < 6; i++)
        fail |= as608_hdr[i] != buffer[i];

    return -fail;
}


int FP_GenImg(UART_HandleTypeDef *uart)
{
    uint8_t type = AS608_CMD;
    uint8_t pkg_len[2] = {0x00, 0x03};
    uint8_t cmd = AS608_GenImg;
    uint8_t checksum[2] = {0x00, 0x05};
    /* Transmit */
    AS608_Transmit(uart, type, pkg_len, &cmd, checksum);

    /* Receive */
    uint8_t buffer[12] = {0};
    int fail = AS608_Receive(uart, 12, buffer);
    if (fail)
        return fail;

    /* check if FP register successfully.*/
    fail = buffer[9];
    /*
     * Option: Check correct code
     */

    return fail;
}

int FP_Img2Tz(UART_HandleTypeDef *uart, int buffID)
{
    uint8_t type = AS608_CMD;
    uint8_t pkg_len[2] = {0x00, 0x04};
    uint8_t data[2] = {AS608_Img2Tz, buffID};

    short cks = type + pkg_len[0] + pkg_len[1] + data[0] + data[1];
    uint8_t checksum[2] = {cks >> 8, cks & 0xFF};
    /* Transmit */
    AS608_Transmit(uart, type, pkg_len, data, checksum);

    /* Receive */
    uint8_t buffer[12] = {0};
    int fail = AS608_Receive(uart, 12, buffer);
    if (fail)
        return fail;

    fail = buffer[9];
    return fail;
}

int FP_RegModel(UART_HandleTypeDef *uart)
{
    uint8_t type = AS608_CMD;
    uint8_t pkg_len[2] = {0x00, 0x03};
    uint8_t cmd = AS608_RegModel;
    uint8_t checksum[2] = {0x00, 0x09};

    /* Transmit */
    AS608_Transmit(uart, type, pkg_len, &cmd, checksum);

    /* Receive */
    uint8_t buffer[12] = {0};
    int fail = AS608_Receive(uart, 12, buffer);
    if (fail)
        return fail;

    /* check if FP register successfully.*/
    fail = buffer[9];
    /*
     * Option: Check correct code
     */

    return fail;
}

int FP_Store(UART_HandleTypeDef *uart, int buffID, int pageID)
{
    uint8_t type = AS608_CMD;
    uint8_t pkg_len[2] = {0x00, 0x06};
//    uint8_t data[4] = {AS608_Store, buffID, pageID};

    uint8_t data[4];
    data[0] = AS608_Store;          // 1 byte
    data[1] = (uint8_t)buffID;      // 1 byte
    data[2] = (uint8_t)(pageID >> 8); // 2 byte (high byte)
    data[3] = (uint8_t)pageID;      // 2 byte (low byte)

    short cks =
        type + pkg_len[0] + pkg_len[1] + data[0] + data[1] + data[2] + data[3];
    uint8_t checksum[2] = {cks >> 8, cks & 0xFF};
    /* Transmit */
    AS608_Transmit(uart, type, pkg_len, data, checksum);

    /* Receive */
    uint8_t buffer[12] = {0};
    int fail = AS608_Receive(uart, 12, buffer);
    if (fail)
        return fail;

    fail = buffer[9];
    return fail;
}


int FP_Delete(UART_HandleTypeDef *uart, int pageID)
{
    uint8_t type = AS608_CMD;
    uint8_t pkg_len[2] = {0x00, 0x07};
//    uint8_t data[4] = {AS608_Store, buffID, pageID};

    uint8_t data[5];
    data[0] = AS608_DeletChar;          // 1 byte
    data[1] = (uint8_t)(pageID >> 8); // 2 byte (high byte)
    data[2] = (uint8_t)pageID;      // 2 byte (low byte)
    data[3] = 0x00;
    data[4] = 0x01;

    short cks =
        type + pkg_len[0] + pkg_len[1] + data[0] + data[1] + data[2] + data[3] + data[4];
    uint8_t checksum[2] = {cks >> 8, cks & 0xFF};
    /* Transmit */
    AS608_Transmit(uart, type, pkg_len, data, checksum);

    /* Receive */
    uint8_t buffer[12] = {0};
    int fail = AS608_Receive(uart, 12, buffer);
    if (fail)
        return fail;

    fail = buffer[9];
    return fail;
}

int FP_Empty(UART_HandleTypeDef *uart)
{
    uint8_t type = AS608_CMD;
    uint8_t pkg_len[2] = {0x00, 0x03};
    uint8_t cmd = 0x0d;
    uint8_t checksum[2] = {0x00, 0x11};

    /* Transmit */
    AS608_Transmit(uart, type, pkg_len, &cmd, checksum);

    /* Receive */
    uint8_t buffer[12] = {0};
    int fail = AS608_Receive(uart, 12, buffer);
    if (fail)
        return fail;

    fail = buffer[9];
    /*
     * Option: Check correct code
     */

    return fail;
}

int FP_Search(UART_HandleTypeDef *uart, int buffID){
    uint8_t type = AS608_CMD;
    uint8_t pkg_len[2] = {0x00, 0x08};
//    uint8_t data[6] = {AS608_Search, buffID, 0x00, 0x00, 0x00, 0x40};
    uint8_t data[6] = {AS608_Search, buffID, 0x00, 0x00, 0x00, 0x40};
    short cks = type + pkg_len[0] + pkg_len[1];
    for (int i = 0; i < 6; i++)
        cks += data[i];
    uint8_t checksum[2] = {cks >> 8, cks & 0xFF};
    /* Transmit */
    AS608_Transmit(uart, type, pkg_len, data, checksum);

    /* Receive */
    uint8_t buffer[16] = {0};
    int fail = AS608_Receive(uart, 16, buffer);
    if (fail)
        return fail;

    fail = buffer[9];
    /*
     * Option: Check correct code, PageID and MatchScore
     */

    return fail;
}
static int page_count = 0;

int register_new_fingerprint(UART_HandleTypeDef *uart, int  set_page_count)
{
	/*
	 * You need to put your finger on the device two times.
	 * In order to indicate you are in first or second step,
	 * you need to show some prompt to tell the user.
	 */
    int fail = 0;

    //		LD6 blue
    //		LD5 red
    //		LD3 orange
    //		LD4 green

    /*
     * First time
     * TODO: Show the indicator to tell the user put the finger
     * on the device.
     */
    HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
    printf("reg fp 1\r\n");
    int wait_counter = 0;
    while ((fail = FP_GenImg(uart)) == 0x2)
    {
    	wait_counter++;
    	if(wait_counter > 20 )
    	{
    		wait_counter = 0;
    		break;
    	}
    }
    HAL_Delay(10);
    if (fail)
        return fail;

    fail = FP_Img2Tz(uart, 0x01);
    if (fail)
        return fail;

    /*
     * Second time
     * TODO: Show the indicator to tell the user put the finger
     * on the device.
     */
    HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
    printf("reg fp 2\r\n");
    while ((fail = FP_GenImg(uart)) == 0x2)
    {
    	wait_counter++;
    	if(wait_counter > 20 )
    	{
    		wait_counter = 0;
    		break;
    	}
    }
    HAL_Delay(10);
    if (fail)
        return fail;

    fail = FP_Img2Tz(uart, 0x02);
    if (fail)
        return fail;

    fail = FP_RegModel(uart);
    if (fail)
        return fail;

    fail = FP_Store(uart, 0x01, set_page_count);
    return fail;
}

int search_fingerprint(UART_HandleTypeDef *uart)
{
    int fail = 0;
    int wait_counter = 0;
    while ((fail = FP_GenImg(uart)) == 0x2)
    {
    	wait_counter++;
    	if(wait_counter > 10 )
    	{
    		wait_counter = 0;
    		break;
    	}
    }
        HAL_Delay(10);
    if (fail)
        return fail;

    fail = FP_Img2Tz(uart, 0x01);
    if (fail)
        return fail;

    fail = FP_Search(uart, 0x01);
    return fail;
}
