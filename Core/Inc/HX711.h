/**
  ******************************************************************************
  * @file    HX711.h
  * @author  Fardeen
  * @brief   Public interface for the low-level HX711 driver.
  * @details The HX711 driver provides GPIO binding, ready-flag handling, and
  *          raw 24-bit conversion reading support for the HX711 ADC used by
  *          the load cell front-end.
  ******************************************************************************
  */

#ifndef HX711_H_
#define HX711_H_

#include <stdint.h>
#include "stm32u0xx_hal.h"

typedef struct
{
  GPIO_TypeDef *sck_port;
  uint16_t sck_pin;
  GPIO_TypeDef *dout_port;
  uint16_t dout_pin;
  volatile uint8_t data_ready;
} HX711_HandleTypeDef;

/**
  * @brief  Initialize the HX711 driver instance.
  * @param  hx711     Driver handle to initialize.
  * @param  sck_port  GPIO port connected to the HX711 SCK pin.
  * @param  sck_pin   GPIO pin connected to the HX711 SCK pin.
  * @param  dout_port GPIO port connected to the HX711 DOUT pin.
  * @param  dout_pin  GPIO pin connected to the HX711 DOUT pin.
  * @retval None
  */
void HX711_Init(HX711_HandleTypeDef *hx711,
                GPIO_TypeDef *sck_port,
                uint16_t sck_pin,
                GPIO_TypeDef *dout_port,
                uint16_t dout_pin);

/**
  * @brief  Mark the HX711 as ready from the EXTI callback context.
  * @param  hx711  Driver handle associated with the interrupt source.
  * @retval None
  */
void HX711_OnDoutLowIsr(HX711_HandleTypeDef *hx711);

/**
  * @brief  Query the current ready-state flag.
  * @param  hx711  Driver handle to inspect.
  * @retval 1 if a conversion is ready, otherwise 0.
  */
uint8_t HX711_DataReady(const HX711_HandleTypeDef *hx711);

/**
  * @brief  Read the latest raw HX711 sample.
  * @param  hx711  Driver handle to use for the conversion read.
  * @retval Sign-extended 24-bit raw ADC value in a 32-bit signed container.
  */
int32_t HX711_ReadRaw(HX711_HandleTypeDef *hx711);

#endif /* HX711_H_ */
