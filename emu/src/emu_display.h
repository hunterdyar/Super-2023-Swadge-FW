#ifndef _EMU_DISPLAY_H_
#define _EMU_DISPLAY_H_

#include <stdint.h>
#include <stdbool.h>

#include "hal/spi_types.h"
#include "led_util.h"
#include "display.h"

/* Screen-specific configurations */
#if defined(CONFIG_ST7735_160x80)
    #define TFT_WIDTH         160
    #define TFT_HEIGHT         80
#elif defined(CONFIG_ST7789_240x135)
    #define TFT_WIDTH         240
    #define TFT_HEIGHT        135
#elif defined(CONFIG_ST7789_240x240)
    #define TFT_WIDTH         240
    #define TFT_HEIGHT        240
#elif defined(CONFIG_GC9307_240x280)
    #define TFT_WIDTH         280
    #define TFT_HEIGHT        240
#else
    #error "Please pick a screen size"
#endif

void lockDisplayMemoryMutex(void);
void unlockDisplayMemoryMutex(void);

uint32_t * getDisplayBitmap(uint16_t * width, uint16_t * height);
led_t * getLedMemory(uint8_t * numLeds);

void setDisplayBitmapMultiplier(uint8_t multiplier);

void deinitDisplayMemory(void);

#endif