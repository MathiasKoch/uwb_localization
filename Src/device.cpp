#include "device.h"
#include "hal.h"


//Constructor and destructor
device::device() {
	//randomShortAddress();
}

device::device(uint8_t deviceAddress[], bool shortOne) {
	if(!shortOne) {
		//we have a 8 ByteS address
		setAddress(deviceAddress);
		randomShortAddress();
	}else {
		//we have a short address (2 ByteS)
		setShortAddress(deviceAddress);
	}
}

device::device(uint8_t deviceAddress[], uint8_t shortAddress[]) {
	//we have a 8 ByteS address
	setAddress(deviceAddress);
	//we set the 2 ByteS address
	setShortAddress(shortAddress);
}

device::~device() {
}

void device::setReplyTime(uint16_t replyDelayTimeUs) { 
	_replyDelayTimeUS = replyDelayTimeUs; 
}

void device::setAddress(uint8_t* deviceAddress) {
	memcpy(_ownAddress, deviceAddress, 8);
}

void device::setShortAddress(uint8_t deviceAddress[]) {
	memcpy(_shortAddress, deviceAddress, 2);
}


void device::setRange(float range) { _range = round(range*100); }

void device::setRXPower(float RXPower) { _RXPower = round(RXPower*100); }

void device::setFPPower(float FPPower) { _FPPower = round(FPPower*100); }

void device::setQuality(float quality) { _quality = round(quality*100); }


uint8_t* device::getByteAddress() {
	return _ownAddress;
}

/*
String device::getAddress(){
    char string[25];
    sprintf(string, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
            _ownAddress[0], _ownAddress[1], _ownAddress[2], _ownAddress[3], _ownAddress[4], _ownAddress[5], _ownAddress[6], _ownAddress[7]);
    return String(string);
}*/

uint8_t* device::getByteShortAddress() {
	return _shortAddress;
}

/*
String device::getShortAddress(){
    char string[6];
    sprintf(string, "%02X:%02X",
            _shortAddress[0], _shortAddress[1]);
    return String(string);
}
*/

unsigned int device::getShortAddress() {
	return _shortAddress[1]*256+_shortAddress[0];
}


bool device::isAddressEqual(device* device) {
	if(memcmp(this->getByteAddress(), device->getByteAddress(), 8) == 0) {
		return true;
	}else {
		return false;
	}
}

bool device::isShortAddressEqual(device* device) {
	if(memcmp(this->getByteShortAddress(), device->getByteShortAddress(), 2) == 0) {
		return true;
	}else {
		return false;
	}
}


float device::getRange() { return float(_range)/100.0f; }

float device::getRXPower() { return float(_RXPower)/100.0f; }

float device::getFPPower() { return float(_FPPower)/100.0f; }

float device::getQuality() { return float(_quality)/100.0f; }


void device::randomShortAddress() {
	_shortAddress[0] = (uint8_t)RNG_Get();
	_shortAddress[1] = (uint8_t)RNG_Get();
}

void device::noteActivity() {
	_activity = HAL_GetTick();
}


bool device::isInactive() {
	//One second of inactivity
	if(HAL_GetTick()-_activity > INACTIVITY_TIME) {
		_activity = HAL_GetTick();
		return true;
	}
	return false;
}