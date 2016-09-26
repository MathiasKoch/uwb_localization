/**
  ******************************************************************************
  * @file    hal.h
  * @brief   HAL configuration file.  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HAL_H
#define __HAL_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "error.h"

#if defined  (STM32F302xC) 
  #include "stm32f3xx_hal.h"
  #include "stm32f3xx.h"


  /* USB pin defines */
  #define USB_PORT (GPIOA)
  #define USBDM_PIN (GPIO_PIN_11)
  #define USBDP_PIN (GPIO_PIN_12)
  #define USB_AF (GPIO_AF14_USB)
  #define USB_EXTI (USB_LP_CAN_RX0_IRQn)



  /* Decawave pin defines */
  #define DECA_SPI_INSTANCE (SPI1)

  #define DECA_CS_PORT (GPIOA)
  #define DECA_CS_PIN (GPIO_PIN_4)

  #define DECA_IRQ_PORT (GPIOB)
  #define DECA_IRQ_PIN (GPIO_PIN_0)
  #define DECA_IRQ_EXTI (EXTI0_IRQn)

  #define DECA_RESET_PORT (GPIOA)
  #define DECA_RESET_PIN (GPIO_PIN_8)



  /* Userbutton pin defines */
  #define USERBUTTON_PORT (GPIOA)
  #define USERBUTTON_PIN (GPIO_PIN_1)
  #define USERBUTTON_EXTI (EXTI1_IRQn)



  /* Enable client SPI (SPI2) */
  #define CLIENT_SPI_INSTANCE (SPI2)



  /* IMU pin defines */
  #define IMU_I2C_INSTANCE (I2C1)

  #define IMU_IRQ_PORT ()
  #define IMU_IRQ_PIN ()
  #define IMU_IRQ_EXTI ()





  /* SPI pin defines */
  #define SPI1_PORT (GPIOA)
  #define SPI1_SCK_PIN (GPIO_PIN_5)
  #define SPI1_MISO_PIN (GPIO_PIN_6)
  #define SPI1_MOSI_PIN (GPIO_PIN_7)
  #define SPI1_AF (GPIO_AF5_SPI1)

  #define SPI2_PORT (GPIOB)
  #define SPI2_SCK_PIN (GPIO_PIN_13)
  #define SPI2_MISO_PIN (GPIO_PIN_14)
  #define SPI2_MOSI_PIN (GPIO_PIN_15)
  #define SPI2_AF (GPIO_AF5_SPI2)

  #define DMA1_CHAN2_EXTI (DMA1_Channel2_IRQn)
  #define DMA1_CHAN3_EXTI (DMA1_Channel3_IRQn)


  /* I2C pin defines */
  #define I2C1_PORT (GPIOB)
  #define I2C1_SCL_PIN (GPIO_PIN_6)
  #define I2C1_SDA_PIN (GPIO_PIN_7)
  #define I2C1_AF (GPIO_AF4_I2C1)



  /*
  * The STM32 factory-programmed UUID memory.
  * Three values of 32 bits each starting at this address
  * Use like this: STM32_UUID[0], STM32_UUID[1], STM32_UUID[2]
  */
  #define STM32_UUID ((uint32_t *)0x1FFFF7AC)






#elif defined (STM32F10X_MD)
  #include "stm32f1xx_hal.h"
  #include "stm32f1xx.h"


















#elif defined (STM32F072xB)
  #include "stm32f0xx_hal.h"
  #include "stm32f0xx.h"


  /* USB pin defines */
  #define USB_AF (GPIO_AF14_USB)
  #define USB_EXTI (USB_IRQn)


  /* Decawave pin defines */
  #define DECA_SPI_INSTANCE (SPI1)

  #define DECA_CS_PORT (GPIOA)
  #define DECA_CS_PIN (GPIO_PIN_4)

  #define DECA_IRQ_PORT (GPIOB)
  #define DECA_IRQ_PIN (GPIO_PIN_0)
  #define DECA_IRQ_EXTI (EXTI0_1_IRQn)

  #define DECA_RESET_PORT (GPIOB)
  #define DECA_RESET_PIN (GPIO_PIN_12)



  /* Userbutton pin defines */
  #define USERBUTTON_PORT (GPIOA)
  #define USERBUTTON_PIN (GPIO_PIN_1)
  #define USERBUTTON_EXTI (EXTI0_1_IRQn)


  /* IMU/PRES pin defines */
  #define IMU_I2C_INSTANCE (I2C1)

  #define IMU_IRQ_PORT ()
  #define IMU_IRQ_PIN ()
  #define IMU_IRQ_EXTI ()


  /* SPI pin defines */
  #define SPI1_PORT (GPIOA)
  #define SPI1_SCK_PIN (GPIO_PIN_5)
  #define SPI1_MISO_PIN (GPIO_PIN_6)
  #define SPI1_MOSI_PIN (GPIO_PIN_7)
  #define SPI1_AF (GPIO_AF0_SPI1)

  #define DMA1_CHAN2_3_EXTI (DMA1_Channel2_3_IRQn)

  /* I2C pin defines */
  #define I2C1_PORT (GPIOB)
  #define I2C1_SCL_PIN (GPIO_PIN_6)
  #define I2C1_SDA_PIN (GPIO_PIN_7)
  #define I2C1_AF (GPIO_AF1_I2C1)
  

  /*
  * The STM32 factory-programmed UUID memory.
  * Three values of 32 bits each starting at this address
  * Use like this: STM32_UUID[0], STM32_UUID[1], STM32_UUID[2]
  */
  #define STM32_UUID ((uint32_t *)0x1FFFF7AC)



#else
  #error "Please first define the target STM32Fxxx device used in your application (in a Makefile define)"
#endif

    
#ifdef __cplusplus
}
#endif

#endif /* __HAL_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
