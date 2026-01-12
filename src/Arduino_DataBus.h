/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 */
#ifndef _ARDUINO_DATABUS_H_
#define _ARDUINO_DATABUS_H_

#include <Arduino.h>

#include "YCbCr2RGB.h"

#define GFX_SKIP_OUTPUT_BEGIN -2
#define GFX_NOT_DEFINED -1
#define GFX_STR_HELPER(x) #x
#define GFX_STR(x) GFX_STR_HELPER(x)

#if defined(ESP32)
#define USE_FAST_PINIO   ///< Use direct PORT register access
#define HAS_PORT_SET_CLR ///< PORTs have set & clear registers
typedef uint32_t ARDUINOGFX_PORT_t;
#endif 

#ifdef USE_FAST_PINIO
typedef volatile ARDUINOGFX_PORT_t *PORTreg_t;
#endif

#if defined(ESP32)
#define SPI_DEFAULT_FREQ 40000000
#endif

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif
#define ATTR_UNUSED __attribute__((unused))

#define MSB_16(val) (((val) & 0xFF00) >> 8) | (((val) & 0xFF) << 8)
#define MSB_16_SET(var, val) \
  {                          \
    (var) = MSB_16(val);     \
  }
#define MSB_32_SET(var, val)                                  \
  {                                                           \
    uint8_t *v = (uint8_t *)&(val);                           \
    (var) = v[3] | (v[2] << 8) | (v[1] << 16) | (v[0] << 24); \
  }
#define MSB_32_16_16_SET(var, v1, v2)                                                                                   \
  {                                                                                                                     \
    (var) = (((uint32_t)v2 & 0xff00) << 8) | (((uint32_t)v2 & 0xff) << 24) | ((v1 & 0xff00) >> 8) | ((v1 & 0xff) << 8); \
  }
#define MSB_32_8_ARRAY_SET(var, a)                                  \
  {                                                                 \
    (var) = ((uint32_t)a[0] << 8 | a[1] | a[2] << 24 | a[3] << 16); \
  }

#if !defined(LITTLE_FOOT_PRINT)
#define GFX_INLINE __attribute__((always_inline)) inline
#else
#define GFX_INLINE inline
#endif // !defined(LITTLE_FOOT_PRINT)

#endif // _ARDUINO_DATABUS_H_
