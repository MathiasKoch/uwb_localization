#define INACTIVITY_TIME 2000

#ifndef _device_H_INCLUDED
#define _device_H_INCLUDED

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "decaTime.h"
#include "mac.h"
#include "rng.h"

class mac;

class device {
public:
	//Constructor and destructor
	device();
	device(uint8_t address[], uint8_t shortAddress[]);
	device(uint8_t address[], bool shortOne = false);
	~device();
	
	//setters:
	void setReplyTime(uint16_t replyDelayTimeUs);
	void setAddress(char address[]);
	void setAddress(uint8_t* address);
	void setShortAddress(uint8_t address[]);
	
	void setRange(float range);
	void setRXPower(float power);
	void setFPPower(float power);
	void setQuality(float quality);
	
	void setReplyDelayTime(int time) { _replyDelayTimeUS = time; }

	void setKalmanID(short deviceID) { _EKF_ID = deviceID; }
	
	void setIndex(short index) { _index = index; }
	
	//getters
	uint16_t getReplyTime() { return _replyDelayTimeUS; }
	
	uint8_t* getByteAddress();
	
	short getIndex() { return _index; }
	short getKalmanID() { return _EKF_ID; }
	
	//String getAddress();
	uint8_t* getByteShortAddress();
	unsigned int getShortAddress();
	//String getShortAddress();
	
	float getRange();
	float getRXPower();
	float getFPPower();
	float getQuality();
	
	bool isAddressEqual(device* device);
	bool isShortAddressEqual(device* device);
	
	//functions which contains the date: (easier to put as public)
	// timestamps to remember
	decaTime timePollSent;
	decaTime timePollReceived;
	decaTime timePollAckSent;
	decaTime timePollAckReceived;
	decaTime timeRangeSent;
	decaTime timeRangeReceived;
	
	void noteActivity();
	long getActivity(){ return _activity; };
	bool isInactive();


private:
	//device ID
	uint8_t      _ownAddress[8];
	uint8_t      _shortAddress[2];
	long         _activity;
	uint16_t _replyDelayTimeUS;
	short        _index;

	short 		_EKF_ID;
	
	int _range;
	int _RXPower;
	int _FPPower;
	int _quality;
	
	void randomShortAddress();
	
};


#endif