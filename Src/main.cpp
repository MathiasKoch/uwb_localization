/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
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
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal.h"
#include "sys.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "dma.h"
#include "gpio.h"
#include "log.h"
#include "led.h"
#include "usb_device.h"
#include "decawave.h"
#include "deca_device_api.h"
#include "deca_regs.h"

#include "deca.h"
#include "mac.h"
#include "ranging.h"
#include "decaTime.h"
#include "packet.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

#define N_NODES 9
// {CPU_ID0, TYPE, ANT_DLY}
unsigned int boards[N_NODES][3] = {
  {0x87153806, ANCHOR, DEFAULT_ANT_DLY},
  {0x003a003a, ANCHOR, 32910},
  {0x00220039, ANCHOR, 32910},
  {0x001e0039, ANCHOR, 32889},
  {0x00300039, ANCHOR, 32923},
  {0x87153929, ANCHOR, DEFAULT_ANT_DLY},
  {0x0032003e, TAG,    DEFAULT_ANT_DLY},   
  {0xffffffff, ANCHOR, DEFAULT_ANT_DLY},
  {0xffffffff, ANCHOR, DEFAULT_ANT_DLY},
};

rangingClass ranging;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_LED_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_USB_DEVICE_Init();
  MX_IMU_I2C_Init();
  MX_DECA_SPI_Init();
  MX_CLIENT_SPI_Init();

  /* USER CODE BEGIN 2 */
  HAL_Delay(2000);

  LED_On(LED_1);

  int boardId = 0;

  // Finding the boad ID
  for(int i=0; i<N_NODES; i++){
    if(STM32_UUID[0] == boards[i][0]){
      boardId = i+1;
    }
  }


  uint32_t devID = dwt_readdevid() ;
  MPL_LOGD("Device ID: 0x%08x (%s)\n", (unsigned int)devID, devID==0xdeca0130?"OK":"ERROR");

  if(boardId == 0){
    while(1){
      MPL_LOGE("CPUID: 0x%08x (Board ID: %s)\n", (unsigned int)STM32_UUID[0], "UNKNOWN");
      HAL_Delay(1000);
    }
  }else{
    MPL_LOGD("CPUID: 0x%08x (Board ID: %02d)\n", (unsigned int)STM32_UUID[0], boardId);
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  uint8_t address[8];
  uint8_t cnt = 0;
  for(int i=0; i < 3; i++){
    for(int j=0; j < 4; j++){
      address[cnt++] = (uint8_t)(STM32_UUID[i] >> (j*8));
    }
  }
  


  if(boards[boardId-1][1] == ANCHOR){
    ranging.startAsAnchor(address, boards[boardId-1][2]);
  }else{
    ranging.startAsTag(address, boards[boardId-1][2]);
  }


  while(1){

    /*if (USART_GetITStatus(USARTx, USART_IT_RXNE)) {
        USART_ClearITPendingBit(USARTx, USART_IT_RXNE);
        //handle_input();
    }*/

    ranging.loop();
  }
  /* USER CODE END 3 */

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */


/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
