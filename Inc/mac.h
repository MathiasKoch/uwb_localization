#define FC_1 0x41
#define FC_1_BLINK 0xC5
#define FC_2 0x8C
#define FC_2_SHORT 0x88

#define PAN_ID_1 0xCA
#define PAN_ID_2 0xDE

#define SHORT_MAC_LEN 9
#define LONG_MAC_LEN 15
#define BLINK_LEN 12
#define CRC_LEN 2


#ifndef _mac_H_INCLUDED
#define _mac_H_INCLUDED

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "device.h" 
#include "deca_device_api.h"
#include "deca_regs.h"
#include "decawave.h"
#include "hal.h"

class device;

class mac {
public:
  //Constructor and destructor
  mac(device* parent);
  mac();
  ~mac();
  
  
  //setters
  void setDestinationAddress(uint8_t* destinationAddress);
  void setDestinationAddressShort(uint8_t* shortDestinationAddress);
  void setSourceAddress(uint8_t* sourceAddress);
  void setSourceAddressShort(uint8_t* shortSourceAddress);
  
  
  //for poll message we use just 2 bytes address
  //total=12 bytes
  void generateBlinkFrame(uint8_t frame[], uint8_t sourceAddress[], uint8_t sourceShortAddress[]);
  
  //the short fram usually for Resp, Final, or Report
  //2 bytes for Desination Address and 2 bytes for Source Address
  //total=9 bytes
  void generateShortMACFrame(uint8_t frame[], uint8_t sourceShortAddress[], uint8_t destinationShortAddress[]);
  
  //the long frame for Ranging init
  //8 bytes for Destination Address and 2 bytes for Source Address
  //total of
  void generateLongMACFrame(uint8_t frame[], uint8_t sourceShortAddress[], uint8_t destinationAddress[]);
  
  //in order to decode the frame and save source Address!
  void    decodeBlinkFrame(uint8_t frame[], uint8_t address[], uint8_t shortAddress[]);
  bool decodeShortMACFrame(uint8_t frame[], uint8_t address[]);
  bool decodeLongMACFrame(uint8_t frame[], uint8_t address[]);
  

private:
  uint8_t  _seqNumber;
  void reverseArray(uint8_t to[], uint8_t from[], int size);
  
  
};


#endif