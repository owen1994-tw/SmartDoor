#include "CardMan.h"

extern uint8_t storedRFIDCards[MAX_CARDS][RFID_CARD_LENGTH];
extern uint8_t EmptyRFIDCards[RFID_CARD_LENGTH];

void InitStoredRFIDCards()
{
    for (int i = 0; i < MAX_CARDS; i++)
    {
        for (int j = 0; j < RFID_CARD_LENGTH; j++)
        {
            storedRFIDCards[i][j] = 0xFF;
        }
    }
    memset(EmptyRFIDCards, 0xFF, RFID_CARD_LENGTH);
    storedRFIDCards[0][0] = 67;
    storedRFIDCards[0][1] = 4;
    storedRFIDCards[0][2] = 23;
    storedRFIDCards[0][3] = 20;
    storedRFIDCards[0][4] = 68;
}


uchar StoreCardSerialNumber(uint8_t idx ,uchar *serNum)
{
    if (storedRFIDCards[idx][0] == 0xFF)
    {
        memcpy(storedRFIDCards[idx], serNum, RFID_CARD_LENGTH);
        return MI_OK;
    }
    return MI_ERR;
}


//uchar DeleteCardSerialNumber(uint8_t idx , uchar *serNum)
//{
//
//    if (memcmp(storedRFIDCards[idx], serNum, RFID_CARD_LENGTH) == 0) {
//        for (int j = 0; j < RFID_CARD_LENGTH; j++) {
//            storedRFIDCards[idx][j] = 0xFF;
//        }
//        return MI_OK;
//    }
//    return MI_ERR;
//}

uchar DeleteCardSerialNumber(uint8_t idx )
{

	for (int j = 0; j < RFID_CARD_LENGTH; j++)
	{
		storedRFIDCards[idx][j] = 0xFF;
	}
	return MI_OK;


}


int CheckCardSerialNumber(uchar *serNum) {

    for (int i = 0; i < MAX_CARDS; i++) {
        if (memcmp(storedRFIDCards[i], serNum, RFID_CARD_LENGTH) == 0) {
            return i;
        }
    }
    return -1;
}

int CheckEmptyIdxStoredRFIDCards() {

	int Empty_idx = 0;
	int i;
    for (i = 0; i < MAX_CARDS; i++) {
        if (memcmp(storedRFIDCards[i], EmptyRFIDCards, RFID_CARD_LENGTH) == 0) {
        	Empty_idx = i;
            break;
        }
    }

    if(i == MAX_CARDS)
    {
    	Empty_idx = -1;
    }

    return Empty_idx;

}
