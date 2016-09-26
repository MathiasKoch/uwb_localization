
#ifndef _DECA_H_
#define _DECA_H_

#include <math.h>
#include "deca_device_api.h"
#include "deca_regs.h"
#include "decawave.h"
#include "hal.h"
#include "decaTime.h"


#define TX_ANT_DLY 16436
#define RX_ANT_DLY 16436
/*#define DEFAULT_ANT_DLY (TX_ANT_DLY+RX_ANT_DLY)*/
#define DEFAULT_ANT_DLY 32907

/* UWB microsecond (uus) to device time unit (dtu, around 15.65 ps) conversion factor.
 * 1 uus = 512 / 499.2 µs and 1 µs = 499.2 * 128 dtu. */
#define UUS_TO_DWT_TIME 65536

/* Speed of light in air, in metres per second. */
#define SPEED_OF_LIGHT 299702547




#define PRF DWT_PRF_64M
#define CHANNEL 5



float getReceiveQuality(void);
float getFirstPathPower(void);
float getReceivePower(void);
int isLOS(void);
void correctTimestamp(decaTime& timestamp);
void readrxtimestamp(decaTime& timestamp);
void get_rx_timestamp(decaTime& timestamp);
void readtxtimestamp(decaTime& timestamp);


extern const uint8_t BIAS_500_16[];
extern const uint8_t BIAS_500_64[];
extern const uint8_t BIAS_900_16[];
extern const uint8_t BIAS_900_64[];

#endif