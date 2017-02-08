#ifndef _decaTime_H_INCLUDED
#define _decaTime_H_INCLUDED


#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>       /* fmod */
#include "deca_device_api.h"
#include "deca_regs.h"
#include "decawave.h"
#include "hal.h"

// Time resolution in micro-seconds of time based registers/values.
// Each bit in a timestamp counts for a period of approx. 15.65ps
#define TIME_RES 0.000015650040064103f
#define TIME_RES_INV 63897.6f

// Speed of radio waves [m/s] * timestamp resolution [~15.65ps] of DW1000
#define DISTANCE_OF_RADIO 0.0046917639786159f
#define DISTANCE_OF_RADIO_INV 213.139451293f

// time stamp byte length
#define LEN_STAMP 5


// timer/counter overflow (40 bits)
#define TIME_OVERFLOW 1099511627776

// time factors (relative to [us]) for setting delayed transceive
#define DW_SECONDS 1.0e6f
#define DW_MILLISECONDS 1.0e3f
#define DW_MICROSECONDS 1.0f
#define DW_NANOSECONDS 1.0e-3f




class decaTime {
public:
	decaTime();
	decaTime(long long int time);
	decaTime(float timeUs);
	decaTime(uint8_t data[]);
	decaTime(long value, float factorUs);
	decaTime(const decaTime& copy);
	~decaTime();
	
	void setTime(float timeUs);
	void setTime(long value, float factorUs);
	
	float getAsFloat() const;
	void  getAsBytes(uint8_t data[]) const;
	float getAsMeters() const;
	
	void          getTimestamp(uint8_t data[]) const;
	long long int getTimestamp() const;
	void          setTimestamp(uint8_t data[]);
	void          setTimestamp(const decaTime& copy);
	void          setTimestamp(int value);
	
	decaTime& wrap();
	
	decaTime& operator=(const decaTime& assign);
	decaTime& operator+=(const decaTime& add);
	decaTime operator+(const decaTime& add) const;
	decaTime& operator-=(const decaTime& sub);
	decaTime operator-(const decaTime& sub) const;
	decaTime& operator*=(float factor);
	decaTime operator*(const decaTime& factor) const;
	decaTime& operator*=(const decaTime& factor);
	decaTime operator*(float factor) const;
	decaTime& operator/=(float factor);
	decaTime operator/(float factor) const;
	decaTime& operator/=(const decaTime& factor);
	decaTime operator/(const decaTime& factor) const;
	bool operator==(const decaTime& cmp) const;
	bool operator!=(const decaTime& cmp) const;
	
	void print();

private:
	long long int _timestamp;
};

#endif
