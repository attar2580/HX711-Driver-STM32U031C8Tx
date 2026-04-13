/**
  ******************************************************************************
  * @file    config.h
  * @author  Fardeen
  * @brief   Centralized application tuning constants for the STM32 firmware.
  * @details This header groups all compile-time configuration values used by
  *          the load cell processing stack and related application modules.
  *          The values are intentionally kept in one place to simplify tuning
  *          and to avoid scattering hard-coded constants throughout the code.
  ******************************************************************************
  */

#ifndef CONFIG_H_
#define CONFIG_H_

/* --- Serial/UART Configuration --- */
#define CONFIG_UART_BAUD          9600

/* --- Ultrasonic (HC-SR04) Hardware Configuration --- */
#define CONFIG_US_TIMEOUT_LOOPS   100000UL
#define CONFIG_US_MAX_TICKS       30000U
#define CONFIG_US_MAX_DIST_CM     400U
#define CONFIG_US_TICK_DIVISOR    117U
#define CONFIG_US_TRIG_DELAY_US   15
#define CONFIG_US_PULSE_DELAY_US  2

/* --- Spatial Processing & Tuning --- */
#define CONFIG_SHAPE_TOLERANCE_CM 3
#define CONFIG_TUNE_X_CM          -2
#define CONFIG_TUNE_Y_CM          0
#define CONFIG_TUNE_Z_CM          1
#define CONFIG_NOISE_FLOOR_CM     2
#define CONFIG_CALIB_SAMPLES      10
#define CONFIG_CALIB_DELAY_MS     15

/* --- Load Cell (HX711) Configuration --- */
#define CONFIG_WEIGHT_CAL_FACTOR  22537L
#define CONFIG_WEIGHT_ZERO_FORCE  30L
#define CONFIG_WEIGHT_AVG_SAMPLES 8U
#define CONFIG_WEIGHT_ROUND_STEP  50L
#define CONFIG_TARE_SAMPLES       30U
#define CONFIG_NOISE_THRESH_G     15L
#define CONFIG_WEIGHT_DELAY_MS    5U

/* --- Volumetric Configuration --- */
#define CONFIG_DEFAULT_VOL_FACTOR 5000.0

#endif /* CONFIG_H_ */
