#include "mac.h" 
#include "ranging.h"

//Constructor and destructor

mac::mac() {
	_seqNumber = 0;
}


mac::~mac() {
}

//for poll message we use just 2 bytes address
//total=12 bytes
void mac::generateBlinkFrame(uint8_t frame[], uint8_t sourceAddress[], uint8_t sourceShortAddress[]) {
	//Frame Control
	*frame = FC_1_BLINK;
	//sequence number
	*(frame+1) = _seqNumber;
	//tag 64 bit ID (8 bytes address) -- reverse
	uint8_t sourceAddressReverse[8];
	reverseArray(sourceAddressReverse, sourceAddress, 8);
	memcpy(frame+2, sourceAddressReverse, 8);
	
	//tag 2bytes address:
	uint8_t sourceShortAddressReverse[2];
	reverseArray(sourceShortAddressReverse, sourceShortAddress, 2);
	memcpy(frame+10, sourceShortAddressReverse, 2);
	
	//we increment seqNumber
	_seqNumber++;
}

//the short frame usually for Resp, Final, or Report
//2 bytes for Desination Address and 2 bytes for Source Address
//total=9 bytes
void mac::generateShortMACFrame(uint8_t frame[], uint8_t sourceShortAddress[], uint8_t destinationShortAddress[]) {
	//Frame controle
	*frame     = FC_1;
	*(frame+1) = FC_2_SHORT;
	//sequence number
	*(frame+2) = _seqNumber;
	//PAN ID
	*(frame+3) = 0xCA;
	*(frame+4) = 0xDE;
	
	
	//destination address (2 bytes)
	uint8_t destinationShortAddressReverse[2];
	reverseArray(destinationShortAddressReverse, destinationShortAddress, 2);
	memcpy(frame+5, destinationShortAddressReverse, 2);
	
	//source address (2 bytes)
	uint8_t sourceShortAddressReverse[2];
	reverseArray(sourceShortAddressReverse, sourceShortAddress, 2);
	memcpy(frame+7, sourceShortAddressReverse, 2);
	
	
	//we increment seqNumber
	_seqNumber++;
}

//the long frame for Ranging init
//8 bytes for Destination Address and 2 bytes for Source Address
//total=15
void mac::generateLongMACFrame(uint8_t frame[], uint8_t sourceShortAddress[], uint8_t destinationAddress[]) {
	//Frame controle
	*frame     = FC_1;
	*(frame+1) = FC_2;
	//sequence number
	*(frame+2) = _seqNumber;
	//PAN ID (0xDECA)
	*(frame+3) = 0xCA;
	*(frame+4) = 0xDE;
	
	//destination address (8 bytes) - we need to reverse the uint8_t array
	uint8_t destinationAddressReverse[8];
	reverseArray(destinationAddressReverse, destinationAddress, 8);
	memcpy(frame+5, destinationAddressReverse, 8);
	
	//source address (2 bytes)
	uint8_t sourceShortAddressReverse[2];
	reverseArray(sourceShortAddressReverse, sourceShortAddress, 2);
	memcpy(frame+13, sourceShortAddressReverse, 2);
	
	//we increment seqNumber
	_seqNumber++;
}


void mac::decodeBlinkFrame(uint8_t frame[], uint8_t address[], uint8_t shortAddress[]) {
	//we save the long address of the sender into the device. -- reverse direction
	uint8_t reverseAddress[8];
	memcpy(reverseAddress, frame+2, 8);
	reverseArray(address, reverseAddress, 8);
	
	uint8_t reverseShortAddress[2];
	memcpy(reverseShortAddress, frame+10, 2);
	reverseArray(shortAddress, reverseShortAddress, 2);
}

bool mac::decodeShortMACFrame(uint8_t frame[], uint8_t address[]) {
	uint8_t reverseAddress[2];
	memcpy(reverseAddress, frame+7, 2);
	reverseArray(address, reverseAddress, 2);
	return true;
}

bool mac::decodeLongMACFrame(uint8_t frame[], uint8_t address[]) {
	uint8_t reverseAddress[2];
	memcpy(reverseAddress, frame+13, 2);
	reverseArray(address, reverseAddress, 2);
	return true;
}


void mac::reverseArray(uint8_t to[], uint8_t from[], int size) {
	for(int i = 0; i < size; i++) {
		*(to+i) = *(from+size-i-1);
	}
	
}