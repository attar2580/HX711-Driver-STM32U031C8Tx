# HX711-Driver-STM32U031C8Tx

Professional STM32 HAL firmware for an HX711-based load-cell measurement system migrated from an AVR bare-metal design.

## Overview

This firmware provides an interrupt-driven implementation for reading a load cell through the HX711 24-bit ADC and converting the raw samples into a stable weight value in kilograms.

The design is split into two layers:

- `HX711`: low-level driver for GPIO binding, data-ready handling, and raw ADC reads
- `hal_weight`: high-level processing for tare, averaging, calibration, noise suppression, and final kg conversion

The STM32U031C8Tx is configured through STM32CubeMX and uses the STM32 HAL only.

## Hardware Mapping

| Signal | STM32 Pin | Mode | Notes |
|---|---|---|---|
| HX711 SCK | PA7 | GPIO Output Push-Pull | Serial clock output to the HX711 |
| HX711 DOUT | PA6 | EXTI6, falling edge | Data-ready interrupt from the HX711 |

### Interrupt Configuration

- EXTI line: `EXTI4_15_IRQn`
- Trigger: falling edge on `PA6`
- ISR entry point: `EXTI4_15_IRQHandler()`
- HAL routing: `HAL_GPIO_EXTI_IRQHandler(HX711_DOUT_Pin)`

## Firmware Architecture

### Low-Level Driver (`HX711`)

Responsibilities:

- Bind the HX711 GPIOs
- Track the ready state using a `volatile` flag
- Read the 24-bit signed conversion value
- Perform explicit sign extension to `int32_t`
- Provide a short software microsecond delay for SCK timing

### High-Level Processing (`hal_weight`)

Responsibilities:

- Initialize the measurement layer
- Execute a blocking tare operation with `HAL_GetTick()` timeout protection
- Average multiple samples
- Apply calibration factor conversion
- Filter low-level noise and zero-force drift
- Return final weight in kilograms

## Data Flow

1. HX711 completes a conversion and drives `DOUT` low.
2. PA6 falling edge triggers EXTI6.
3. `HAL_GPIO_EXTI_Callback()` marks the HX711 as ready.
4. `hal_weight` consumes the ready sample and reads the 24-bit value.
5. The raw value is tare-corrected, calibrated, filtered, and returned as kilograms.

## Startup Sequence

At boot, the application performs the following steps:

1. `HAL_Init()`
2. System clock configuration
3. GPIO initialization
4. `HX711_Init()`
5. `HAL_Weight_Init()`
6. `HAL_Weight_Tare()`
7. Idle loop with `__WFI()`

## Timing and Safety Notes

- A software microsecond delay is used to shape HX711 SCK pulses.
- The 24-clock read sequence is wrapped in a critical section to prevent interrupt jitter from violating HX711 timing.
- The EXTI pending flag is cleared after the conversion read to prevent stale interrupts.
- `HAL_Weight_Tare()` exits safely on timeout and leaves offset at zero if the sensor does not respond.

## Configuration Files

### `Core/Inc/config.h`

Contains application tuning constants:

- `CONFIG_WEIGHT_CAL_FACTOR`
- `CONFIG_WEIGHT_ZERO_FORCE`
- `CONFIG_WEIGHT_AVG_SAMPLES`
- `CONFIG_WEIGHT_ROUND_STEP`
- `CONFIG_TARE_SAMPLES`
- `CONFIG_NOISE_THRESH_G`
- `CONFIG_WEIGHT_DELAY_MS`

### `Core/Inc/main.h`

Contains CubeMX-generated pin definitions:

- `HX711_DOUT_Pin` = `GPIO_PIN_6`
- `HX711_DOUT_GPIO_Port` = `GPIOA`
- `HX711_SCK_Pin` = `GPIO_PIN_7`
- `HX711_SCK_GPIO_Port` = `GPIOA`

## Project Structure

- `Core/Inc/HX711.h`
- `Core/Src/HX711.c`
- `Core/Inc/hal_weight.h`
- `Core/Src/hal_weight.c`
- `Core/Inc/config.h`
- `Core/Src/main.c`
- `Core/Src/stm32u0xx_it.c`

## Build Environment

### Required Tools

- STM32CubeMX-generated project
- STM32CubeIDE or VS Code with STM32 Cube build integration
- GNU Arm Embedded Toolchain
- CMake 3.22 or newer
- Ninja or the Cube-generated build backend

### Toolchain Notes

- Target MCU: `STM32U031C8Tx`
- Core: `Cortex-M0+`
- HAL: STM32U0 HAL
- Language standard: C11

### Example Build Flow

From the project root:

```bash
cube-cmake --build "D:/STM32 Workspace/hx711/build/Debug" --
```

## Weight Processing Behavior

### Tare

`HAL_Weight_Tare()` collects `CONFIG_TARE_SAMPLES` samples and computes an offset. If the HX711 fails to respond before the timeout expires, tare returns without hanging the MCU.

### Measurement

`HAL_Weight_Get_Kg()`:

- waits for new HX711 samples
- subtracts the tare offset
- averages multiple readings
- applies the calibration factor
- suppresses small zero-force values
- rounds the result to the configured step size

### Error Handling

The high-level layer exposes a latched error flag:

- `HAL_Weight_HasError()`
- `HAL_Weight_ClearError()`

## CubeMX Integration

The following user-code regions are used:

- `Core/Src/main.c`
  - `USER CODE BEGIN Includes`
  - `USER CODE BEGIN PV`
  - `USER CODE BEGIN 2`
  - `USER CODE BEGIN 4`
- `Core/Src/stm32u0xx_it.c`
  - generated EXTI handler is retained unchanged

## Operational Notes

- The main loop does not poll the HX711.
- The EXTI callback only marks data-ready state.
- The HX711 read path is strictly event-driven.
- The firmware is intended for a single HX711 channel/gain configuration using the 25-clock sequence.

## License

Follow the repository license and STMicroelectronics license headers included in the source files.
