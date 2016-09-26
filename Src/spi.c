/**
  ******************************************************************************
  * File Name          : SPI.c
  * Description        : This file provides code for the configuration
  *                      of the SPI instances.
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
#include "spi.h"

#include "gpio.h"
#include "dma.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

SPI_HandleTypeDef deca_spi;
SPI_HandleTypeDef client_spi;
DMA_HandleTypeDef hdma_deca_spi_rx;
DMA_HandleTypeDef hdma_deca_spi_tx;

/* DECAWAVE SPI init function */
void MX_DECA_SPI_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;


  /*Configure GPIO pins : DECA_RESET */
  GPIO_InitStruct.Pin = DECA_RESET_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DECA_RESET_PORT, &GPIO_InitStruct);

  /*Configure GPIO pins : DECA_IRQ */
  GPIO_InitStruct.Pin = DECA_IRQ_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DECA_IRQ_PORT, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(DECA_IRQ_EXTI, 0, 0);
  HAL_NVIC_EnableIRQ(DECA_IRQ_EXTI);


  deca_spi.Instance = DECA_SPI_INSTANCE;
  deca_spi.Init.Mode = SPI_MODE_MASTER;
  deca_spi.Init.Direction = SPI_DIRECTION_2LINES;
  deca_spi.Init.DataSize = SPI_DATASIZE_8BIT;
  deca_spi.Init.CLKPolarity = SPI_POLARITY_LOW;
  deca_spi.Init.CLKPhase = SPI_PHASE_1EDGE;
  deca_spi.Init.NSS = SPI_NSS_SOFT;
  deca_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  deca_spi.Init.FirstBit = SPI_FIRSTBIT_MSB;
  deca_spi.Init.TIMode = SPI_TIMODE_DISABLE;
  deca_spi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
  deca_spi.Init.CRCPolynomial = 7;
  deca_spi.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  deca_spi.Init.NSSPMode = SPI_NSS_PULSE_DISABLED;
  if (HAL_SPI_Init(&deca_spi) != HAL_OK)
  {
    Error_Handler();
  }
}
/* CLIENT SPI init function */
void MX_CLIENT_SPI_Init(void)
{

#if defined CLIENT_SPI_INSTANCE
  client_spi.Instance = CLIENT_SPI_INSTANCE;
  client_spi.Init.Mode = SPI_MODE_SLAVE;
  client_spi.Init.Direction = SPI_DIRECTION_2LINES;
  client_spi.Init.DataSize = SPI_DATASIZE_4BIT;
  client_spi.Init.CLKPolarity = SPI_POLARITY_LOW;
  client_spi.Init.CLKPhase = SPI_PHASE_1EDGE;
  client_spi.Init.NSS = SPI_NSS_SOFT;
  client_spi.Init.FirstBit = SPI_FIRSTBIT_MSB;
  client_spi.Init.TIMode = SPI_TIMODE_DISABLE;
  client_spi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
  client_spi.Init.CRCPolynomial = 7;
  client_spi.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  client_spi.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&client_spi) != HAL_OK)
  {
    Error_Handler();
  }
#endif

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();
  
    /**SPI1 GPIO Configuration */
    GPIO_InitStruct.Pin = SPI1_SCK_PIN|SPI1_MISO_PIN|SPI1_MOSI_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = SPI1_AF;
    HAL_GPIO_Init(SPI1_PORT, &GPIO_InitStruct);

    /*Configure GPIO pin : SPI1_CS */
    GPIO_InitStruct.Pin = DECA_CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DECA_CS_PORT, &GPIO_InitStruct);

    /*Configure GPIO pin Output Level - Decawave SPI Chip select */
    HAL_GPIO_WritePin(DECA_CS_PORT, DECA_CS_PIN, GPIO_PIN_RESET);

    /* Peripheral DMA init*/
  
    hdma_deca_spi_rx.Instance = DMA1_Channel2;
    hdma_deca_spi_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_deca_spi_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_deca_spi_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_deca_spi_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_deca_spi_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_deca_spi_rx.Init.Mode = DMA_NORMAL;
    hdma_deca_spi_rx.Init.Priority = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(&hdma_deca_spi_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(spiHandle,hdmarx,hdma_deca_spi_rx);

    hdma_deca_spi_tx.Instance = DMA1_Channel3;
    hdma_deca_spi_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_deca_spi_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_deca_spi_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_deca_spi_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_deca_spi_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_deca_spi_tx.Init.Mode = DMA_NORMAL;
    hdma_deca_spi_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_deca_spi_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(spiHandle,hdmatx,hdma_deca_spi_tx);

  /* USER CODE BEGIN SPI1_MspInit 1 */

  /* USER CODE END SPI1_MspInit 1 */
  }
  else if(spiHandle->Instance==SPI2)
  {
  /* USER CODE BEGIN SPI2_MspInit 0 */
#if defined CLIENT_SPI_INSTANCE
  /* USER CODE END SPI2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_SPI2_CLK_ENABLE();
  
    /**SPI2 GPIO Configuration */
    GPIO_InitStruct.Pin = SPI2_SCK_PIN|SPI2_MISO_PIN|SPI2_MOSI_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = SPI2_AF;
    HAL_GPIO_Init(SPI2_PORT, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI2_MspInit 1 */
#endif
  /* USER CODE END SPI2_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspDeInit 0 */

  /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();
  
    /**SPI1 GPIO Configuration    
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI 
    */
    HAL_GPIO_DeInit(SPI1_PORT, SPI1_SCK_PIN|SPI1_MISO_PIN|SPI1_MOSI_PIN);

    /* Peripheral DMA DeInit*/
    HAL_DMA_DeInit(spiHandle->hdmarx);
    HAL_DMA_DeInit(spiHandle->hdmatx);
  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
  }
  else if(spiHandle->Instance==SPI2)
  {
  /* USER CODE BEGIN SPI2_MspDeInit 0 */
#if defined CLIENT_SPI_INSTANCE
  /* USER CODE END SPI2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI2_CLK_DISABLE();
  
    /**SPI2 GPIO Configuration    
    PB13     ------> SPI2_SCK
    PB14     ------> SPI2_MISO
    PB15     ------> SPI2_MOSI 
    */
    HAL_GPIO_DeInit(SPI2_PORT, SPI2_SCK_PIN|SPI2_MISO_PIN|SPI2_MOSI_PIN);

  /* USER CODE BEGIN SPI2_MspDeInit 1 */
#endif
  /* USER CODE END SPI2_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */


void dw1000_spi_fast(void)
{
  deca_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  if (HAL_SPI_Init(&deca_spi) != HAL_OK)
  {
    Error_Handler();
  }
}

void dw1000_spi_slow(void)
{
  deca_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  if (HAL_SPI_Init(&deca_spi) != HAL_OK)
  {
    Error_Handler();
  }
}


/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
