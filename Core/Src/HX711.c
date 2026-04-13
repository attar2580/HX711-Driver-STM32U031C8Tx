#include "HX711.h"

static void HX711_DelayUs(uint32_t delay_us)
{
  uint32_t outer;
  uint32_t inner;
  const uint32_t loops_per_us = (SystemCoreClock / 4000000U) + 1U;

  for (outer = 0U; outer < delay_us; outer++)
  {
    for (inner = 0U; inner < loops_per_us; inner++)
    {
      __NOP();
    }
  }
}

static int32_t HX711_ReadSample(const HX711_HandleTypeDef *hx711)
{
  uint32_t bit_index;
  uint32_t sample_u32 = 0U;

  __disable_irq();

  for (bit_index = 0U; bit_index < 24U; bit_index++)
  {
    HAL_GPIO_WritePin(hx711->sck_port, hx711->sck_pin, GPIO_PIN_SET);
    HX711_DelayUs(1U);

    sample_u32 = (sample_u32 << 1U);
    if (HAL_GPIO_ReadPin(hx711->dout_port, hx711->dout_pin) == GPIO_PIN_SET)
    {
      sample_u32 |= 0x01U;
    }

    HAL_GPIO_WritePin(hx711->sck_port, hx711->sck_pin, GPIO_PIN_RESET);
    HX711_DelayUs(1U);
  }

  HAL_GPIO_WritePin(hx711->sck_port, hx711->sck_pin, GPIO_PIN_SET);
  HX711_DelayUs(1U);
  HAL_GPIO_WritePin(hx711->sck_port, hx711->sck_pin, GPIO_PIN_RESET);
  HX711_DelayUs(1U);

  __HAL_GPIO_EXTI_CLEAR_IT(hx711->dout_pin);

  __enable_irq();

  if ((sample_u32 & 0x00800000UL) != 0UL)
  {
    sample_u32 |= 0xFF000000UL;
  }

  return (int32_t)sample_u32;
}

void HX711_Init(HX711_HandleTypeDef *hx711,
                GPIO_TypeDef *sck_port,
                uint16_t sck_pin,
                GPIO_TypeDef *dout_port,
                uint16_t dout_pin)
{
  if (hx711 != NULL)
  {
    hx711->sck_port = sck_port;
    hx711->sck_pin = sck_pin;
    hx711->dout_port = dout_port;
    hx711->dout_pin = dout_pin;
    hx711->data_ready = 0U;

    HAL_GPIO_WritePin(hx711->sck_port, hx711->sck_pin, GPIO_PIN_SET);
    HAL_Delay(1U);

    HAL_GPIO_WritePin(hx711->sck_port, hx711->sck_pin, GPIO_PIN_RESET);
    HAL_Delay(1U);
  }
}

void HX711_OnDoutLowIsr(HX711_HandleTypeDef *hx711)
{
  if (hx711 != NULL)
  {
    hx711->data_ready = 1U;
  }
}

uint8_t HX711_DataReady(const HX711_HandleTypeDef *hx711)
{
  uint8_t ready = 0U;

  if (hx711 != NULL)
  {
    ready = (hx711->data_ready != 0U) ? 1U : 0U;
  }

  return ready;
}

int32_t HX711_ReadRaw(HX711_HandleTypeDef *hx711)
{
  int32_t sample = 0L;

  if ((hx711 != NULL) && (hx711->data_ready != 0U))
  {
    sample = HX711_ReadSample(hx711);
    hx711->data_ready = 0U;
  }

  return sample;
}
