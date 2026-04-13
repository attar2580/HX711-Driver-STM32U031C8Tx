/* *****************************************************************
 * File Name:       hal_weight.h
 * Description:     Hardware Abstraction Layer for the Load Cell
 * ***************************************************************** */

#ifndef HAL_WEIGHT_H_
#define HAL_WEIGHT_H_

#include <stdint.h>
#include "stm32u0xx_hal.h"
#include "HX711.h"
#include "config.h"

#define HAL_WEIGHT_TARE_TIMEOUT_MS    5000U
#define HAL_WEIGHT_SAMPLE_TIMEOUT_MS  1000U

void HAL_Weight_Init(HX711_HandleTypeDef *hx711);
void HAL_Weight_Tare(void);
float HAL_Weight_Get_Kg(void);

uint8_t HAL_Weight_HasError(void);
void HAL_Weight_ClearError(void);

#endif /* HAL_WEIGHT_H_ */
