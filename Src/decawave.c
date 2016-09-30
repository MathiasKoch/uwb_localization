/*! ----------------------------------------------------------------------------
 * @file    deca_sleep.c
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

#include "decawave.h"


SPI_HandleTypeDef deca_spi;


void deca_sleep(unsigned int time_ms)
{
    HAL_Delay(time_ms);
}



int readfromspi (uint16_t headerLength,
                 const uint8_t *headerBuffer,
                 uint32_t readlength,
                 uint8_t *readBuffer) {


	HAL_GPIO_WritePin(DECA_CS_PORT, DECA_CS_PIN, GPIO_PIN_RESET);

	if(HAL_SPI_Transmit_DMA(&deca_spi, (uint8_t*)headerBuffer, headerLength) != HAL_OK){
		goto error;
	}


	if(HAL_SPI_Receive_DMA(&deca_spi, (uint8_t*)readBuffer, readlength) != HAL_OK){
		goto error;
	}


	HAL_GPIO_WritePin(DECA_CS_PORT, DECA_CS_PIN, GPIO_PIN_SET);
	return 0;

error:
	HAL_GPIO_WritePin(DECA_CS_PORT, DECA_CS_PIN, GPIO_PIN_SET);
	return -1;
}


int writetospi (uint16_t headerLength,
                const uint8_t *headerBuffer,
                uint32_t bodylength,
                const uint8_t *bodyBuffer) {

	HAL_GPIO_WritePin(DECA_CS_PORT, DECA_CS_PIN, GPIO_PIN_RESET);
	
	if(HAL_SPI_Transmit_DMA(&deca_spi, (uint8_t*)headerBuffer, headerLength) != HAL_OK){
		goto error;
	}
	

	if(HAL_SPI_Transmit_DMA(&deca_spi, (uint8_t*)bodyBuffer, bodylength) != HAL_OK){
		goto error;
	}

	HAL_GPIO_WritePin(DECA_CS_PORT, DECA_CS_PIN, GPIO_PIN_SET);
	return 0;

error:
	HAL_GPIO_WritePin(DECA_CS_PORT, DECA_CS_PIN, GPIO_PIN_SET);
	return -1;
}



// ---------------------------------------------------------------------------
//
// NB: The purpose of this file is to provide for microprocessor interrupt enable/disable, this is used for 
//     controlling mutual exclusion from critical sections in the code where interrupts and background 
//     processing may interact.  The code using this is kept to a minimum and the disabling time is also 
//     kept to a minimum, so blanket interrupt disable may be the easiest way to provide this.  But at a
//     minimum those interrupts coming from the decawave device should be disabled/re-enabled by this activity.
//
//     In porting this to a particular microprocessor, the implementer may choose to use #defines in the
//     deca_irq.h include file to map these calls transparently to the target system.  Alternatively the 
//     appropriate code may be embedded in the functions provided below.
//
//     This mutex dependent on HW port.
//	   If HW port uses EXT_IRQ line to receive ready/busy status from DW1000 then mutex should use this signal
//     If HW port not use EXT_IRQ line (i.e. SW polling) then no necessary for decamutex(on/off)
//
//	   For critical section use this mutex instead
//	   __save_intstate()
//     __restore_intstate()
// ---------------------------------------------------------------------------


/*! ------------------------------------------------------------------------------------------------------------------
 * Function: decamutexon()
 *
 * Description: This function should disable interrupts. This is called at the start of a critical section
 * It returns the irq state before disable, this value is used to re-enable in decamutexoff call
 *
 * Note: The body of this function is defined in deca_mutex.c and is platform specific
 *
 * input parameters:	
 *
 * output parameters
 *
 * returns the state of the DW1000 interrupt
 */
decaIrqStatus_t decamutexon(void)           
{
	decaIrqStatus_t s = EXTI_GetITEnStatus(DECA_IRQ_EXTI);

	if(s) {
		HAL_NVIC_DisableIRQ(DECA_IRQ_EXTI); //disable the external interrupt line
	}
	return s ;   // return state before disable, value is used to re-enable in decamutexoff call
}

/*! ------------------------------------------------------------------------------------------------------------------
 * Function: decamutexoff()
 *
 * Description: This function should re-enable interrupts, or at least restore their state as returned(&saved) by decamutexon 
 * This is called at the end of a critical section
 *
 * Note: The body of this function is defined in deca_mutex.c and is platform specific
 *
 * input parameters:	
 * @param s - the state of the DW1000 interrupt as returned by decamutexon
 *
 * output parameters
 *
 * returns the state of the DW1000 interrupt
 */
void decamutexoff(decaIrqStatus_t s)        // put a function here that re-enables the interrupt at the end of the critical section
{
	if(s) { //need to check the port state as we can't use level sensitive interrupt on the STM ARM
		HAL_NVIC_EnableIRQ(DECA_IRQ_EXTI);
	}
}


void reset_DW1000(void){
	GPIO_InitTypeDef GPIO_InitStruct;

	// Enable GPIO used for DW1000 reset
	GPIO_InitStruct.Pin = DECA_RESET_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(DECA_RESET_PORT, &GPIO_InitStruct);

	//drive the RSTn pin low
	HAL_GPIO_WritePin(DECA_RESET_PORT, DECA_RESET_PIN, GPIO_PIN_RESET);

	//put the pin back to tri-state ... as input
	GPIO_InitStruct.Pin = DECA_RESET_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(DECA_RESET_PORT, &GPIO_InitStruct);

    deca_sleep(2);
}