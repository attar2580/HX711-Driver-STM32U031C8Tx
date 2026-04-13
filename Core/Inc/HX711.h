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

void HX711_Init(HX711_HandleTypeDef *hx711,
                GPIO_TypeDef *sck_port,
                uint16_t sck_pin,
                GPIO_TypeDef *dout_port,
                uint16_t dout_pin);

void HX711_OnDoutLowIsr(HX711_HandleTypeDef *hx711);
uint8_t HX711_DataReady(const HX711_HandleTypeDef *hx711);
int32_t HX711_ReadRaw(HX711_HandleTypeDef *hx711);

#endif /* HX711_H_ */
