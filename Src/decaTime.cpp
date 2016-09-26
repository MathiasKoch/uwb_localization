#include "decaTime.h"
#include "hal.h"

decaTime::decaTime() {
	_timestamp = 0;
}

decaTime::decaTime(uint8_t data[]) {
	setTimestamp(data);
}

decaTime::decaTime(float timeUs) {
	setTime(timeUs);
}

decaTime::decaTime(const decaTime& copy) {
	setTimestamp(copy);
}

decaTime::decaTime(long value, float factorUs) {
	setTime(value, factorUs);
}

decaTime::~decaTime() { }

void decaTime::setTime(float timeUs) {
	// _timestamp in dtu (device time units)
	_timestamp = (long long int)(timeUs*TIME_RES_INV);
}

void decaTime::setTime(long value, float factorUs) {
	float tsValue = value*factorUs;
	tsValue = fmod(tsValue, TIME_OVERFLOW);
	// tsValue in us
	setTime(tsValue);
}

void decaTime::setTimestamp(uint8_t data[]) {
	_timestamp = 0;
	for(int i = 0; i < LEN_STAMP; i++) {
		_timestamp |= ((long long int)data[i] << (i*8));
	}
}

void decaTime::setTimestamp(const decaTime& copy) {
	_timestamp = copy.getTimestamp();
}

void decaTime::setTimestamp(int value) {
	_timestamp = value;
}

long long int decaTime::getTimestamp() const {
	return _timestamp;
}

decaTime& decaTime::wrap() {
	if(_timestamp < 0) {
		_timestamp += TIME_OVERFLOW;
	}
	return *this;
}

void decaTime::getTimestamp(uint8_t data[]) const {
	memset(data, 0, LEN_STAMP);
	for(int i = 0; i < LEN_STAMP; i++) {
		data[i] = (uint8_t)((_timestamp >> (i*8)) & 0xFF);
	}
}

float decaTime::getAsFloat() const {
	return fmod((float)_timestamp, TIME_OVERFLOW)*TIME_RES;
}

float decaTime::getAsMeters() const {
	return fmod((float)_timestamp, TIME_OVERFLOW)*DISTANCE_OF_RADIO;
}

decaTime& decaTime::operator=(const decaTime& assign) {
	if(this == &assign) {
		return *this;
	}
	_timestamp = assign.getTimestamp();
	return *this;
}

decaTime& decaTime::operator+=(const decaTime& add) {
	_timestamp += add.getTimestamp();
	return *this;
}

decaTime decaTime::operator+(const decaTime& add) const {
	return decaTime(*this) += add;
}

decaTime& decaTime::operator-=(const decaTime& sub) {
	_timestamp -= sub.getTimestamp();
	return *this;
}

decaTime decaTime::operator-(const decaTime& sub) const {
	return decaTime(*this) -= sub;
}

decaTime& decaTime::operator*=(float factor) {
	float tsValue = (float)_timestamp*factor;
	_timestamp = (long long int)tsValue;
	return *this;
}

decaTime decaTime::operator*(float factor) const {
	return decaTime(*this) *= factor;
}

decaTime& decaTime::operator*=(const decaTime& factor) {
	_timestamp *= factor.getTimestamp();
	return *this;
}

decaTime decaTime::operator*(const decaTime& factor) const {
	return decaTime(*this) *= factor;
}

decaTime& decaTime::operator/=(float factor) {
	_timestamp *= (1.0f/factor);
	return *this;
}

decaTime decaTime::operator/(float factor) const {
	return decaTime(*this) /= factor;
}

decaTime& decaTime::operator/=(const decaTime& factor) {
	_timestamp /= factor.getTimestamp();
	return *this;
}

decaTime decaTime::operator/(const decaTime& factor) const {
	return decaTime(*this) /= factor;
}

bool decaTime::operator==(const decaTime& cmp) const {
	return _timestamp == cmp.getTimestamp();
}

bool decaTime::operator!=(const decaTime& cmp) const {
	return !(*this == cmp);
}

void decaTime::print() {
	/*long long int number = _timestamp;
	unsigned char buf[64];
	int i = 0;
	
	if(number == 0) {
		printf2("%c", (char)'0');
		return;
	}
	
	
	while(number > 0) {
		uint64_t q = number/10;
		buf[i++] = number-q*10;
		number = q;
	}

	for(; i > 0; i--)
		printf2("%c", (char)(buf[i-1] < 10 ? '0'+buf[i-1] : 'A'+buf[i-1]-10));
	
	printf2("\r\n");*/
	
}

