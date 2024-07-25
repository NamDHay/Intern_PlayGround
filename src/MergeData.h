#include <Arduino.h>

void FourByteToOneByte(float value, byte* dataArray) {
  byte* pFloat = (byte*)&value;
  
  for (byte i = 0; i < 4; i++) {
    dataArray[i] = *(pFloat + i);
  }
}

float OneByteToFourByte(byte* dataArray) {
  float value;
  byte* pValue = (byte*)&value; 

  for (byte i = 0; i < 4; i++) {
    *(pValue + i) = dataArray[i];
  }
  return value;
}
void TwoByteToOneByte(short value, byte* dataArray){
  byte* pShort = (byte*)&value;

  for(byte i = 0;i < 2; i++){
    dataArray[i] = *(pShort + i);
    Serial.printf("0x%x \n", dataArray[i]);
  }
}
short OneByteToTwoByte(byte* dataArray){
  short value;
  byte* pValue = (byte*)value;
  for(byte i = 0; i < 2; i++){
    *(pValue + i) = dataArray[i]; 
  }
  return value;
}
void BitsToBytes(bool* bitArray, byte* byteArray, int numBits) {
  int numBytes = (numBits + 7) / 8; // Calculate the number of bytes needed
  
  for (int i = 0; i < numBytes; i++) {
    byteArray[i] = 0;
  }

  for (int i = 0; i < numBits; i++) {
    if (bitArray[i]) {
      byteArray[i / 8] |= (1 << (i % 8));
    }
  }
}

void BytesToBits(byte* byteArray, bool* bitArray, int numBytes) {
  for (int i = 0; i < numBytes; i++) {
    for (int j = 0; j < 8; j++) {
      bitArray[i * 8 + j] = (byteArray[i] & (1 << j)) != 0;
    }
  }
}