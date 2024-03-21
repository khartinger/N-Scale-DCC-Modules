//_____D1_class_PCF8574.cpp______________________khartinger_____
// D1 mini class for i2c remote 8-Bit I/O Expander PCF8574.
// Default i2c address is 0x20 (other options 0x21...0x27).
// Created by Karl Hartinger, April 06, 2023
// Updates
//
// Released into the public domain.
#include "D1_class_PCF8574.h"

//**************************************************************
//       constructor & co
//**************************************************************

//_______constructor (default 0x23)_____________________________
PCF8574::PCF8574() { i2cAddress=PCF8574_ADDR; setup(); }

//_______constructor with i2c address___________________________
PCF8574::PCF8574(int i2c_address) {
 if(checkAddress(i2c_address)) i2cAddress=i2c_address;
 else i2cAddress=PCF8574_ADDR;
 setup();
}

//_______constructor with i2c address + start value_____________
PCF8574::PCF8574(int i2c_address, byte startvalue) {
 if(checkAddress(i2c_address)) i2cAddress=i2c_address;
 else i2cAddress=PCF8574_ADDR;
 setup();
 ioByte = startvalue;
}


//_______setup properties_______________________________________
void PCF8574::setup() {
 invertOutput = false;
 ioByte = PCF8574_STARTVALUE;
}

//_______is the i2c address in the range 0x20 to 0x27 ?_________
// return true: yes, false: not in the range
bool PCF8574::checkAddress(int i2c_address) {
 if((i2c_address>=0x20) && (i2c_address<=0x27)) return true;
 return false;
}

//**************************************************************
//       setter methods
//**************************************************************

//_______set i2c address________________________________________
// return true: address set, false: use default i2c address
bool PCF8574::setAddress(int i2c_address)
{
 if(checkAddress(i2c_address)) {
  i2cAddress=i2c_address;
  return true;
 }
 i2cAddress=PCF8574_ADDR;
 return false;
}

//_______set bit 0 or 1_________________________________________
// return: true=bit set 0 or 1, false=error
bool PCF8574::setBit(int bitnumber, int bitvalue) {
 if(bitnumber<0 || bitnumber>7) {
  status=PCF8574_ERR_BIT_NUM;
  return false;
 }
 if(bitvalue<0 || bitvalue>1) { 
  status=PCF8574_ERR_BIT_VAL;
  return false; 
 }
 int mask = 1 << bitnumber;
 //------set output to LOW (0 V)--------------------------------
 if(bitvalue==0) ioByte &= ~(mask);
 //------set output to HIGH (0 V)-------------------------------
 if(bitvalue==1) ioByte |= mask;
 //------output byte to pins and check input--------------------
 if (!writeIoByte()) return false;
 int res=getBit(bitnumber);
 if(res!=bitvalue) return false;
 return true;
}

//_______set all I/Os of PCF8574________________________________
// return: true: value set and read back correctly, false: error
bool PCF8574::setByte(byte ioByte_) {
 byte oldByte=ioByte;
 //------write i/o byte-----------------------------------------
 ioByte=ioByte_;
 if(!writeIoByte()) {
  ioByte=oldByte;
  return false;
 }
 //------read i/o byte------------------------------------------
 if(!readIoByte()) return false;
 //------check if the bits could be set correctly---------------
 if(ioByte!=ioByte_) return false;
 return true;
}

//_______must hardware signals be inverted?_____________________
void PCF8574::setInvertOutput(bool doInvert) {
 invertOutput=doInvert;
}

//**************************************************************
//       getter methods
//**************************************************************

//_______return i2c address_____________________________________
int PCF8574::getAddress() { return i2cAddress; }

//_______system status as integer_______________________________
// Meaning of the numbers see getsStatus()
int PCF8574::getStatus() { return status; }

//_______system status as text__________________________________
String PCF8574::getsStatus() {
  switch(status)
 {
  case PCF8574_OK:
   return String("OK");
  case PCF8574_ERR_TOO_LONG:
   return String("I2C send buffer is too small for the data");
  case PCF8574_ERR_NACK_ADDR:
   return String("No acknowledge (ACK) after address byte");
  case PCF8574_ERR_NACK_DATA:
   return String("No acknowledge (ACK) after data byte");
  case PCF8574_ERR_OTHER:
   return String("I2C Error");
  case PCF8574_ERR_NUM_BYTES:
   return String("Wrong number of bytes");
  case PCF8574_ERR_NO_BYTE:
   return String("No byte received");
  case PCF8574_ERR_BIT_NUM:
   return String("bit number not 0..7");
  case PCF8574_ERR_BIT_VAL:
   return String("bit value not 0 or 1");

  
  default:
   return String("Unknown error number ")+String(status);
 }
 return String("Serious, unknown error");
}

//_______get pcf8574 inputs_____________________________________
// return new value or -1 on error
int PCF8574::getByte() {
 byte oldByte=ioByte;
 if(!readIoByte()) {
  ioByte=oldByte;
  return -1;
 }
 return ioByte;
}

//_______set bit 0 or 1_________________________________________
// return: 0 or 1 or -1 on error
int PCF8574::getBit(int bitnumber) {
 if(bitnumber<0 || bitnumber>7) {
  status=PCF8574_ERR_BIT_NUM;
  return -1;
 }
 //------read byte from pcf8574---------------------------------
 int iRet=getByte();
 if(iRet==-1) return -1; // error
 int mask = 1 << bitnumber;
 if((iRet&mask) > 0) return 1;
 return 0;
}

//_______invert output?_________________________________________
bool PCF8574::getInvertOutput() { return invertOutput; }

//**************************************************************
//       working methods
//**************************************************************

//_______check sensor, start I2C and first measuring____________
bool PCF8574::begin() { return begin(true); }

//_______check sensor, start first measuring____________________
// set startI2C=false if I2C has already been started
// return: true=OK, false on error
bool PCF8574::begin(bool startI2C) {
 //------start I2C?---------------------------------------------
 if(startI2C) {
  Wire.begin();                   // call Wire.begin only once
 }
 //------write start value--------------------------------------
 if(!writeIoByte()) return false;
 delay(1);
 return readIoByte();
}

//**************************************************************
//       helper methods: i2c-access
//**************************************************************

//_______i2c: write io byte_____________________________________
// return: true=byte written, false on error
bool PCF8574::writeIoByte() {
 byte out_=ioByte;
 if(invertOutput) out_=~out_;
 Wire.beginTransmission(i2cAddress);
 Wire.write(out_);
 status=Wire.endTransmission();
 if(status!=PCF8574_OK) return false;
 return true;
}

//_______i2c: read io byte______________________________________
// return: true=byte read, false on error
bool PCF8574::readIoByte() {
 int num=Wire.requestFrom(i2cAddress, 1);
 if(num==0) {status=PCF8574_ERR_NO_BYTE; return false;};
 num=Wire.available();
 if(num==0) {status=PCF8574_ERR_NO_BYTE; return false;};
 if(num!=1) 
 {
  Wire.endTransmission();
  status=PCF8574_ERR_NUM_BYTES;
  return false;
 }
 byte in_=Wire.read();
 status=PCF8574_OK;
 //status=Wire.endTransmission();
 if(invertOutput) in_=~in_;
 ioByte=in_;
 return true;
}
