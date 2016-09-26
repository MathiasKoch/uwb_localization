/*! ----------------------------------------------------------------------------
 * @file    deca_sleep.h
 * @brief   platform dependent sleep implementation
 *
 * @attention
 *
 * Copyright 2015 (c) DecaWave Ltd, Dublin, Ireland.
 *
 * All rights reserved.
 *
 * @author DecaWave
 */

#ifndef _DECA_SLEEP_H_
#define _DECA_SLEEP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hal.h"
#include "deca_device_api.h"


/*! ------------------------------------------------------------------------------------------------------------------
 * Function: deca_sleep()
 *
 * Wait for a given amount of time.
 * /!\ This implementation is designed for a single threaded application and is blocking.
 *
 * param  time_ms  time to wait in milliseconds
 */
void deca_sleep(unsigned int time_ms);

int readfromspi (uint16_t headerLength,
                 const uint8_t *headerBuffer,
                 uint32_t readlength,
                 uint8_t *readBuffer);

int writetospi (uint16_t headerLength,
                const uint8_t *headerBuffer,
                uint32_t bodylength,
                const uint8_t *bodyBuffer);

decaIrqStatus_t decamutexon(void);
void decamutexoff(decaIrqStatus_t s);

void reset_DW1000(void);

#ifdef __cplusplus
}
#endif

#endif /* _DECA_SLEEP_H_ */
