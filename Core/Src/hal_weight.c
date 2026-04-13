/**
  ******************************************************************************
  * @file    hal_weight.c
  * @author  Fardeen
  * @brief   High-level HX711 load-cell processing implementation.
  * @details This module consumes raw HX711 samples and converts them into a
  *          stable user-facing weight value using tare, averaging, calibration,
  *          and simple noise rejection logic.
  ******************************************************************************
  */

#include "hal_weight.h"

static HX711_HandleTypeDef *s_hx711 = NULL;
static int32_t s_offset = 0L;
static int32_t s_last_stable_grams = 0L;
static volatile uint8_t s_error_flag = 0U;

/**
  * @brief  Wait for a ready sample until a timeout expires.
  * @param  start_tick  Tick value captured before waiting began.
  * @param  timeout_ms  Maximum time to wait in milliseconds.
  * @retval 1 when a sample is available, otherwise 0.
  */
static uint8_t HAL_Weight_WaitForSampleUntil(uint32_t start_tick,
                                             uint32_t timeout_ms)
{
  uint8_t ready = 0U;

  if (s_hx711 == NULL)
  {
    return 0U;
  }

  while (HX711_DataReady(s_hx711) == 0U)
  {
    if ((HAL_GetTick() - start_tick) >= timeout_ms)
    {
      break;
    }

    __WFI();
  }

  ready = (HX711_DataReady(s_hx711) != 0U) ? 1U : 0U;

  return ready;
}

/**
  * @brief  Blocking millisecond delay used during sample spacing.
  * @param  delay_ms  Delay duration in milliseconds.
  * @retval None
  */
static void HAL_Weight_WaitMs(uint32_t delay_ms)
{
  uint32_t start_tick = HAL_GetTick();

  while ((HAL_GetTick() - start_tick) < delay_ms)
  {
    __WFI();
  }
}

/**
  * @brief  Initialize the weight-processing layer.
  * @param  hx711  Initialized HX711 driver handle.
  * @retval None
  */
void HAL_Weight_Init(HX711_HandleTypeDef *hx711)
{
  s_hx711 = hx711;
  s_offset = 0L;
  s_last_stable_grams = 0L;
  s_error_flag = 0U;
}

/**
  * @brief  Read the current error flag.
  * @retval 1 if an error is latched, otherwise 0.
  */
uint8_t HAL_Weight_HasError(void)
{
  return s_error_flag;
}

/**
  * @brief  Clear the latched error flag.
  * @retval None
  */
void HAL_Weight_ClearError(void)
{
  s_error_flag = 0U;
}

/**
  * @brief  Perform a blocking tare cycle with timeout protection.
  * @details The tare routine waits for a stream of HX711 samples, averages the
  *          configured number of readings, and stores the resulting offset.
  *          If the HX711 does not respond in time, the function exits safely
  *          and leaves the offset at zero while latching an error state.
  * @retval None
  */
void HAL_Weight_Tare(void)
{
  uint32_t tare_start_tick;
  int64_t tare_sum;
  uint32_t sample_count;

  s_error_flag = 0U;
  s_offset = 0L;

  if (s_hx711 == NULL)
  {
    s_error_flag = 1U;
    return;
  }

  tare_start_tick = HAL_GetTick();

  if (HAL_Weight_WaitForSampleUntil(tare_start_tick, HAL_WEIGHT_TARE_TIMEOUT_MS) == 0U)
  {
    s_error_flag = 1U;
    s_offset = 0L;
    return;
  }

  (void)HX711_ReadRaw(s_hx711);

  tare_sum = 0;
  sample_count = 0U;

  while (sample_count < (uint32_t)CONFIG_TARE_SAMPLES)
  {
    if ((HAL_GetTick() - tare_start_tick) >= HAL_WEIGHT_TARE_TIMEOUT_MS)
    {
      s_error_flag = 1U;
      s_offset = 0L;
      return;
    }

    if (HAL_Weight_WaitForSampleUntil(tare_start_tick, HAL_WEIGHT_TARE_TIMEOUT_MS) == 0U)
    {
      s_error_flag = 1U;
      s_offset = 0L;
      return;
    }

    tare_sum += (int64_t)HX711_ReadRaw(s_hx711);
    sample_count++;
  }

  s_offset = (int32_t)(tare_sum / (int64_t)CONFIG_TARE_SAMPLES);
}

/**
  * @brief  Convert the latest HX711 readings into kilograms.
  * @details This function applies tare offset removal, calibration scaling,
  *          noise suppression, and rounding before returning the final value.
  * @retval Weight in kilograms.
  */
float HAL_Weight_Get_Kg(void)
{
  int64_t sum = 0;
  uint32_t collected = 0U;
  uint32_t sample_start_tick;
  int32_t avg_net;
  int32_t grams;

  if (s_hx711 == NULL)
  {
    s_error_flag = 1U;
    return 0.0f;
  }

  s_error_flag = 0U;

  while (collected < (uint32_t)CONFIG_WEIGHT_AVG_SAMPLES)
  {
    sample_start_tick = HAL_GetTick();

    if (HAL_Weight_WaitForSampleUntil(sample_start_tick, HAL_WEIGHT_SAMPLE_TIMEOUT_MS) == 0U)
    {
      s_error_flag = 1U;
      return 0.0f;
    }

    sum += (int64_t)(HX711_ReadRaw(s_hx711) - s_offset);
    collected++;

    if (CONFIG_WEIGHT_DELAY_MS != 0U)
    {
      HAL_Weight_WaitMs((uint32_t)CONFIG_WEIGHT_DELAY_MS);
    }
  }

  avg_net = (int32_t)(sum / (int64_t)collected);
  grams = (int32_t)(((int64_t)avg_net * 1000LL) / (int64_t)CONFIG_WEIGHT_CAL_FACTOR);

  if (grams < 0)
  {
    grams = (int32_t)(-grams);
  }

  if (grams < (int32_t)CONFIG_WEIGHT_ZERO_FORCE)
  {
    grams = 0;
  }

  {
    int32_t noise_diff;

    noise_diff = grams - s_last_stable_grams;

    if (noise_diff < 0)
    {
      noise_diff = (int32_t)(-noise_diff);
    }

    if (noise_diff > (int32_t)CONFIG_NOISE_THRESH_G)
    {
      s_last_stable_grams = grams;
    }

    grams = s_last_stable_grams;
  }

  grams = (int32_t)(((grams + (int32_t)(CONFIG_WEIGHT_ROUND_STEP / 2L)) /
                     (int32_t)CONFIG_WEIGHT_ROUND_STEP) *
                    (int32_t)CONFIG_WEIGHT_ROUND_STEP);

  return ((float)grams) / 1000.0f;
}
