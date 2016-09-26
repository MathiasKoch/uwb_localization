/**
  ******************************************************************************
  * File Name          : LED.c
  * Description        : This file provides code for the configuration
  *                      of the LEDs.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "led.h"


/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* LED GPIO init function */
void MX_LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/*Configure GPIO pins : LEDS */
	GPIO_InitStruct.Pin = LED_1_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED_1_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LED_2_PIN;
	HAL_GPIO_Init(LED_2_PORT, &GPIO_InitStruct);

#if defined (LED_CHARGING_PORT) && defined (LED_CHARGING_PIN)
	GPIO_InitStruct.Pin = LED_CHARGING_PIN;
	HAL_GPIO_Init(LED_CHARGING_PORT, &GPIO_InitStruct);
#endif

	LED_Set(LED_ALL, GPIO_PIN_RESET);
}

void LED_Set(led_t led, GPIO_PinState state)
{
	switch (led)
	{
	case LED_1:
		HAL_GPIO_WritePin(LED_1_PORT, LED_1_PIN, state);
		break;
	case LED_2:
		HAL_GPIO_WritePin(LED_2_PORT, LED_2_PIN, state);
		break;
	case LED_CHARGING:
#if defined (LED_CHARGING_PORT) && defined (LED_CHARGING_PIN)
		HAL_GPIO_WritePin(LED_CHARGING_PORT, LED_CHARGING_PIN, state);
#endif
		break;
	case LED_ALL:
		// Cannot set multiple LEDs at once.
		break;
	default:
		// Do nothing for undefined led number
		break;
	}
}

void LED_On(led_t led)
{
	switch (led)
	{
	case LED_1:
		HAL_GPIO_WritePin(LED_1_PORT, LED_1_PIN, GPIO_PIN_SET);
		break;
	case LED_2:
		HAL_GPIO_WritePin(LED_2_PORT, LED_2_PIN, GPIO_PIN_SET);
		break;
	case LED_CHARGING:
#if defined (LED_CHARGING_PORT) && defined (LED_CHARGING_PIN)
		HAL_GPIO_WritePin(LED_CHARGING_PORT, LED_CHARGING_PIN, GPIO_PIN_SET);
#endif
		break;
	case LED_ALL:
		HAL_GPIO_WritePin(LED_1_PORT, LED_1_PIN, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED_2_PORT, LED_2_PIN, GPIO_PIN_SET);
#if defined (LED_CHARGING_PORT) && defined (LED_CHARGING_PIN)
		HAL_GPIO_WritePin(LED_CHARGING_PORT, LED_CHARGING_PIN, GPIO_PIN_SET);
#endif
		break;
	default:
		// do nothing for undefined led number
		break;
	}
}

void LED_Off(led_t led)
{
	switch (led)
	{
	case LED_1:
		HAL_GPIO_WritePin(LED_1_PORT, LED_1_PIN, GPIO_PIN_RESET);
		break;
	case LED_2:
		HAL_GPIO_WritePin(LED_2_PORT, LED_2_PIN, GPIO_PIN_RESET);
		break;
	case LED_CHARGING:
#if defined (LED_CHARGING_PORT) && defined (LED_CHARGING_PIN)
		HAL_GPIO_WritePin(LED_CHARGING_PORT, LED_CHARGING_PIN, GPIO_PIN_RESET);
#endif
		break;
	case LED_ALL:
		HAL_GPIO_WritePin(LED_1_PORT, LED_1_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED_2_PORT, LED_2_PIN, GPIO_PIN_RESET);
#if defined (LED_CHARGING_PORT) && defined (LED_CHARGING_PIN)
		HAL_GPIO_WritePin(LED_CHARGING_PORT, LED_CHARGING_PIN, GPIO_PIN_RESET);
#endif
		break;
	default:
		// do nothing for undefined led number
		break;
	}
}

void LED_Toggle(led_t led)
{
	switch (led)
	{
	case LED_1:
		HAL_GPIO_TogglePin(LED_1_PORT, LED_1_PIN);
		break;
	case LED_2:
		HAL_GPIO_TogglePin(LED_2_PORT, LED_2_PIN);
		break;
	case LED_CHARGING:
#if defined (LED_CHARGING_PORT) && defined (LED_CHARGING_PIN)
		HAL_GPIO_TogglePin(LED_CHARGING_PORT, LED_CHARGING_PIN);
#endif
		break;
	case LED_ALL:
		HAL_GPIO_TogglePin(LED_1_PORT, LED_1_PIN);
		HAL_GPIO_TogglePin(LED_2_PORT, LED_2_PIN);
#if defined (LED_CHARGING_PORT) && defined (LED_CHARGING_PIN)
		HAL_GPIO_TogglePin(LED_CHARGING_PORT, LED_CHARGING_PIN);
#endif
		break;
	default:
		// do nothing for undefined led number
		break;
	}
}

GPIO_PinState LED_Get(led_t led)
{
	switch (led)
	{
	case LED_1:
		return HAL_GPIO_ReadPin(LED_1_PORT, LED_1_PIN);
	case LED_2:
		return HAL_GPIO_ReadPin(LED_2_PORT, LED_2_PIN);
	case LED_CHARGING:
#if defined (LED_CHARGING_PORT) && defined (LED_CHARGING_PIN)
		return HAL_GPIO_ReadPin(LED_CHARGING_PORT, LED_CHARGING_PIN);
#endif
	case LED_ALL:
		// Cannot read more than one at a time.
		break;
	default:
		// Do nothing for undefined led number
		break;
	}
	return 0;
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
