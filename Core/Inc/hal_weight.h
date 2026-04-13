/**
  ******************************************************************************
  * @file    hal_weight.h
  * @author  Fardeen
  * @brief   High-level load-cell processing interface.
  * @details This module implements tare handling, averaging, calibration,
  *          noise suppression, and user-facing weight conversion on top of the
  *          low-level HX711 driver.
  ******************************************************************************
  */

#ifndef HAL_WEIGHT_H_
#define HAL_WEIGHT_H_

#include <stdint.h>
#include "stm32u0xx_hal.h"
#include "HX711.h"
#include "config.h"

#define HAL_WEIGHT_TARE_TIMEOUT_MS    5000U
#define HAL_WEIGHT_SAMPLE_TIMEOUT_MS  1000U

/**
  * @brief  Attach the HX711 instance used by the weight-processing layer.
  * @param  hx711  Initialized HX711 driver handle.
  * @retval None
  */
void HAL_Weight_Init(HX711_HandleTypeDef *hx711);

/**
  * @brief  Perform a blocking tare operation with timeout protection.
  * @retval None
  */
void HAL_Weight_Tare(void);

/**
  * @brief  Obtain the latest processed weight in kilograms.
  * @retval Weight in kilograms as a floating-point value.
  */
float HAL_Weight_Get_Kg(void);

/**
  * @brief  Query the internal weight-layer error state.
  * @retval 1 when an error has occurred, otherwise 0.
  */
uint8_t HAL_Weight_HasError(void);

/**
  * @brief  Clear the internal weight-layer error state.
  * @retval None
  */
void HAL_Weight_ClearError(void);

#endif /* HAL_WEIGHT_H_ */
