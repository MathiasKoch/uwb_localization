
#include "hal.h"
#include "deca.h"


#define BIAS_500_16_ZERO 10
#define BIAS_500_64_ZERO 8
#define BIAS_900_16_ZERO 7
#define BIAS_900_64_ZERO 7

// range bias tables (500 MHz in [mm] and 900 MHz in [2mm] - to fit into bytes)
const uint8_t BIAS_500_16[] = {198, 187, 179, 163, 143, 127, 109, 84, 59, 31, 0,  36,  65,  84,  97,  106, 110, 112};
const uint8_t BIAS_500_64[] = {110, 105, 100, 93,  82,  69,  51,  27, 0,  21, 35, 42,  49,  62,  71,  76,  81,  86};
const uint8_t BIAS_900_16[] = {137, 122, 105, 88,  69,  47,  25,  0,  21, 48, 79, 105, 127, 147, 160, 169, 178, 197};
const uint8_t BIAS_900_64[] = {147, 133, 117, 99,  75,  50,  29,  0,  24, 45, 63, 76,  87,  98,  116, 122, 132, 142};

float getReceiveQuality(void) {
	dwt_rxdiag_t diag;
	dwt_readdiagnostics(&diag);
	return (float)diag.stdNoise / (float)diag.firstPathAmp2;
}

float getFirstPathPower(void) {
	float f1, f2, f3, N, A, corrFac;
	dwt_rxdiag_t diag;
	dwt_readdiagnostics(&diag);
	f1 = (float)diag.firstPathAmp1;
	f2 = (float)diag.firstPathAmp2;
	f3 = (float)diag.firstPathAmp3;
	N = (float)diag.rxPreamCount;
	if(PRF == DWT_PRF_16M) {
		A = 115.72;
		corrFac = 2.3334;
	}else{
		A = 121.74;
		corrFac = 1.1667;
	}
	float estFpPwr = 10.0 * log10((f1 * f1 + f2 * f2 + f3 * f3) / (N * N)) - A;
	if(estFpPwr <= -88) {
		return estFpPwr;
	} else {
		// approximation of Fig. 22 in user manual for dbm correction
		estFpPwr += (estFpPwr + 88) * corrFac;
	}
	return estFpPwr;
}


float getReceivePower(void) {
	unsigned long twoPower17 = 131072;
	float C, N, A, corrFac;

	dwt_rxdiag_t diag;
	dwt_readdiagnostics(&diag);
	
	C = (float)diag.maxGrowthCIR;
	N = (float)diag.rxPreamCount;
	if(PRF == DWT_PRF_16M) {
		A = 115.72;
		corrFac = 2.3334;
	}else{
		A = 121.74;
		corrFac = 1.1667;
	}
	float estRxPwr = 10.0 * log10((C * (float)twoPower17) / (N * N)) - A;
	if(estRxPwr <= -88) {
		return estRxPwr;
	} else {
		// approximation of Fig. 22 in user manual for dbm correction
		estRxPwr += (estRxPwr + 88) * corrFac;
	}
	return estRxPwr;
}




void correctTimestamp(decaTime& timestamp) {
	// base line dBm, which is -61, 2 dBm steps, total 18 data points (down to -95 dBm)
	float rxPowerBase = -(getReceivePower() + 61.0f) * 0.5f;
	int rxPowerBaseLow = (int)rxPowerBase;
	int rxPowerBaseHigh = rxPowerBaseLow + 1;
	if(rxPowerBaseLow < 0) {
		rxPowerBaseLow = 0;
		rxPowerBaseHigh = 0;
	} else if(rxPowerBaseHigh > 17) {
		rxPowerBaseLow = 17;
		rxPowerBaseHigh = 17;
	}
	// select range low/high values from corresponding table
	int rangeBiasHigh;
	int rangeBiasLow;
	if(CHANNEL == 4 || CHANNEL == 7) {
		// 900 MHz receiver bandwidth
		if(PRF == DWT_PRF_16M) {
			rangeBiasHigh = (rxPowerBaseHigh < BIAS_900_16_ZERO ? -BIAS_900_16[rxPowerBaseHigh] : BIAS_900_16[rxPowerBaseHigh]);
			rangeBiasHigh <<= 1;
			rangeBiasLow = (rxPowerBaseLow < BIAS_900_16_ZERO ? -BIAS_900_16[rxPowerBaseLow] : BIAS_900_16[rxPowerBaseLow]);
			rangeBiasLow <<= 1;
		} else if(PRF == DWT_PRF_64M) {
			rangeBiasHigh = (rxPowerBaseHigh < BIAS_900_64_ZERO ? -BIAS_900_64[rxPowerBaseHigh] : BIAS_900_64[rxPowerBaseHigh]);
			rangeBiasHigh <<= 1;
			rangeBiasLow = (rxPowerBaseLow < BIAS_900_64_ZERO ? -BIAS_900_64[rxPowerBaseLow] : BIAS_900_64[rxPowerBaseLow]);
			rangeBiasLow <<= 1;		
		} else {
			// TODO proper error handling
		}
	} else {
		// 500 MHz receiver bandwidth
		if(PRF == DWT_PRF_16M) {
			rangeBiasHigh = (rxPowerBaseHigh < BIAS_500_16_ZERO ? -BIAS_500_16[rxPowerBaseHigh] : BIAS_500_16[rxPowerBaseHigh]);
			rangeBiasLow = (rxPowerBaseLow < BIAS_500_16_ZERO ? -BIAS_500_16[rxPowerBaseLow] : BIAS_500_16[rxPowerBaseLow]);
		} else if(PRF == DWT_PRF_64M) {
			rangeBiasHigh = (rxPowerBaseHigh < BIAS_500_64_ZERO ? -BIAS_500_64[rxPowerBaseHigh] : BIAS_500_64[rxPowerBaseHigh]);
			rangeBiasLow = (rxPowerBaseLow < BIAS_500_64_ZERO ? -BIAS_500_64[rxPowerBaseLow] : BIAS_500_64[rxPowerBaseLow]);		
		} else {
			// TODO proper error handling
		}
	}
	// linear interpolation of bias values
	float rangeBias = rangeBiasLow + (rxPowerBase - rxPowerBaseLow) * (rangeBiasHigh - rangeBiasLow);
	// range bias [mm] to timestamp modification value conversion
	// If something in this function does NOT work, it is this line!!
	decaTime adjustmentTime;
	adjustmentTime.setTimestamp((int)(rangeBias*DISTANCE_OF_RADIO_INV*0.001f));
	// apply correction
	timestamp += adjustmentTime;
}


int isLOS(void){
	// Returns wether it is likely that the beacons are in line of sight, or non line of sight.
	// DISCLAIMER: This function is only a rule of thumb, not the definitive case!
	// In the interval 6-10 dB it is not really possible to tell wether it is a LOS case or NLOS case.
	float diff_power = getReceivePower()-getFirstPathPower();
	if(diff_power < 6)
		return 1;
	else if(diff_power > 10)
		return 0;
	else
		return -1;
}


void readrxtimestamp(decaTime& timestamp){
	get_rx_timestamp(timestamp);
	correctTimestamp(timestamp);
}

void get_rx_timestamp(decaTime& timestamp)
{
    uint8_t ts_tab[5];
    dwt_readrxtimestamp(ts_tab);
	timestamp.setTimestamp(ts_tab);
}

void readtxtimestamp(decaTime& timestamp)
{
    uint8_t ts_tab[5];
    dwt_readtxtimestamp(ts_tab);
	timestamp.setTimestamp(ts_tab);
}