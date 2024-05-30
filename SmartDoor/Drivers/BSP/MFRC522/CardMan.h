#include "MFRC522.h"
#include <stdint.h>
//#define uchar uint8_t
#define MAX_CARDS 3
#define RFID_CARD_LENGTH 5


//storedRFIDCards

// Functions for manage card
void InitStoredRFIDCards();
uchar StoreCardSerialNumber(uint8_t idx ,uchar *serNum);
//uchar DeleteCardSerialNumber(uint8_t idx , uchar *serNum);
uchar DeleteCardSerialNumber(uint8_t idx );
int CheckCardSerialNumber(uchar *serNum);
int CheckEmptyIdxStoredRFIDCards();
